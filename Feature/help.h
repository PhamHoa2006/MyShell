// ============================================================================
// help.h — Help System (Role 4)
// ============================================================================
// Command to implement:
//   help           — Show all available commands
//   help <command> — Show help for a specific command
//
// NOTE: This file is owned by Role 4 (System & Shell Utilities).
// You need to coordinate with ALL other roles to document their commands.
// This is the user-facing documentation for the entire shell.
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
    // Commands to document (coordinate with each role):
    //
    // Role 1 — Core:
    //   exit                       Quit the shell
    //   history                    Show command history
    //   clear_history              Clear command history
    //
    // Role 2 — File System:
    //   cd <path>                  Change directory
    //   pwd                        Print working directory
    //   dir [path]                 List directory contents
    //   create_file <name>         Create a new file
    //   delete_file <name>         Delete a file
    //   read_file <name>           Read a file's contents
    //   copy_file <src> <dst>      Copy a file
    //   write_file <text> <name>   Write text to a file
    //   mkdir <name>               Create a directory
    //   rmdir <name>               Remove a directory
    //   list_tree [path]           Show directory tree
    //
    // Role 3 — Process:
    //   start_foreground <exe>     Start a process and wait
    //   start_background <exe>     Start a process in background
    //   list                       List background processes (PID, name, status)
    //   list_processes             List all system processes
    //   terminate <PID>            Kill a process
    //   suspend <PID>              Pause a process
    //   resume <PID>               Resume a paused process
    //
    // Role 4 — Utilities:
    //   time                       Show current time
    //   date                       Show current date
    //   path                       Show PATH variable
    //   addpath <dir>              Add directory to PATH
    //   set_env <var> <value>      Set environment variable
    //   unset_env <var>            Remove environment variable
    //   print_env <var>            Print an environment variable
    //   run <file.bat>             Execute a batch file
    //   help                       Show this help
    if (args.empty()) {
        std::cout << "=================================================================" << std::endl;
        std::cout << "                   SMALL SHELL - AVAILABLE COMMANDS              " << std::endl;
        std::cout << "=================================================================" << std::endl;
        
        // Cài đặt độ rộng cột lệnh là 25 ký tự, cột mô tả căn lề trái
        std::cout << std::left << std::setw(25) << "COMMAND" << "DESCRIPTION" << std::endl;
        std::cout << "-----------------------------------------------------------------" << std::endl;

        // Role 1 - Core Commands
        std::cout << std::setw(25) << "exit" << "Quit the shell" << std::endl;
        std::cout << std::setw(25) << "history" << "Show command history" << std::endl;
        std::cout << std::setw(25) << "clear_history" << "Clear command history" << std::endl;

        // Role 2 - File System Commands
        std::cout << std::setw(25) << "cd <path>" << "Change directory" << std::endl;
        std::cout << std::setw(25) << "pwd" << "Print current working directory" << std::endl;
        std::cout << std::setw(25) << "dir [path]" << "List directory contents" << std::endl;
        std::cout << std::setw(25) << "create_file <name>" << "Create a new file" << std::endl;
        std::cout << std::setw(25) << "delete_file <name>" << "Delete a file" << std::endl;
        std::cout << std::setw(25) << "read_file <name>" << "Read a file's contents" << std::endl;
        std::cout << std::setw(25) << "copy_file <src> <dst>" << "Copy a file" << std::endl;
        std::cout << std::setw(25) << "write_file <text> <name>" << "Write text to a file" << std::endl;
        std::cout << std::setw(25) << "mkdir <name>" << "Create a directory" << std::endl;
        std::cout << std::setw(25) << "rmdir <name>" << "Remove a directory" << std::endl;
        std::cout << std::setw(25) << "list_tree [path]" << "Show directory tree" << std::endl;

        // Role 3 - Process Commands
        std::cout << std::setw(25) << "start_foreground <exe>" << "Start a process and wait for it" << std::endl;
        std::cout << std::setw(25) << "start_background <exe>" << "Start a process in the background" << std::endl;
        std::cout << std::setw(25) << "list" << "List background processes (PID, name, status)" << std::endl;
        std::cout << std::setw(25) << "list_processes" << "List all system processes" << std::endl;
        std::cout << std::setw(25) << "terminate <PID>" << "Kill a process" << std::endl;
        std::cout << std::setw(25) << "suspend <PID>" << "Pause a process" << std::endl;
        std::cout << std::setw(25) << "resume <PID>" << "Resume a paused process" << std::endl;

        // Role 4 - Utilities
        std::cout << std::setw(25) << "time" << "Show current system time" << std::endl;
        std::cout << std::setw(25) << "date" << "Show current system date" << std::endl;
        std::cout << std::setw(25) << "path" << "Show PATH variable" << std::endl;
        std::cout << std::setw(25) << "addpath <dir>" << "Add directory to PATH" << std::endl;
        std::cout << std::setw(25) << "set_env <var> <value>" << "Set environment variable" << std::endl;
        std::cout << std::setw(25) << "unset_env <var>" << "Remove environment variable" << std::endl;
        std::cout << std::setw(25) << "print_env <var>" << "Print an environment variable" << std::endl;
        std::cout << std::setw(25) << "run <file.bat>" << "Execute a batch file" << std::endl;
        std::cout << std::setw(25) << "help" << "Show this help" << std::endl;
        
        std::cout << "==============================================================================\n";
        std::cout << "Tip: Type 'help <command>' to view detailed manual for a specific command!\n";
        return;
    }

    std::string cmd = args[0];
    std::cout << "\n------------------------------------------------------------------------------\n";
    std::cout << " MANUAL PAGE: " << cmd << "\n";
    std::cout << "------------------------------------------------------------------------------\n";

    // --- ROLE 1 ---
    if (cmd == "exit") {
        std::cout << "Usage      : exit\n";
        std::cout << "Description: Safely close the Small Shell prompt and return to Windows.\n";
    } else if (cmd == "history") {
        std::cout << "Usage      : history\n";
        std::cout << "Description: Print a numbered list of all previously executed commands.\n";
    } else if (cmd == "clear_history") {
        std::cout << "Usage      : clear_history\n";
        std::cout << "Description: Delete all recorded command history from the history.txt file.\n";
    }
    // --- ROLE 2 ---
    else if (cmd == "cd") {
        std::cout << "Usage      : cd <path>\n";
        std::cout << "Description: Navigate the shell to a different directory path.\n";
        std::cout << "Example    : cd C:/Users/Admin/Documents\n";
    } else if (cmd == "pwd") {
        std::cout << "Usage      : pwd\n";
        std::cout << "Description: Display the absolute path of the current working directory.\n";
    } else if (cmd == "dir") {
        std::cout << "Usage      : dir [path]\n";
        std::cout << "Description: List all files and subdirectories inside [path].\n";
        std::cout << "             If [path] is omitted, lists current directory contents.\n";
    } else if (cmd == "create_file") {
        std::cout << "Usage      : create_file <name>\n";
        std::cout << "Description: Create a brand new, empty regular file.\n";
    } else if (cmd == "delete_file") {
        std::cout << "Usage      : delete_file <name>\n";
        std::cout << "Description: Permanently delete a specified target file.\n";
    } else if (cmd == "read_file") {
        std::cout << "Usage      : read_file <name>\n";
        std::cout << "Description: Open the target file and print all its raw text to console.\n";
    } else if (cmd == "copy_file") {
        std::cout << "Usage      : copy_file <src> <dst>\n";
        std::cout << "Description: Duplicate a file from Source path to Destination path.\n";
    } else if (cmd == "write_file") {
        std::cout << "Usage      : write_file <text> <name>\n";
        std::cout << "Description: Insert or append a string of text into the target file.\n";
    } else if (cmd == "mkdir") {
        std::cout << "Usage      : mkdir <name>\n";
        std::cout << "Description: Create a new empty folder/directory.\n";
    } else if (cmd == "rmdir") {
        std::cout << "Usage      : rmdir <name>\n";
        std::cout << "Description: Remove an existing directory from the file system.\n";
    } else if (cmd == "list_tree") {
        std::cout << "Usage      : list_tree [path]\n";
        std::cout << "Description: Render the directory structure visually as an ASCII tree.\n";
    }
    // --- ROLE 3 ---
    else if (cmd == "start_foreground") {
        std::cout << "Usage      : start_foreground <executable_path>\n";
        std::cout << "Description: Launch a child program and FREEZE the shell until it finishes.\n";
    } else if (cmd == "start_background") {
        std::cout << "Usage      : start_background <executable_path>\n";
        std::cout << "Description: Launch a child program in the background. Shell stays active.\n";
    } else if (cmd == "list") {
        std::cout << "Usage      : list\n";
        std::cout << "Description: Display internal tracking table of background jobs launched by MyShell.\n";
        std::cout << "             Outputs: [Internal PID] | [Command Name] | [Status]\n";
    } else if (cmd == "list_processes") {
        std::cout << "Usage      : list_processes\n";
        std::cout << "Description: Snapshot and list ALL running processes currently live in Windows OS.\n";
    } else if (cmd == "terminate") {
        std::cout << "Usage      : terminate <PID>\n";
        std::cout << "Description: Forcefully send a kill signal to terminate process matching <PID>.\n";
    } else if (cmd == "suspend") {
        std::cout << "Usage      : suspend <PID>\n";
        std::cout << "Description: Freeze all executing threads of a target process.\n";
    } else if (cmd == "resume") {
        std::cout << "Usage      : resume <PID>\n";
        std::cout << "Description: Unfreeze all suspended threads of a target process.\n";
    }
    // --- ROLE 4 ---
    else if (cmd == "time") {
        std::cout << "Usage      : time\n";
        std::cout << "Description: Query Windows OS and output current local time (HH:MM:SS).\n";
    } else if (cmd == "date") {
        std::cout << "Usage      : date\n";
        std::cout << "Description: Query Windows OS and output current local date (DD/MM/YYYY).\n";
    } else if (cmd == "path") {
        std::cout << "Usage      : path\n";
        std::cout << "Description: Print out all stored paths registered in the OS PATH variable.\n";
    } else if (cmd == "addpath") {
        std::cout << "Usage      : addpath <directory_path>\n";
        std::cout << "Description: Dynamically append a new folder path into the system PATH.\n";
    } else if (cmd == "set_env") {
        std::cout << "Usage      : set_env <variable_name> <value>\n";
        std::cout << "Description: Assign an internal environment variable & sync it to Windows OS.\n";
    } else if (cmd == "unset_env") {
        std::cout << "Usage      : unset_env <variable_name>\n";
        std::cout << "Description: Erase an environment variable from the shell memory and OS scope.\n";
    } else if (cmd == "print_env") {
        std::cout << "Usage      : print_env <variable_name>\n";
        std::cout << "Description: Lookup and print the stored value of a specific environment variable.\n";
    } else if (cmd == "run") {
        std::cout << "Usage      : run <script_file.bat>\n";
        std::cout << "Description: Read a batch script line-by-line and dispatch them to the shell engine.\n";
    } else if (cmd == "help") {
        std::cout << "Usage      : help [command_name]\n";
        std::cout << "Description: The manual system you are looking at right now.\n";
    } else {
        std::cout << "Error: No manual entry found for command '" << cmd << "'.\n";
        std::cout << "Type 'help' without arguments to see the valid list of 30 commands.\n";
    }
    std::cout << "------------------------------------------------------------------------------\n\n";


}

#endif // HELP_H
