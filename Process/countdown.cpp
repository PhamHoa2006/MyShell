#include <iostream>
#include <windows.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
    int seconds = 10;
    if (argc > 1) {
        seconds = std::atoi(argv[1]);
    }
    
    std::cout << "[Countdown] Starting countdown for " << seconds << " seconds. PID: " << GetCurrentProcessId() << std::endl;
    
    for (int i = seconds; i > 0; --i) {
        std::cout << "[Countdown] " << i << "..." << std::endl;
        Sleep(1000);
    }
    
    std::cout << "[Countdown] Blastoff! Finished." << std::endl;
    return 0;
}
