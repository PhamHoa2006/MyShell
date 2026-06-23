// ============================================================================
// environment.h — Environment Variable Management
// ============================================================================
// Commands to implement:
//   set_env <var> = <value>   — Set an environment variable
//   unset_env <var>           — Remove an environment variable
//   print_env <var>           — Print a variable's value
//   list_env                  — List all environment variables
//   add_path <path>           — Add a directory to the PATH
//   remove_path <path>        — Remove a directory from PATH
//
// Libraries needed:
//   #include <cstdlib>    — getenv(), _putenv_s() (Windows)
//   #include <map>        — for storing shell-local variables
//   #include <string>
//   #include <vector>
//   #include <iostream>
//   #include <fstream>    — for save_env / load_env
//
// Key C functions:
//   getenv("VAR")              — Get an environment variable value (returns char*)
//   _putenv_s("VAR", "value")  — Set an environment variable (Windows-specific)
//
// Reference: ../Operating-System-Projects/Feature/environment.h
// ============================================================================

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>

class EnvironmentManager
{
public:
    // --- set_env <var> = <value> ---
    void setEnv(const std::string& var, const std::string& value)
    {
        // 1. Store in local map: envVariables[var] = value
        // 2. Also set in OS: _putenv_s(var.c_str(), value.c_str())

        if (var.empty()) {
            std::cerr << "Error: Variable name cannot be empty.\n";
            return;
        }

        envVariables[var] = value;

        if (_putenv_s(var.c_str(), value.c_str()) == 0) {
            std::cout << "Successfully set: " << var << " = " << value << "\n";
        } else {
            std::cerr << "Failed to update Windows OS environment for '" << var << "'.\n";
        }
        
    }

    // --- unset_env <var> ---
    void unsetEnv(const std::string& var)
    {
        // 1. Remove from local map
        // 2. Set to empty in OS: _putenv_s(var.c_str(), "")
        if (var.empty()) {
            std::cerr << "Usage: unset_env <variable_name>\n";
            return;
        }

        envVariables.erase(var);

        if (_putenv_s(var.c_str(), "") == 0) {
            std::cout << "Successfully unset variable '" << var << "'.\n";
        } else {
            std::cerr << "Failed to remove '" << var << "' from OS environment.\n";
        }
        
    }

    // --- print_env <var> ---
    void printEnv(const std::string& var)
    {
        // 1. Check local map first
        // 2. If not found, try getenv(var.c_str())
        if (var.empty()) {
            std::cerr << "Usage: print_env <variable_name>\n";
            return;
        }

        std::string val = getEnv(var);

        if (!val.empty()) {
            std::cout << var << " = " << val << "\n";
        } else {
            std::cout << "Environment variable '" << var << "' is not set.\n";
        }
    }

    // --- list_env ---
    void listAllEnv()
    {
        // Iterate over envVariables map and print each

        std::cout << "--- Local Shell Environment Variables ---\n";
        if (envVariables.empty()) {
            std::cout << "(No shell-local variables set yet)\n";
        } else {
            for (const auto& pair : envVariables) {
                std::cout << pair.first << " = " << pair.second << "\n";
            }
        }
        std::cout << "-------------------------------------------\n";
    }

    // --- getEnv (helper, used by VariableManager) ---
    std::string getEnv(const std::string& var)
    {
        // 1. Check local map first
        // 2. If not found, try getenv()
        // 3. Return value or empty string

        auto it = envVariables.find(var);
        if (it != envVariables.end()) {
            return it->second;
        }

        char* val = std::getenv(var.c_str());
        if (val != nullptr) {
            return std::string(val);
        }

        return "";
    }

    void printPath()
    {
        std::string sysPath = getEnv("PATH");
        std::cout << "SYSTEM PATH:\n" << sysPath << "\n";
    }

    void addPath(const std::string& newPath)
    {
        if (newPath.empty()) {
            std::cerr << "Usage: add_path <directory_path>\n";
            return;
        }

        // Lưu vào mảng private searchPaths của class
        if (std::find(searchPaths.begin(), searchPaths.end(), newPath) == searchPaths.end()) {
            searchPaths.push_back(newPath);
        }

        // Nối vào chuỗi PATH của hệ điều hành Windows
        std::string currentPath = getEnv("PATH");
        std::string updatedPath = currentPath;
        if (!updatedPath.empty() && updatedPath.back() != ';') {
            updatedPath += ";";
        }
        updatedPath += newPath;

        if (_putenv_s("PATH", updatedPath.c_str()) == 0) {
            std::cout << "Successfully appended '" << newPath << "' to PATH.\n";
        } else {
            std::cerr << "Failed to update OS PATH variable.\n";
        }
    }

    void removePath(const std::string& targetPath)
    {
        if (targetPath.empty()) {
            std::cerr << "Usage: remove_path <directory_path>\n";
            return;
        }

        // Xóa khỏi vector searchPaths
        auto it = std::find(searchPaths.begin(), searchPaths.end(), targetPath);
        if (it != searchPaths.end()) {
            searchPaths.erase(it);
        }

        // Cắt bỏ chuỗi targetPath ra khỏi chuỗi PATH khổng lồ của Windows
        std::string sysPath = getEnv("PATH");
        size_t pos = sysPath.find(targetPath);
        if (pos != std::string::npos) {
            size_t len = targetPath.length();
            // Xóa luôn dấu chấm phẩy đi kèm (ở sau hoặc ở trước nó)
            if (pos + len < sysPath.length() && sysPath[pos + len] == ';') {
                len += 1;
            } else if (pos > 0 && sysPath[pos - 1] == ';') {
                pos -= 1;
                len += 1;
            }
            sysPath.erase(pos, len);
            _putenv_s("PATH", sysPath.c_str());
            std::cout << "Removed '" << targetPath << "' from system PATH.\n";
        } else {
            std::cout << "Directory '" << targetPath << "' is not in the PATH.\n";
        }
    }


private:
    std::map<std::string, std::string> envVariables;
    std::vector<std::string> searchPaths;
};

#endif // ENVIRONMENT_H
