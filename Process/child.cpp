// ============================================================================
// child.cpp — Minimal Child Process
// ============================================================================
// This is a separate program that MyShell can launch as a child process.
// It does nothing useful — just sleeps for 10 seconds then exits.
// Purpose: Test that CreateProcess() and WaitForSingleObject() work.
//
// Compile separately:
//   g++ -o child.exe child.cpp
//
// Usage from MyShell:
//   start_foreground child.exe    → shell waits 10 seconds
//   start_background child.exe   → shell returns immediately
// ============================================================================

// TODO: Implement this
#include <windows.h>
#include <iostream>

int main() {
    std::cout << "Child process started! PID: " << GetCurrentProcessId() << std::endl;
    Sleep(10000);  // Sleep 10 seconds
    std::cout << "Child process finished!" << std::endl;
    return 0;
}
