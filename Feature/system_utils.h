// ============================================================================
// system_utils.h — System Utilities & Batch Execution (Role 4)
// ============================================================================
// Commands to implement:
//   time       — Show current system time
//   date       — Show current date
//   run <file> — Execute a .bat file (read line-by-line, dispatch each command)
//
// Libraries needed:
//   #include <windows.h>   — GetLocalTime
//   #include <iostream>
//   #include <fstream>     — For reading .bat files
//   #include <string>
//   #include <vector>
//   #include <sstream>
//
// Key Windows APIs:
//   GetLocalTime(&st)             — SYSTEMTIME struct with year/month/day/hour/min/sec
//
// For .bat execution:
//   Role 4 needs to agree with Role 1 on the execute_command() function signature.
//   The .bat runner reads the file line-by-line, tokenizes each line, and calls
//   execute_command(command, args) for each non-empty, non-comment line.
//
// Reference: ../Operating-System-Projects/Feature/system_utils.h
// ============================================================================

#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

extern void execute_command(const std::string& command, const std::vector<std::string>& args);

class SystemUtils
{
public:
    void showSystemTime(const std::vector<std::string>& args)
    {
        SYSTEMTIME st;
        GetLocalTime(&st); 
        std::cout << "Current Time: " 
                  << (st.wHour < 10 ? "0" : "") << st.wHour << ":"
                  << (st.wMinute < 10 ? "0" : "") << st.wMinute << ":"
                  << (st.wSecond < 10 ? "0" : "") << st.wSecond << std::endl;
    }

    void showSystemDate(const std::vector<std::string>& args)
    {
        SYSTEMTIME st;
        GetLocalTime(&st); 
        
        std::cout << "Current Date: " 
                  << (st.wDay < 10 ? "0" : "") << st.wDay << "/"
                  << (st.wMonth < 10 ? "0" : "") << st.wMonth << "/"
                  << st.wYear << std::endl;
    }

    // --- run <file.bat> ---
    // Execute a .bat file by reading it line-by-line and dispatching each line
    // through the shell's execute_command() function.
    //
    // NOTE: This function needs a callback/function pointer to execute_command().
    //       Agree with Role 1 on the interface. Example signature:
    //       void runBatFile(const std::string& filename,
    //                       std::function<void(const std::string&, const std::vector<std::string>&)> executor)
    //
    // Steps:
    // 1. Open the .bat file with std::ifstream
    // 2. Read each line with std::getline
    // 3. Skip empty lines and comment lines (starting with "::" or "REM")
    // 4. Tokenize the line into command + args
    // 5. Call executor(command, args)
    void runBatFile(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // 1. Check args is not empty (args[0] should be the filename)
        // 2. Open file, read line-by-line
        // 3. Skip empty/comment lines
        // 4. Tokenize and dispatch each line
        if (args.empty()) {
            std::cerr << "Usage: run <filename.bat>" << std::endl;
            return;
        }
        std::string filename = args[0];
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        std::cout << "Executing batch file: " << filename << "...\n" << std::endl;
        std::string line;
        while(std::getline(file, line)){
            if (line.empty()) continue;

            if (line.rfind("::", 0) == 0 || line.rfind("REM", 0) == 0) {
                continue; 
            }

            std::stringstream ss(line);
            std::string cmd;
            ss >> cmd; 

            if (cmd.empty()) continue;

            std::vector<std::string> batchArgs;
            std::string arg;
            while (ss >> arg) {
                batchArgs.push_back(arg);
            }

            std::cout << "=> Running: " << line << std::endl;

            execute_command(cmd, batchArgs);
            std::cout << "--------------------------------------" << std::endl;

            file.close();
            std::cout << "Batch file execution finished." << std::endl;
        }
    }
};

#endif // SYSTEM_UTILS_H
