// ============================================================================
// countdown.cpp — Countdown Timer (Child Process)
// ============================================================================
// A separate program that counts down from N to 0.
// Demonstrates: child process + synchronization concepts
//
// Compile separately:
//   g++ -o countdown.exe countdown.cpp
//
// Usage from MyShell:
//   start_background countdown.exe
// ============================================================================

// TODO: Implement this
#include <windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
    int count = 10;  // Default: count down from 10
    if (argc > 1) count = std::stoi(argv[1]);

    for (int i = count; i >= 0; --i) {
        std::cout << "Countdown: " << i << std::endl;
        Sleep(1000);  // Wait 1 second
    }
    std::cout << "Done!" << std::endl;
    return 0;
}
