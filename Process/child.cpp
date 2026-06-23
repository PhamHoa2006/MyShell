#include <iostream>
#include <windows.h>

int main() {
    std::cout << "[Child] Process started! PID: " << GetCurrentProcessId() << std::endl;
    std::cout << "[Child] Simulating work for 10 seconds..." << std::endl;
    Sleep(10000);
    std::cout << "[Child] Process finished successfully!" << std::endl;
    return 0;
}
