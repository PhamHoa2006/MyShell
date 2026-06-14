// ============================================================================
// help.h — Help System
// ============================================================================
// Command to implement:
//   help           — Show all available commands
//   help <command> — Show help for a specific command
//
// Reference: ../Operating-System-Projects/Feature/help.h
// ============================================================================

#ifndef HELP_H
#define HELP_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

void showHelp(const std::vector<std::string>& args)
{
    // TODO: Implement
    // If args is empty → print a table of ALL commands with descriptions
    // If args has a command name → print detailed help for that command
    //
    // Example output:
    //   ===== MyShell Help =====
    //   cd <path>              Change directory
    //   pwd                    Print working directory
    //   dir [path]             List directory contents
    //   ...
}

#endif // HELP_H
