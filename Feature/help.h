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
    if (args.empty()) {
        std::cout << "=================================================================\n";
        std::cout << "                      Small Shell Help Menu                      \n";
        std::cout << "=================================================================\n";
        
        std::cout << "\nRole 1 — Core Architect Commands:\n";
        std::cout << "  " << std::left << std::setw(25) << "exit" << "Quit the shell\n";
        std::cout << "  " << std::left << std::setw(25) << "history" << "Show command history\n";
        std::cout << "  " << std::left << std::setw(25) << "clear_history" << "Clear command history\n";
        std::cout << "  " << std::left << std::setw(25) << "help" << "Show this help menu\n";

#ifndef TEST_ROLE1
        std::cout << "\nRole 2 — File System Commands:\n";
        std::cout << "  " << std::left << std::setw(25) << "cd <path>" << "Change directory\n";
        std::cout << "  " << std::left << std::setw(25) << "pwd" << "Print working directory\n";
        std::cout << "  " << std::left << std::setw(25) << "dir [path]" << "List directory contents\n";
        std::cout << "  " << std::left << std::setw(25) << "create_file <name>" << "Create a new file\n";
        std::cout << "  " << std::left << std::setw(25) << "delete_file <name>" << "Delete a file\n";
        std::cout << "  " << std::left << std::setw(25) << "read_file <name>" << "Read a file's contents\n";
        std::cout << "  " << std::left << std::setw(25) << "copy_file <src> <dst>" << "Copy a file\n";
        std::cout << "  " << std::left << std::setw(25) << "write_file <text> <name>" << "Write text to a file\n";
        std::cout << "  " << std::left << std::setw(25) << "mkdir <name>" << "Create a directory\n";
        std::cout << "  " << std::left << std::setw(25) << "rmdir <name>" << "Remove a directory\n";
        std::cout << "  " << std::left << std::setw(25) << "list_tree [path]" << "Show directory tree\n";

        std::cout << "\nRole 3 — Process Commands:\n";
        std::cout << "  " << std::left << std::setw(25) << "start_foreground <exe>" << "Start a process and wait\n";
        std::cout << "  " << std::left << std::setw(25) << "start_background <exe>" << "Start a process in background\n";
        std::cout << "  " << std::left << std::setw(25) << "list" << "List background processes (PID, name, status)\n";
        std::cout << "  " << std::left << std::setw(25) << "list_processes" << "List all system processes\n";
        std::cout << "  " << std::left << std::setw(25) << "terminate <PID>" << "Kill a process\n";
        std::cout << "  " << std::left << std::setw(25) << "suspend <PID>" << "Pause a process\n";
        std::cout << "  " << std::left << std::setw(25) << "resume <PID>" << "Resume a paused process\n";

        std::cout << "\nRole 4 — Utilities Commands:\n";
        std::cout << "  " << std::left << std::setw(25) << "time" << "Show current time\n";
        std::cout << "  " << std::left << std::setw(25) << "date" << "Show current date\n";
        std::cout << "  " << std::left << std::setw(25) << "path" << "Show PATH variable\n";
        std::cout << "  " << std::left << std::setw(25) << "addpath <dir>" << "Add directory to PATH\n";
        std::cout << "  " << std::left << std::setw(25) << "set_env <var> <val>" << "Set environment variable\n";
        std::cout << "  " << std::left << std::setw(25) << "unset_env <var>" << "Remove environment variable\n";
        std::cout << "  " << std::left << std::setw(25) << "print_env <var>" << "Print an environment variable\n";
        std::cout << "  " << std::left << std::setw(25) << "run <file.bat>" << "Execute a batch file\n";
#endif
        std::cout << "=================================================================\n";
        std::cout << "Use 'help <command>' to get detailed help for a specific command.\n";
    } else {
        std::string cmd = args[0];
        if (cmd == "exit") {
            std::cout << "exit: Terminate the Small Shell program.\nUsage: exit\n";
        } else if (cmd == "history") {
            std::cout << "history: Display a list of all commands entered in the current and past sessions.\nUsage: history\n";
        } else if (cmd == "clear_history") {
            std::cout << "clear_history: Delete all command records from the persistent history log.\nUsage: clear_history\n";
        } else if (cmd == "help") {
            std::cout << "help: Show list of available commands or detailed info about a specific command.\nUsage: help [command]\n";
        } 
#ifndef TEST_ROLE1
        else if (cmd == "cd") {
            std::cout << "cd: Change the current working directory.\nUsage: cd <path>\n";
        } else if (cmd == "pwd") {
            std::cout << "pwd: Print the absolute path of the current working directory.\nUsage: pwd\n";
        } else if (cmd == "dir") {
            std::cout << "dir: List the files and directories in the specified path, or the current path if none specified.\nUsage: dir [path]\n";
        } else if (cmd == "create_file") {
            std::cout << "create_file: Create an empty file.\nUsage: create_file <name>\n";
        } else if (cmd == "delete_file") {
            std::cout << "delete_file: Delete an existing file.\nUsage: delete_file <name>\n";
        } else if (cmd == "read_file") {
            std::cout << "read_file: Display the contents of a file on the console.\nUsage: read_file <name>\n";
        } else if (cmd == "copy_file") {
            std::cout << "copy_file: Copy a file from source to destination.\nUsage: copy_file <src> <dst>\n";
        } else if (cmd == "write_file") {
            std::cout << "write_file: Write the specified text to a file. Overwrites any existing contents.\nUsage: write_file <text> <name>\n";
        } else if (cmd == "mkdir") {
            std::cout << "mkdir: Create a new directory.\nUsage: mkdir <name>\n";
        } else if (cmd == "rmdir") {
            std::cout << "rmdir: Remove an empty directory.\nUsage: rmdir <name>\n";
        } else if (cmd == "list_tree") {
            std::cout << "list_tree: Display the directory structure in a tree-like format starting from a path.\nUsage: list_tree [path]\n";
        } else if (cmd == "start_foreground") {
            std::cout << "start_foreground: Start a program in the foreground and wait for it to complete.\nUsage: start_foreground <exe>\n";
        } else if (cmd == "start_background") {
            std::cout << "start_background: Start a program in the background and continue executing shell commands.\nUsage: start_background <exe>\n";
        } else if (cmd == "list") {
            std::cout << "list: List all background processes spawned by the shell with their PID, name, and status.\nUsage: list\n";
        } else if (cmd == "list_processes") {
            std::cout << "list_processes: List all active processes currently running on the system.\nUsage: list_processes\n";
        } else if (cmd == "terminate") {
            std::cout << "terminate: Terminate a running process by its PID.\nUsage: terminate <PID>\n";
        } else if (cmd == "suspend") {
            std::cout << "suspend: Pause the execution of a process by its PID.\nUsage: suspend <PID>\n";
        } else if (cmd == "resume") {
            std::cout << "resume: Resume the execution of a paused process by its PID.\nUsage: resume <PID>\n";
        } else if (cmd == "time") {
            std::cout << "time: Show the current system time.\nUsage: time\n";
        } else if (cmd == "date") {
            std::cout << "date: Show the current system date.\nUsage: date\n";
        } else if (cmd == "path") {
            std::cout << "path: Display the current PATH environment variable directories.\nUsage: path\n";
        } else if (cmd == "addpath") {
            std::cout << "addpath: Append a new directory path to the PATH environment variable.\nUsage: addpath <dir>\n";
        } else if (cmd == "set_env") {
            std::cout << "set_env: Define or update an environment variable with a key and value.\nUsage: set_env <var> <value>\n";
        } else if (cmd == "unset_env") {
            std::cout << "unset_env: Delete an existing environment variable.\nUsage: unset_env <var>\n";
        } else if (cmd == "print_env") {
            std::cout << "print_env: Display the value of a specific environment variable.\nUsage: print_env <var>\n";
        } else if (cmd == "run") {
            std::cout << "run: Parse and execute commands from a batch (.bat) file line by line.\nUsage: run <file.bat>\n";
        }
#endif
        else {
            std::cout << "Help: Unknown command '" << cmd << "'.\n";
        }
    }
}

#endif // HELP_H
