#ifndef HELP_H
#define HELP_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

void showHelp(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cout
        << "================================================================="
        << std::endl;
    std::cout
        << "                   MY SHELL - AVAILABLE COMMANDS              "
        << std::endl;
    std::cout
        << "================================================================="
        << std::endl;

    std::cout << std::left << std::setw(25) << "COMMAND" << "DESCRIPTION"
              << std::endl;
    std::cout
        << "-----------------------------------------------------------------"
        << std::endl;

    // Role 1 - Core Commands
    std::cout << std::setw(25) << "help" << "Show this help menu" << std::endl;
    std::cout << std::setw(25) << "history" << "Show command history"
              << std::endl;
    std::cout << std::setw(25) << "clear_history" << "Clear command history"
              << std::endl;
    std::cout << std::setw(25) << "exit / quit" << "Quit the shell"
              << std::endl;

    // Role 2 - Navigation & File System
    std::cout << std::setw(25) << "cd <path>" << "Change directory"
              << std::endl;
    std::cout << std::setw(25) << "pwd" << "Print current working directory"
              << std::endl;
    std::cout << std::setw(25) << "dir / ls" << "List directory contents"
              << std::endl;
    std::cout << std::setw(25) << "mkfile <name>" << "Create a new file"
              << std::endl;
    std::cout << std::setw(25) << "del / rm <name>" << "Delete a file"
              << std::endl;
    std::cout << std::setw(25) << "cat / type <name>"
              << "Read a file's contents" << std::endl;
    std::cout << std::setw(25) << "write / echo" << "Write text to a file"
              << std::endl;
    std::cout << std::setw(25) << "move / mv" << "Move a file" << std::endl;
    std::cout << std::setw(25) << "rename / ren" << "Rename a file"
              << std::endl;
    std::cout << std::setw(25) << "size <name>" << "Get file size" << std::endl;
    std::cout << std::setw(25) << "start <name>" << "Open file with default app"
              << std::endl;

    std::cout << std::setw(25) << "mkdir / md <name>" << "Create a directory"
              << std::endl;
    std::cout << std::setw(25) << "rmdir / rd <name>" << "Remove a directory"
              << std::endl;
    std::cout << std::setw(25) << "cpdir / xcopy"
              << "Copy a directory recursively" << std::endl;
    std::cout << std::setw(25) << "mvdir" << "Move a directory" << std::endl;
    std::cout << std::setw(25) << "tree" << "Show directory tree" << std::endl;

    // Role 3 - Process Commands
    std::cout << std::setw(25) << "start_foreground"
              << "Start a process and wait" << std::endl;
    std::cout << std::setw(25) << "start_background"
              << "Start a process in the background" << std::endl;
    std::cout << std::setw(25) << "list_background"
              << "List MyShell background processes" << std::endl;
    std::cout << std::setw(25) << "list_all_processes"
              << "List all system OS processes" << std::endl;
    std::cout << std::setw(25) << "terminate <PID>" << "Kill a process"
              << std::endl;
    std::cout << std::setw(25) << "suspend <PID>" << "Pause a process"
              << std::endl;
    std::cout << std::setw(25) << "resume <PID>" << "Resume a paused process"
              << std::endl;

    // Role 4 - Utilities
    std::cout << std::setw(25) << "time" << "Show current system time"
              << std::endl;
    std::cout << std::setw(25) << "date" << "Show current system date"
              << std::endl;
    std::cout << std::setw(25) << "path" << "Show PATH variable" << std::endl;
    std::cout << std::setw(25) << "addpath <dir>" << "Add directory to PATH"
              << std::endl;
    std::cout << std::setw(25) << "set_env" << "Set environment variable"
              << std::endl;
    std::cout << std::setw(25) << "unset_env <var>"
              << "Remove environment variable" << std::endl;
    std::cout << std::setw(25) << "print_env <var>"
              << "Print an environment variable" << std::endl;
    std::cout << std::setw(25) << "list_env"
              << "Print all environment variables" << std::endl;
    std::cout << std::setw(25) << "run <file.bat>" << "Execute a batch script"
              << std::endl;

    std::cout << "============================================================="
                 "=================\n";
    std::cout << "Tip: Type 'help <command>' to view detailed manual for a "
                 "specific command!\n";
    return;
  }

  std::string cmd = args[0];
  std::cout << "\n-------------------------------------------------------------"
               "-----------------\n";
  std::cout << " MANUAL PAGE: " << cmd << "\n";
  std::cout << "---------------------------------------------------------------"
               "---------------\n";

  // --- ROLE 1 ---
  if (cmd == "exit" || cmd == "quit") {
    std::cout << "Usage      : exit\n";
    std::cout << "Description: Safely close the shell and terminate all "
                 "background child processes.\n";
  } else if (cmd == "history") {
    std::cout << "Usage      : history\n";
    std::cout << "Description: Print a numbered list of all previously "
                 "executed commands.\n";
  } else if (cmd == "clear_history") {
    std::cout << "Usage      : clear_history\n";
    std::cout << "Description: Delete all recorded command history.\n";
  }
  // --- ROLE 2 ---
  else if (cmd == "cd") {
    std::cout << "Usage      : cd <path>\n";
    std::cout
        << "Description: Navigate the shell to a different directory path.\n";
  } else if (cmd == "pwd") {
    std::cout << "Usage      : pwd\n";
    std::cout << "Description: Display the absolute path of the current "
                 "working directory.\n";
  } else if (cmd == "dir" || cmd == "ls") {
    std::cout << "Usage      : dir [path]\n";
    std::cout
        << "Description: List all files and subdirectories inside [path].\n";
  } else if (cmd == "mkfile") {
    std::cout << "Usage      : mkfile <name>\n";
    std::cout << "Description: Create a brand new, empty regular file.\n";
  } else if (cmd == "del" || cmd == "rm") {
    std::cout << "Usage      : del <name>\n";
    std::cout << "Description: Permanently delete a specified target file.\n";
  } else if (cmd == "cat" || cmd == "type") {
    std::cout << "Usage      : cat <name>\n";
    std::cout << "Description: Open the target file and print all its raw text "
                 "to console.\n";
  } else if (cmd == "write" || cmd == "echo") {
    std::cout << "Usage      : write <text> <name>\n";
    std::cout << "Description: Insert or append a string of text into the "
                 "target file.\n";
  } else if (cmd == "move" || cmd == "mv") {
    std::cout << "Usage      : move <src> <dst>\n";
    std::cout << "Description: Move a file to a new location.\n";
  } else if (cmd == "rename" || cmd == "ren") {
    std::cout << "Usage      : rename <old_name> <new_name>\n";
    std::cout << "Description: Rename an existing file.\n";
  } else if (cmd == "start") {
    std::cout << "Usage      : start <name>\n";
    std::cout << "Description: Open the file using the default Windows "
                 "application.\n";
  } else if (cmd == "size") {
    std::cout << "Usage      : size <name>\n";
    std::cout << "Description: Get the size of the file in bytes.\n";
  } else if (cmd == "mkdir" || cmd == "md") {
    std::cout << "Usage      : mkdir <name>\n";
    std::cout << "Description: Create a new empty folder/directory.\n";
  } else if (cmd == "rmdir" || cmd == "rd") {
    std::cout << "Usage      : rmdir <name>\n";
    std::cout
        << "Description: Remove an existing directory from the file system.\n";
  } else if (cmd == "cpdir" || cmd == "xcopy") {
    std::cout << "Usage      : cpdir <src> <dst>\n";
    std::cout
        << "Description: Copy a directory and all its contents recursively.\n";
  } else if (cmd == "mvdir") {
    std::cout << "Usage      : mvdir <src> <dst>\n";
    std::cout << "Description: Move a directory to a new location.\n";
  } else if (cmd == "tree") {
    std::cout << "Usage      : tree [path]\n";
    std::cout << "Description: Render the directory structure visually as an "
                 "ASCII tree.\n";
  }
  // --- ROLE 3 ---
  else if (cmd == "start_foreground") {
    std::cout << "Usage      : start_foreground <executable_path>\n";
    std::cout << "Description: Launch a child program and FREEZE the shell "
                 "until it finishes.\n";
  } else if (cmd == "start_background") {
    std::cout << "Usage      : start_background <executable_path>\n";
    std::cout << "Description: Launch a child program in the background. Shell "
                 "stays active.\n";
  } else if (cmd == "list_background") {
    std::cout << "Usage      : list_background\n";
    std::cout << "Description: Display tracking table of background jobs "
                 "launched by MyShell.\n";
  } else if (cmd == "list_all_processes") {
    std::cout << "Usage      : list_all_processes\n";
    std::cout << "Description: Snapshot and list ALL running processes "
                 "currently live in Windows OS.\n";
  } else if (cmd == "terminate") {
    std::cout << "Usage      : terminate <PID>\n";
    std::cout << "Description: Forcefully send a kill signal to terminate "
                 "process matching <PID>.\n";
  } else if (cmd == "suspend") {
    std::cout << "Usage      : suspend <PID>\n";
    std::cout
        << "Description: Freeze all executing threads of a target process.\n";
  } else if (cmd == "resume") {
    std::cout << "Usage      : resume <PID>\n";
    std::cout
        << "Description: Unfreeze all suspended threads of a target process.\n";
  }
  // --- ROLE 4 ---
  else if (cmd == "time") {
    std::cout << "Usage      : time\n";
    std::cout << "Description: Query Windows OS and output current local time "
                 "(HH:MM:SS).\n";
  } else if (cmd == "date") {
    std::cout << "Usage      : date\n";
    std::cout << "Description: Query Windows OS and output current local date "
                 "(DD/MM/YYYY).\n";
  } else if (cmd == "path") {
    std::cout << "Usage      : path\n";
    std::cout << "Description: Print out all stored paths registered in the OS "
                 "PATH variable.\n";
  } else if (cmd == "addpath") {
    std::cout << "Usage      : addpath <directory_path>\n";
    std::cout << "Description: Dynamically append a new folder path into the "
                 "system PATH.\n";
  } else if (cmd == "set_env") {
    std::cout << "Usage      : set_env <variable_name> <value>\n";
    std::cout << "Description: Assign an internal environment variable & sync "
                 "it to Windows OS.\n";
  } else if (cmd == "unset_env") {
    std::cout << "Usage      : unset_env <variable_name>\n";
    std::cout << "Description: Erase an environment variable from the shell "
                 "memory and OS scope.\n";
  } else if (cmd == "print_env") {
    std::cout << "Usage      : print_env <variable_name>\n";
    std::cout << "Description: Lookup and print the stored value of a specific "
                 "environment variable.\n";
  } else if (cmd == "list_env") {
    std::cout << "Usage      : list_env\n";
    std::cout << "Description: Print all stored shell environment variables.\n";
  } else if (cmd == "run") {
    std::cout << "Usage      : run <script_file.bat>\n";
    std::cout << "Description: Read a batch script line-by-line and dispatch "
                 "them to the shell engine.\n";
  } else if (cmd == "help") {
    std::cout << "Usage      : help [command_name]\n";
    std::cout
        << "Description: The manual system you are looking at right now.\n";
  } else {
    std::cout << "Error: No manual entry found for command '" << cmd << "'.\n";
  }
  std::cout << "---------------------------------------------------------------"
               "---------------\n\n";
}

#endif // HELP_H
