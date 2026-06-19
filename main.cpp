#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>

// Include your new process.h
#include "Feature/process.h" 

using namespace std;    

// Initialize the ProcessManager globally or inside main
ProcessManager pm;

void print_startup_info() {
    DWORD pid = GetCurrentProcessId();
    cout << "========================================" << endl;
    cout << "              MyShell                   " << endl;
    cout << "========================================" << endl;
    cout << "Welcome to MyShell (Testing Branch 'Son')" << endl;
    cout << "PID of MyShell: " << pid << endl;
    cout << "Test commands: start_foreground, start_background, terminate, suspend, resume, list_background, list_all_processes, exit" << endl;
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