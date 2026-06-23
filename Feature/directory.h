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
#include <unordered_set>
using namespace std;
namespace fs = filesystem;

class DirectoryManager
{
public:
    static const unordered_set<string> supportedCommands;

    static const unordered_set<string> &getSupportedCommands()
    {
        return supportedCommands;
    }
    // --- create_dir <path> ---
    void createDirectory(const vector<string>& args)
    {
        // Steps:
        // 1. Check if args is empty
        // 2. Use fs::create_directories(args[0])
        //    (This automatically creates parent folders if they don't exist)
        if (args.empty()) {
            cerr << "Usage: create_dir <path>" << endl;
            return;
        }
        fs::path dirPath = args[0]; 
        try {
            if (fs::exists(dirPath)) {
                cerr << "Directory already exists: " << dirPath << endl;
                return;
            }
            fs::create_directories(dirPath);
            cout << "Directory created: " << dirPath << endl;
        } catch (const exception& e) {
            cerr << "Error creating directory: " << e.what() << endl;
        }
    }

    // --- delete_dir <path> ---
    void deleteDirectory(const vector<string>& args)
    {
        // Steps:
        // 1. Check if the directory exists: fs::exists(args[0])
        // 2. Use fs::remove_all(args[0]) to delete the folder AND everything inside it

        if (args.empty()) {
            cerr << "Usage: delete_dir <path>" << endl;
            return;
        }
        fs::path dirPath = args[0];
        try {
            if (!fs::exists(dirPath)) {
                cerr << "Directory does not exist: " << dirPath << endl;
                return;
            }
            fs::remove_all(dirPath);
            cout << "Directory deleted: " << dirPath << endl;
        } catch (const exception& e) {
            cerr << "Error deleting directory: " << e.what() << endl;
        }
    }

    // --- copy_dir <src> <dst> ---
    void copyDirectory(const vector<string>& args)
    {
        // Steps:
        // 1. Check if you have exactly 2 arguments
        if (args.size() != 2) {
            cerr << "Usage: copy_dir <src> <dst>" << endl;
            return;
        }
        // 2. Use fs::copy(args[0], args[1], fs::copy_options::recursive)

        fs::path sourcePath = args[0];
        fs::path destinationPath = args[1];
        try {
            if (!fs::exists(sourcePath) || !fs::is_directory(sourcePath)) {
                cerr << "Source directory does not exist or is not a directory: " << sourcePath << endl;
                return; 
            }
            if (fs::exists(destinationPath)) {
                if (fs::is_directory(destinationPath)) {
                    // Nếu nơi đến là thư mục có sẵn, tự động hiểu là copy VÀO TRONG thư mục đó
                    // Ví dụ: Folder_B + Folder_A => Folder_B/Folder_A
                    destinationPath = destinationPath / sourcePath.filename();

                    // Check lại xem bên trong Folder_B đã có sẵn thư mục con tên Folder_A chưa
                    if (fs::exists(destinationPath)) {
                        cerr << "Error: " << destinationPath.filename().string() << " already exists inside destination folder." << endl;
                        return;
                    }
                } else {
                    cerr << "Destination already exists and is a file: " << destinationPath << endl;
                    return;
                }
            }
            fs::copy(sourcePath, destinationPath, fs::copy_options::recursive);
            cout << "Directory copied from " << sourcePath << " to " << destinationPath << endl;
        } catch (const exception& e) {
            cerr << "Error copying directory: " << e.what() << endl;
        }
    }

    // --- move_dir <src> <dst> ---
    void moveDirectory(const vector<string>& args)
    {
        // Steps:
        // 1. Check if you have exactly 2 arguments
        // 2. Use fs::rename(args[0], args[1])
            if (args.size() != 2) {
                cerr << "Usage: move_dir <src> <dst>" << endl;
                return;
            }
            fs::path sourcePath = args[0];
            fs::path destinationPath = args[1];
            try {
                if (!fs::exists(sourcePath)) {
                    cerr << "Source directory does not exist: " << sourcePath << endl;
                    return;
                }
                if (fs::exists(destinationPath)) {
                    if (fs::is_directory(destinationPath)) {
                        // Nếu nơi đến là thư mục có sẵn, tự động hiểu là copy VÀO TRONG thư mục đó
                        // Ví dụ: Folder_B + Folder_A => Folder_B/Folder_A
                        destinationPath = destinationPath / sourcePath.filename();

                        // Check lại xem bên trong Folder_B đã có sẵn thư mục con tên Folder_A chưa
                        if (fs::exists(destinationPath)) {
                            cerr << "Error: " << destinationPath.filename().string() << " already exists inside destination folder." << endl;
                            return;
                        }
                    } else {
                        cerr << "Destination already exists and is a file: " << destinationPath << endl;
                        return;
                    }
                }
                fs::path parentPath = destinationPath.parent_path();
                if (!parentPath.empty() && !fs::exists(parentPath)) {
                    // Để an toàn hơn, nên dùng trực tiếp hàm của thư viện thay vì gọi hàm createDirectory tùy biến
                    fs::create_directories(parentPath); 
                }
                fs::rename(sourcePath, destinationPath);
                cout << "Directory moved from " << sourcePath << " to " << destinationPath << endl;
            } catch (const exception& e) {
                cerr << "Error moving directory: " << e.what() << endl;
            }
    }

    // --- list_tree [path] ---
    // Print a visual tree of the directory structure
    void listTree(const vector<string>& args)
    {
        // 1. Determine starting path (current_path if args empty, else args[0])
        // 2. Call a recursive helper function to print the tree
        //
        // Hint for the visual part:
        // You'll need to pass a 'depth' or 'prefix' string to the recursive function
        // to print the "├── " and "└── " formatting.

        fs::path startPath;
        if (args.empty()) {
            startPath = fs::current_path();
        } else {
            startPath = args[0];
        }
        if (!fs::exists(startPath) || !fs::is_directory(startPath)) {
            cerr << "directory does not exist or is not a directory: " << startPath << endl;
            return;
        }
        cout << startPath.string() << endl;
        printTreeRecursive(startPath, "");
    }

private:
    // Helper function for listTree
    void printTreeRecursive(const fs::path& path, const string& prefix)
    {
        // 1. List all entries in the directory using fs::directory_iterator
        // 2. For each entry, print the prefix + "├── " + entry name
        // 3. If the entry is a directory, call printTreeRecursive on it with an updated prefix
        // 4. For the last entry in the directory, use "└── " instead of "├── "
        vector<fs::directory_entry> entries;
        for (const auto& entry : fs::directory_iterator(path)) {
            string name = entry.path().filename().string();
    
            // Bỏ qua thư mục ẩn hoặc thư mục build nặng
            if (name.rfind(".", 0) == 0 || name == "node_modules" || name == "build") {
                continue; 
            }
            entries.push_back(entry);
        }
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& entry = entries[i];
            
            bool isLast = (i == entries.size() - 1); 
            if (isLast) {
                cout << prefix << "+-- " << entry.path().filename().string() << endl;
            } else {
                cout << prefix << "|-- " << entry.path().filename().string() << endl;
            }
            if (entry.is_directory()) {
                printTreeRecursive(entry.path(), prefix + (isLast ? "    " : "|  "));
            }
        }
    }
};

const unordered_set<string> DirectoryManager::supportedCommands = {
    "copy",
    "create",
    "delete",
    "list_tree",
    "move"};


#endif // DIRECTORY_H
