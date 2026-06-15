// ============================================================================
// directory.h — Directory Operations
// ============================================================================
// Commands to implement:
//   create_dir <path>       — Create a new directory
//   delete_dir <path>       — Delete a directory (and all its contents)
//   copy_dir <src> <dst>    — Copy a directory recursively
//   move_dir <src> <dst>    — Move or rename a directory
//   list_tree [path]        — Print directory structure as a visual tree
//
// Libraries needed:
//   #include <filesystem>    — fs::create_directories, fs::remove_all, etc.
//   #include <iostream>
//   #include <string>
//   #include <vector>
//
// Reference: ../Operating-System-Projects/Feature/directory.h
// ============================================================================

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class DirectoryManager
{
public:
    // --- create_dir <path> ---
    void createDirectory(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check if args is empty
        // 2. Use fs::create_directories(args[0])
        //    (This automatically creates parent folders if they don't exist)
    }

    // --- delete_dir <path> ---
    void deleteDirectory(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check if the directory exists: fs::exists(args[0])
        // 2. Use fs::remove_all(args[0]) to delete the folder AND everything inside it
    }

    // --- copy_dir <src> <dst> ---
    void copyDirectory(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check if you have exactly 2 arguments
        // 2. Use fs::copy(args[0], args[1], fs::copy_options::recursive)
    }

    // --- move_dir <src> <dst> ---
    void moveDirectory(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // Steps:
        // 1. Check if you have exactly 2 arguments
        // 2. Use fs::rename(args[0], args[1])
    }

    // --- list_tree [path] ---
    // Print a visual tree of the directory structure
    void listTree(const std::vector<std::string>& args)
    {
        // TODO: Implement this function
        // 1. Determine starting path (current_path if args empty, else args[0])
        // 2. Call a recursive helper function to print the tree
        //
        // Hint for the visual part:
        // You'll need to pass a 'depth' or 'prefix' string to the recursive function
        // to print the "├── " and "└── " formatting.
    }

private:
    // Helper function for listTree
    void printTreeRecursive(const fs::path& path, const std::string& prefix)
    {
        // TODO: Implement the recursion logic here
    }
};

#endif // DIRECTORY_H
