// ============================================================================
// main.cpp — Master Engine (Hợp nhất code nhánh Sơn + Role 4 + Role 1)
// ============================================================================

#include <windows.h> // Include this FIRST to fix std::byte ambiguity
#include <csignal>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


// --- ROLE 1 ---
#include "Feature/history.h"

// --- ROLE 2 ---
#include "Feature/directory.h"
#include "Feature/file.h"
#include "Feature/navigation.h"


// --- ROLE 3 ---
#include "Feature/process.h"

// --- ROLE 4 ---
#include "Feature/environment.h"
#include "Feature/help.h"
#include "Feature/system_utils.h"


using namespace std;
namespace fs = std::filesystem;

CommandHistory cmdHistory;

// Khởi tạo toàn bộ các Manager toàn cục
ProcessManager pm;
FileManager fm;
DirectoryManager dm;
EnvironmentManager envMgr;
SystemUtils sysUtils;
CommandHistory cmdHist;

void print_startup_info() {
  DWORD pid = GetCurrentProcessId();
  cout << "===================================================================="
          "=========="
       << endl;
  cout << "                                   MyShell                          "
          "          "
       << endl;
  cout << "===================================================================="
          "=========="
       << endl;
  cout << "Welcome to MyShell!" << endl;
  cout << "PID of MyShell: " << pid << endl;
  cout << "General commands    : help, history, clear_history, exit" << endl;
  cout << "Navigation commands : cd, pwd, dir" << endl;
  cout << "File commands       : mkfile, del, cat, write, move, rename, start, "
          "size"
       << endl;
  cout << "Directory commands  : mkdir, rmdir, cpdir, mvdir, tree" << endl;
  cout << "Process commands    : start_foreground, start_background, "
          "terminate, suspend, resume, list_background, list_all_processes"
       << endl;
  cout << "Utility & Env cmds  : time, date, path, addpath, set_env, "
          "unset_env, print_env, list_env, run"
       << endl;
  cout << "===================================================================="
          "=========="
       << endl;
}

// BỘ TÁCH TỪ THÔNG MINH (Nâng cấp từ code của Sơn để hiểu được dấu " ")
vector<string> split_input(const string &input) {
  vector<string> tokens;
  string current_token;
  bool in_quotes = false;

  for (char c : input) {
    if (c == '\"') {
      in_quotes = !in_quotes; // Lật trạng thái khi gặp ngoặc kép
    } else if (c == ' ' && !in_quotes) {
      if (!current_token.empty()) {
        tokens.push_back(current_token);
        current_token.clear();
      }
    } else {
      current_token += c;
    }
  }
  if (!current_token.empty())
    tokens.push_back(current_token);
  return tokens;
}

void execute_command(const string &command, const vector<string> &args) {

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

  } else if (command == "help") {
    showHelp(args);
  } else if (command == "time") {
    sysUtils.showSystemTime(args);
  } else if (command == "date") {
    sysUtils.showSystemDate(args);
  } else if (command == "set_env") {
    if (args.size() >= 2)
      envMgr.setEnv(args[0], args[1]);
    else
      cerr << "Usage: set_env <variable_name> <value>\n";
  } else if (command == "unset_env") {
    if (!args.empty())
      envMgr.unsetEnv(args[0]);
    else
      cerr << "Usage: unset_env <variable_name>\n";
  } else if (command == "print_env") {
    if (!args.empty())
      envMgr.printEnv(args[0]);
    else
      cerr << "Usage: print_env <variable_name>\n";
  } else if (command == "list_env") {
    envMgr.listAllEnv();
  } else if (command == "path") {
    envMgr.printPath();
  } else if (command == "addpath") {
    if (!args.empty())
      envMgr.addPath(args[0]);
    else
      cerr << "Usage: addpath <directory_path>\n";
  } else if (command == "run") {
    sysUtils.runBatFile(args);
  } else if (command == "history") {
    cmdHist.show();
  } else if (command == "clear_history") {
    cmdHist.clear();

    // --- EXIT ---
  } else if (command == "exit" || command == "quit") {
    cout << "Exiting MyShell. Goodbye!" << endl;
    pm.killAllBackgroundProcesses();
    exit(0);
  } else {
    cout << "Unknown command: " << command << ". Type 'help' for the manual."
         << endl;
  }
}

int main() {
  SetConsoleCtrlHandler(CtrlHandler, TRUE);

  cmdHist.load();

  print_startup_info();

  while (true) {
    cout << "\nMyShell> ";
    string input;

    if (!getline(cin, input))
      break;
    if (input.empty())
      continue;

    cmdHist.add(input);
    vector<string> tokens = split_input(input);
    if (tokens.empty())
      continue;

    string command = tokens[0];
    vector<string> args(tokens.begin() + 1, tokens.end());

    execute_command(command, args);
  }

  return 0;
}