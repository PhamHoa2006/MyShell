#define TEST_ROLE1

#include <csignal>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Feature/feature.h"

using namespace std;
namespace fs = std::filesystem;

CommandHistory cmdHistory;

void print_startup_info() {
#ifdef _WIN32
  DWORD pid = GetCurrentProcessId();
#else
  pid_t pid = getpid();
#endif
  cout << "========================================" << endl;
  cout << "              Small Shell                " << endl;
  cout << "========================================" << endl;
  cout << "Welcome to Small Shell!" << endl;
  cout
      << "This is a simple shell program to interact with the operating system."
      << endl;
  cout << "PID of Small Shell: " << pid << endl;
  cout << "Type 'help' to see the list of available commands." << endl;
  cout << "========================================" << endl;
}

void execute_command(const string &command, const vector<string> &args) {
  if (command == "help") {
    showHelp(args);
  } else if (command == "history") {
    cmdHistory.show();
  } else if (command == "clear_history") {
    cmdHistory.clear();
  } else {
#ifndef TEST_ROLE1
    // Other role commands would be handled here
#else
    cout << "Unknown command: " << command << ". Type 'help' for support."
         << endl;
#endif
  }
}

vector<string> split_input(const string &input) {
  vector<string> tokens;
  string current = "";
  bool in_quotes = false;

  for (size_t i = 0; i < input.length(); ++i) {
    char c = input[i];
    if (c == '"') {
      in_quotes = !in_quotes;
    } else if (c == ' ' && !in_quotes) {
      if (!current.empty()) {
        tokens.push_back(current);
        current.clear();
      }
    } else {
      current += c;
    }
  }
  if (!current.empty()) {
    tokens.push_back(current);
  }

  return tokens;
}

void sigint_handler(int signal) {
  // When CTRL+C is pressed, we just output a newline.
  // The active getline in main will fail, and we will clear cin's error state
  // and re-print the prompt.
  cout << endl;
}

int main() {
  // Set up local CTRL+C handler
  signal(SIGINT, sigint_handler);

  // Load command history
  cmdHistory.load();

  print_startup_info();

  while (true) {
    string prompt = "";
    try {
      prompt = fs::current_path().string();
    } catch (...) {
      prompt = "MyShell";
    }
    cout << prompt << "> ";

    string input;
    if (!getline(cin, input)) {
      if (cin.eof()) {
        cout << "\nExiting Small Shell..." << endl;
        break;
      }
      // If interrupted by SIGINT (Ctrl+C), reset error flags and continue loop
      cin.clear();
      continue;
    }

    // Simple trim whitespace from start and end
    size_t start = input.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
      continue; // Empty line
    }
    size_t end = input.find_last_not_of(" \t\r\n");
    input = input.substr(start, end - start + 1);

    if (input.empty()) {
      continue;
    }

    // Save command to history
    cmdHistory.add(input);

    // Parse input
    vector<string> tokens = split_input(input);
    if (tokens.empty()) {
      continue;
    }

    string command = tokens[0];
    vector<string> args(tokens.begin() + 1, tokens.end());

    if (command == "exit") {
      cout << "Exiting Small Shell..." << endl;
      break;
    }

    execute_command(command, args);
  }

  return 0;
}