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

class EnvironmentManager
{
public:
    // --- set_env <var> = <value> ---
    void setEnv(const std::string& var, const std::string& value)
    {
        // TODO: Implement
        // 1. Store in local map: envVariables[var] = value
        // 2. Also set in OS: _putenv_s(var.c_str(), value.c_str())
    }

    // --- unset_env <var> ---
    void unsetEnv(const std::string& var)
    {
        // TODO: Implement
        // 1. Remove from local map
        // 2. Set to empty in OS: _putenv_s(var.c_str(), "")
    }

    // --- print_env <var> ---
    void printEnv(const std::string& var)
    {
        // TODO: Implement
        // 1. Check local map first
        // 2. If not found, try getenv(var.c_str())
    }

    // --- list_env ---
    void listAllEnv()
    {
        // TODO: Implement
        // Iterate over envVariables map and print each
    }

    // --- getEnv (helper, used by VariableManager) ---
    std::string getEnv(const std::string& var)
    {
        // TODO: Implement
        // 1. Check local map first
        // 2. If not found, try getenv()
        // 3. Return value or empty string
        return "";
    }

private:
    std::map<std::string, std::string> envVariables;
    std::vector<std::string> searchPaths;
};

#endif // ENVIRONMENT_H
