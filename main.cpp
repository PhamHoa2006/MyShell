// VIết vòng lặp để chạy chương trình

#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <sstream>
#include <csignal>
#include "Feature/feature.h"

using namespace std;    

void print_startup_info() {
    DWORD pid = GetCurrentProcessId();
    cout << "========================================" << endl;
    cout << "              Small Shell                " << endl;
    cout << "========================================" << endl;
    cout << "Welcome to Small Shell!" << endl;
    cout << "This is a simple shell program to interact with the Windows operating system." << endl;
    cout << "PID of Small Shell: " << pid << endl;
    cout << "Type 'help' to see the list of available commands." << endl;
    cout << "========================================" << endl;
}

void execute_command(const string& command, const vector<string>& args){

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
    print_startup_info();
    return 0;
}