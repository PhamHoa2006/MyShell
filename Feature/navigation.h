// ============================================================================
// navigation.h — Shell Navigation Commands
// ============================================================================
// Commands to implement:
//   cd <path>     — Change the current working directory
//   pwd           — Print the current working directory
//   dir [path]    — List contents of a directory
//
// Libraries needed:
//   #include <filesystem>    — fs::current_path(), fs::directory_iterator
//   #include <iostream>      — cout for output
//   #include <vector>        — for args
//   #include <string>        — for string
//
// Reference: ../Operating-System-Projects/Feature/navigation.h
// ============================================================================

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <iomanip>

namespace fs = std::filesystem;

// --- cd <path> ---
// Change the current working directory
// API to use: fs::current_path(path)
// Don't forget to check if the path exists first!
void changeDirectory(const std::vector<std::string>& args)
{
    // TODO: Implement this function
    // Steps:
    // 1. Check if args is empty → print usage: "Usage: cd <path>"
    // 2. Check if args[0] is a valid path → fs::exists(args[0])
    // 3. Check if it's a directory → fs::is_directory(args[0])
    // 4. Change directory → fs::current_path(args[0])
    // 5. Handle exceptions with try/catch
}

// --- pwd ---
// Print the current working directory
// API to use: fs::current_path()
void printWorkingDirectory(const std::vector<std::string>& args)
{
    // TODO: Implement this function
    // Just one line: cout << fs::current_path() << endl;
}

// --- dir [path] ---
// List directory contents (like Windows 'dir' command)
// API to use: fs::directory_iterator
void listDirectoryContents(const std::vector<std::string>& args)
{
    // TODO: Implement this function
    // Steps:
    // 1. Determine which path to list:
    //    - If args is empty → use fs::current_path()
    //    - Otherwise → use args[0]
    // 2. Check if the path exists and is a directory
    // 3. Iterate with fs::directory_iterator(path)
    // 4. For each entry, print:
    //    - [DIR] if it's a directory, [FILE] if it's a file
    //    - The entry name
}

#endif // NAVIGATION_H
