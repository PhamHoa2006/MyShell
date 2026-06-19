// ============================================================================
// process.h — Process Management
// ============================================================================
// This is the MOST IMPORTANT file for your OS course!
//
// Commands to implement:
//   start_foreground <exe>   — Start a process and WAIT for it to finish
//   start_background <exe>   — Start a process and return to shell immediately
//   terminate <PID>          — Kill a process by its PID
//   list_processes           — List all running processes on the system
//   list                     — List the shell's own background processes
//                              (with PID, Cmd name, Status)
//   suspend <PID>            — Pause a process (suspend all its threads)
//   resume <PID>             — Unpause a process (resume all its threads)
//
// CTRL+C Handling:
//   Use SetConsoleCtrlHandler() to catch Ctrl+C and kill only the running
//   foreground child process, without terminating the shell itself.
//
// Reference: ../Operating-System-Projects/Feature/process.h
// ============================================================================

#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

// Info struct for tracking background processes launched by this shell
struct ProcessInfo {
    DWORD pid;
    std::string name;
    std::string status;
};

// Global handle to the current foreground child process (used by CTRL+C handler)
inline HANDLE g_hForegroundProcess = NULL;

// --- CTRL+C Handler ---
inline BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_C_EVENT) {
        if (g_hForegroundProcess != NULL) {
            std::cout << "\n[Ctrl+C] Foreground process interrupted. Terminating it..." << std::endl;
            TerminateProcess(g_hForegroundProcess, 0);
            g_hForegroundProcess = NULL;
        } else {
            std::cout << "\n[Ctrl+C] Ignored. No foreground process running." << std::endl;
        }
        return TRUE; // Tell Windows we handled it; do NOT kill the shell.
    }
    return FALSE; // Let default behavior handle other signals
}

// Compare function for sorting processes by name
inline bool compareProcessName(const std::pair<DWORD, std::string>& a, const std::pair<DWORD, std::string>& b) {
    return a.second < b.second;
}

class ProcessManager
{
public:
    void startProcessForeground(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            std::cout << "Usage: start_foreground <executable_path>" << std::endl;
            return;
        }
        
        std::string pathStr = joinArgs(args);
        
        // We need a modifiable copy for CreateProcess
        char* path = new char[pathStr.length() + 1];
        strcpy(path, pathStr.c_str());

        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);

        std::cout << "Initializing new process (Foreground)..." << std::endl;

        if (CreateProcessA(
            nullptr, path, nullptr, nullptr, FALSE,
            CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi
        )) {
            std::cout << "New process is running, shell stops ..." << std::endl;
            
            // Register the running foreground process
            g_hForegroundProcess = pi.hProcess;

            // FOREGROUND: Use WaitForSingleObject to freeze shell indefinitely.
            WaitForSingleObject(pi.hProcess, INFINITE);
            
            // Unregister since it finished
            g_hForegroundProcess = NULL;

            std::cout << "New process was terminated, Shell is now working back!" << std::endl;

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            std::cout << "Error! Can't open process! Error code: " << GetLastError() << std::endl;
        }
        delete[] path;
    }

    void startProcessBackground(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            std::cout << "Usage: start_background <executable_path>" << std::endl;
            return;
        }
        
        std::string pathStr = joinArgs(args);
        
        char* path = new char[pathStr.length() + 1];
        strcpy(path, pathStr.c_str());

        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);

        std::cout << "Initializing new process (Background)..." << std::endl;

        if (CreateProcessA(
            nullptr, path, nullptr, nullptr, FALSE,
            CREATE_NEW_CONSOLE, nullptr, nullptr,
            &si, &pi
        )) {
            std::cout << "Successfully, new process is running in background! PID: " << pi.dwProcessId << std::endl;
            
            // Save to tracking table
            ProcessInfo info;
            info.pid = pi.dwProcessId;
            info.name = pathStr;
            info.status = "Running";
            myBackgroundProcesses.push_back(info);
            
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            std::cout << "Error! Can't open process! Error code: " << GetLastError() << std::endl;
        }
        delete[] path;
    }

    void terminateProcess(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            std::cout << "Usage: terminate <PID>" << std::endl;
            return;
        }
        
        DWORD targetPID = std::stoul(args[0]);
        std::cout << "Trying to terminate " << targetPID << " ..." << std::endl;

        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, targetPID);

        if (hProcess == nullptr){
            std::cout << "Fail to open process PID " << targetPID << "\nError code: " << GetLastError() << std::endl;
        } else {
            // Successfully opened target process => terminate immediately
            if (TerminateProcess(hProcess, 0)){
                std::cout << "Successfully terminated PID " << targetPID << std::endl;

                // Update status in tracking table
                for (auto& process : myBackgroundProcesses) {
                    if (process.pid == targetPID) {
                        process.status = "Terminated";
                        break;
                    }
                }
            } else {
                std::cout << "Fail to terminate PID " << targetPID << "\nError code: " << GetLastError() << std::endl;
            }
            CloseHandle(hProcess);
        }
    }

    void listProcesses(const std::vector<std::string>& args)
    {
        std::cout << "Trying to snapshot all existing processes ..." << std::endl;
        
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == nullptr || hProcessSnap == INVALID_HANDLE_VALUE){
            std::cout << "E02: Can't snapshot processes!" << std::endl;
            return;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        std::vector<std::pair<DWORD, std::string>> processList;

        if (Process32First(hProcessSnap, &pe32)){
            do{
                processList.push_back(std::make_pair(pe32.th32ProcessID, pe32.szExeFile));
            } while (Process32Next(hProcessSnap, &pe32));
        } else {
            std::cout << "E03: Can't read the first process!" << std::endl;
        }

        CloseHandle(hProcessSnap);
        std::sort(processList.begin(), processList.end(), compareProcessName);
        
        std::cout << "--- All Processes ---" << std::endl;
        for (const auto& process : processList){
            std::cout << process.first << "\t- Name: " << process.second << std::endl;
        }
        std::cout << "Finish get list of All Process. Size: " << processList.size() << std::endl;
    }

    void listBackgroundProcesses(const std::vector<std::string>& args)
    {
        std::cout << "\n--- Shell Background Processes ---" << std::endl;
        if (myBackgroundProcesses.empty()) {
            std::cout << "No background processes running." << std::endl;
        } else {
            for (const auto& process : myBackgroundProcesses) {
                std::cout << "PID: " << process.pid 
                     << "\t| Status: " << process.status 
                     << "\t| Name: " << process.name << std::endl;
            }
        }
        std::cout << "----------------------------------\n" << std::endl;
    }

    void suspendProcess(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            std::cout << "Usage: suspend <PID>" << std::endl;
            return;
        }
        
        DWORD targetPID = std::stoul(args[0]);
        std::cout << "Trying to freeze: " << targetPID << "..." << std::endl;
        
        HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnap == nullptr || hThreadSnap == INVALID_HANDLE_VALUE){
            std::cout << "E01: Can't snapshot threads!" << std::endl;
            return;
        }

        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        int frozenCount = 0;

        if (Thread32First(hThreadSnap, &te32)){
            do{
                if (te32.th32OwnerProcessID == targetPID){
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread != nullptr){
                        SuspendThread(hThread);
                        CloseHandle(hThread);
                        frozenCount++;
                        std::cout << "Freezed thread: " << te32.th32ThreadID << std::endl;   
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }

        CloseHandle(hThreadSnap);

        std::cout << "Finished freezing PID " << targetPID << "\nTotal threads frozen: " << frozenCount << std::endl;

        // Update status in tracking table
        for (auto& process : myBackgroundProcesses) {
            if (process.pid == targetPID) {
                process.status = "Suspended";
                break;
            }
        }
    }

    void resumeProcess(const std::vector<std::string>& args)
    {
        if (args.empty()) {
            std::cout << "Usage: resume <PID>" << std::endl;
            return;
        }
        
        DWORD targetPID = std::stoul(args[0]);
        std::cout << "Trying to resume " << targetPID << " ..." << std::endl;

        // Snapshot of all threads
        HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnap == nullptr || hThreadSnap == INVALID_HANDLE_VALUE){
            std::cout << "E04: Can't snapshot threads!" << std::endl;
            return;
        }

        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);
        int resumedCount = 0;

        // Loop through every threads, and resume all that fit targetPID
        if (Thread32First(hThreadSnap, &te32)){
            do{
                if (te32.th32OwnerProcessID == targetPID){
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                    if (hThread != nullptr){
                        DWORD prevCount;
                        do{
                            prevCount = ResumeThread(hThread);
                        } while (prevCount > 1);

                        CloseHandle(hThread);
                        resumedCount++;
                        std::cout << "Activated thread: " << te32.th32ThreadID << std::endl;
                    }
                }
            } while (Thread32Next(hThreadSnap, &te32));
        }

        CloseHandle(hThreadSnap);
        std::cout << "Finished resuming PID " << targetPID << "\nTotal threads resumed: " << resumedCount << std::endl;

        // Update status in tracking table
        for (auto& process : myBackgroundProcesses) {
            if (process.pid == targetPID) {
                process.status = "Running";
                break;
            }
        }
    }

private:
    // Helper function to combine arguments into a single command line string
    std::string joinArgs(const std::vector<std::string>& args) {
        if (args.empty()) return "";
        std::string result = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            result += " " + args[i];
        }
        return result;
    }

    std::vector<ProcessInfo> myBackgroundProcesses;
};

#endif // PROCESS_H
