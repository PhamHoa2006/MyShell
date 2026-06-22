#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>

// Include your new process.h and Role 2's headers
#include "Feature/process.h" 
#include "Feature/navigation.h"
#include "Feature/file.h"
#include "Feature/directory.h"

using namespace std;    

// Initialize the Managers globally or inside main
ProcessManager pm;
FileManager fm;
DirectoryManager dm;

void print_startup_info() {
    DWORD pid = GetCurrentProcessId();
    cout << "========================================" << endl;
    cout << "              MyShell                   " << endl;
    cout << "========================================" << endl;
    cout << "Welcome to MyShell (Testing Branch 'Son')" << endl;
    cout << "PID of MyShell: " << pid << endl;
    cout << "Process commands: start_foreground, start_background, terminate, suspend, resume, list_background, list_all_processes, exit" << endl;
    cout << "Navigation commands: cd, pwd, dir" << endl;
    cout << "File commands: mkfile, del, cat, write, move, rename, start, size" << endl;
    cout << "Directory commands: mkdir, rmdir, cpdir, mvdir, tree" << endl;
    cout << "========================================" << endl;
}

// A simple dispatcher just for your Process Management commands
void execute_command(const string& command, const vector<string>& args){
    if (command == "start_foreground") {
        pm.startProcessForeground(args);
    } else if (command == "start_background") {
        pm.startProcessBackground(args);
    } else if (command == "terminate") {
        pm.terminateProcess(args);
    } else if (command == "suspend") {
        pm.suspendProcess(args);
    } else if (command == "resume") {
        pm.resumeProcess(args);
    } else if (command == "list_background") {
        pm.listBackgroundProcesses(args);
    } else if (command == "list_all_processes") {
        pm.listProcesses(args);
        
    // --- NAVIGATION COMMANDS (Role 2) ---
    } else if (command == "cd") {
        changeDirectory(args);
    } else if (command == "pwd") {
        printWorkingDirectory(args);
    } else if (command == "dir" || command == "ls") {
        listDirectoryContents(args);
        
    // --- FILE COMMANDS (Role 2) ---
    } else if (command == "mkfile") {
        fm.createFile(args);
    } else if (command == "del" || command == "rm") {
        fm.deleteFile(args);
    } else if (command == "cat" || command == "type") {
        fm.readFile(args);
    } else if (command == "write" || command == "echo") {
        fm.writeFile(args);
    } else if (command == "move" || command == "mv") {
        fm.moveFile(args);
    } else if (command == "rename" || command == "ren") {
        fm.renameFile(args);
    } else if (command == "start") {
        fm.openFile(args);
    } else if (command == "size") {
        fm.fileSize(args);

    // --- DIRECTORY COMMANDS (Role 2) ---
    } else if (command == "mkdir" || command == "md") {
        dm.createDirectory(args);
    } else if (command == "rmdir" || command == "rd") {
        dm.deleteDirectory(args);
    } else if (command == "cpdir" || command == "xcopy") {
        dm.copyDirectory(args);
    } else if (command == "mvdir") {
        dm.moveDirectory(args);
    } else if (command == "tree") {
        dm.listTree(args);

    // --- EXIT ---
    } else if (command == "exit" || command == "quit") {
        cout << "Exiting testing shell..." << endl;
        exit(0);
    } else {
        cout << "Unknown command: " << command << endl;
    }
}

vector<string> split_input(const string& input){
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(){
    // 1. Register the Ctrl+C Handler that you wrote in process.h
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    // 2. Print Welcome Screen
    print_startup_info();
    
    // 3. The REPL (Read-Evaluate-Print Loop)
    while (true) {
        cout << "\nMyShell> ";
        string input;
        
        // Read the entire line from user
        if (!getline(cin, input)) break;
        if (input.empty()) continue;
        
        // Tokenize the input
        vector<string> tokens = split_input(input);
        if (tokens.empty()) continue;
        
        // Extract command vs arguments
        string command = tokens[0];
        vector<string> args(tokens.begin() + 1, tokens.end());
        
        // Dispatch
        execute_command(command, args);
    }
    
    return 0;
}