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

// Status enum for tracking the shell's own background processes
enum class ProcessStatus
{
    RUNNING,
    SUSPENDED,
    TERMINATED
};

// Info struct for tracking background processes launched by this shell
struct BackgroundProcessInfo
{
    DWORD pid;
    std::string cmdName;
    HANDLE hProcess;
    HANDLE hThread;
    ProcessStatus status;
};

// Global handle to the current foreground child process (used by CTRL+C handler)
// Set before WaitForSingleObject, reset to NULL after foreground process finishes.
inline HANDLE g_hForegroundProcess = NULL;

// --- CTRL+C Handler ---
// Register this with SetConsoleCtrlHandler() at shell startup.
// When Ctrl+C is pressed:
//   - If a foreground process is running → terminate it, return TRUE (shell survives)
//   - If no foreground process → return TRUE (ignore, shell survives)
inline BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        if (g_hForegroundProcess != NULL)
        {
            // TODO: TerminateProcess(g_hForegroundProcess, 1);
            // TODO: Print message like "[Ctrl+C] Foreground process terminated."
        }
        return TRUE; // Shell does NOT exit
    }
    return FALSE;
}

class ProcessManager
{
public:
    // --- start_foreground <exe> ---
    // Create a new process and WAIT for it to finish before returning to shell
    // APIs: CreateProcessA() + WaitForSingleObject(pi.hProcess, INFINITE)
    // IMPORTANT: Set g_hForegroundProcess = pi.hProcess BEFORE calling
    //            WaitForSingleObject, and reset it to NULL after.
    void startProcessForeground(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check args is not empty
        // 2. Build command string from args
        // 3. Initialize STARTUPINFOA and PROCESS_INFORMATION
        // 4. Call CreateProcessA(...)
        // 5. If success:
        //    a. g_hForegroundProcess = pi.hProcess;   // Enable CTRL+C kill
        //    b. WaitForSingleObject(pi.hProcess, INFINITE);
        //    c. g_hForegroundProcess = NULL;           // Disable CTRL+C kill
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
        // - Store process info in backgroundProcesses vector for tracking
        // - Set status = ProcessStatus::RUNNING
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
        // 5. Update status in backgroundProcesses to TERMINATED
    }

    // --- list_processes ---
    // List all running processes on the system (bonus, not required by course)
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

    // --- list ---
    // List the shell's own background processes with status
    // This is the REQUIRED command from the course requirements.
    // Output format: PID | Cmd Name | Status (Running/Suspended/Terminated)
    void listBackgroundProcesses(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Print table header: PID | Command | Status
        // 2. Loop through backgroundProcesses vector
        // 3. For each process, check if it's still alive (WaitForSingleObject with 0 timeout)
        //    - If WAIT_OBJECT_0: process has exited → update status to TERMINATED
        // 4. Print each entry with its current status
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
        // 5. Update status in backgroundProcesses to SUSPENDED
    }

    // --- resume <PID> ---
    // Resume ALL threads of a suspended process
    // Same as suspend but call ResumeThread() instead of SuspendThread()
    void resumeProcess(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Same pattern as suspendProcess but use ResumeThread()
        // Update status in backgroundProcesses to RUNNING
    }

private:
    // Track background processes launched by this shell (PID, name, handle, status)
    std::vector<BackgroundProcessInfo> backgroundProcesses;
};

#endif // PROCESS_H
