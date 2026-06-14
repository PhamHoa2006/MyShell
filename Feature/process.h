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
//   list_children            — List child processes of this shell
//   suspend <PID>            — Pause a process (suspend all its threads)
//   resume <PID>             — Unpause a process (resume all its threads)
//
// Libraries needed:
//   #include <windows.h>     — CreateProcess, TerminateProcess, etc.
//   #include <tlhelp32.h>    — Toolhelp API for process/thread enumeration
//   #include <vector>
//   #include <string>
//   #include <iostream>
//
// Key Windows structures:
//   STARTUPINFOA si          — Initialize with: si.cb = sizeof(si)
//   PROCESS_INFORMATION pi   — Output: pi.hProcess, pi.hThread, pi.dwProcessId
//   PROCESSENTRY32 pe        — Initialize with: pe.dwSize = sizeof(pe)
//   THREADENTRY32 te         — Initialize with: te.dwSize = sizeof(te)
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
#include <set>

class ProcessManager
{
public:
    // --- start_foreground <exe> ---
    // Create a new process and WAIT for it to finish before returning to shell
    // APIs: CreateProcessA() + WaitForSingleObject(pi.hProcess, INFINITE)
    void startProcessForeground(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check args is not empty
        // 2. Build command string from args
        // 3. Initialize STARTUPINFOA and PROCESS_INFORMATION
        // 4. Call CreateProcessA(...)
        // 5. If success: WaitForSingleObject(pi.hProcess, INFINITE)
        // 6. CloseHandle(pi.hProcess) and CloseHandle(pi.hThread)
        // 7. If fail: print error with GetLastError()
    }

    // --- start_background <exe> ---
    // Create a new process and return to shell IMMEDIATELY
    // APIs: CreateProcessA() with CREATE_NEW_PROCESS_GROUP flag
    void startProcessBackground(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Same as foreground but:
        // - Use CREATE_NEW_PROCESS_GROUP as dwCreationFlags
        // - Do NOT call WaitForSingleObject
        // - Store pi in childProcesses vector for tracking
    }

    // --- terminate <PID> ---
    // Kill a process by its PID
    // APIs: OpenProcess() + TerminateProcess() + CloseHandle()
    void terminateProcess(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Parse PID from args[0] using std::stoul()
        // 2. OpenProcess(PROCESS_TERMINATE, FALSE, pid)
        // 3. TerminateProcess(handle, 0)
        // 4. CloseHandle(handle)
    }

    // --- list_processes ---
    // List all running processes on the system
    // APIs: CreateToolhelp32Snapshot() + Process32First/Next()
    void listProcesses(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
        // 2. Initialize PROCESSENTRY32, set dwSize
        // 3. Process32First(snapshot, &pe)
        // 4. Loop with Process32Next, print pe.szExeFile and pe.th32ProcessID
        // 5. CloseHandle(snapshot)
    }

    // --- list_children ---
    // List only the child processes of this shell
    // APIs: GetCurrentProcessId() + Toolhelp snapshot
    void listChildProcesses(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Get our PID with GetCurrentProcessId()
        // 2. Take a process snapshot
        // 3. Filter entries where th32ParentProcessID == our PID
    }

    // --- suspend <PID> ---
    // Suspend ALL threads of a process (pauses the process)
    // APIs: CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD) + Thread32First/Next
    //       + OpenThread() + SuspendThread()
    void suspendProcess(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Parse PID from args
        // 2. Snapshot all threads: CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)
        // 3. Iterate with Thread32First/Next
        // 4. For each thread where th32OwnerProcessID == target PID:
        //    a. OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadID)
        //    b. SuspendThread(handle)
        //    c. CloseHandle(handle)
    }

    // --- resume <PID> ---
    // Resume ALL threads of a suspended process
    // Same as suspend but call ResumeThread() instead of SuspendThread()
    void resumeProcess(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Same pattern as suspendProcess but use ResumeThread()
    }

private:
    // Store child processes so we can track/list them later
    std::vector<std::pair<std::string, PROCESS_INFORMATION>> childProcesses;
};

#endif // PROCESS_H
