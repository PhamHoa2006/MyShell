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
}

#endif // HELP_H
