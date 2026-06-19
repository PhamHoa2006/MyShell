# 👥 Team Workload Division

Based on a team of 4 members, the MyShell project has been divided into 4 distinct roles. The workload is balanced, and the tasks are grouped by functional similarity. 

> **🍏 Mac User Notice:** The **Core Architect (Role 1)** role is designed specifically for the macOS user. It uses only Standard C++ and requires no Windows-specific APIs (`windows.h`).

---

## 💻 Role 1: The Core Architect (Perfect for Mac User 🍏)
**Focus:** The Engine, Input Parsing, and Command History.
This role builds the brain of the shell. You will handle the REPL loop, make sure the shell understands complex commands, and build the history system. Since this relies entirely on Standard C++ string manipulation and data structures, it can be developed and tested fully on macOS.

**Assigned Files:**
- `main.cpp` (REPL loop, tokenizer, command dispatcher)
- `Feature/history.h` (Save and load command history)

**Key Tasks:**
- Write the `while(true)` REPL loop.
- Upgrade the tokenizer to handle quotation marks (e.g., `write_file "hello world" out.txt`).
- Implement the `history` command (read/write to `history.txt`).
- Implement the `exit` command to quit the shell.
- Build the command dispatcher (`execute_command`) that routes commands to the correct feature function.

**Libraries:** `<iostream>`, `<string>`, `<vector>`, `<fstream>`, `<sstream>`

---

## 📁 Role 2: The File System Master
**Focus:** Navigation, Files, and Directories.
This role handles everything related to the file system. You will build tools to move around the system, manipulate files, and display directory trees.

**Assigned Files:**
- `Feature/navigation.h` (`cd`, `pwd`, `dir`)
- `Feature/file.h` (Finish reading/writing, copying, and moving files)
- `Feature/directory.h` (Create, delete, and list folder trees)

**Key Tasks:**
- Implement `cd`, `pwd`, and a basic `dir`.
- Finish `file.h` by adding `readFile`, `copyFile`, and fixing `writeFile` to support append/head/foot modes.
- Implement folder creation/deletion, and write a recursive function for `list_tree` to show a beautiful folder tree.

**Libraries:** `<filesystem>` (C++17), `<fstream>`

---

## ⚙️ Role 3: The Process Controller
**Focus:** Process Management (The Core of the OS Course).
This role is the most critical for an Operating Systems class. You will write code to launch other programs, track them, pause them, and kill them. This role requires deep interaction with the Windows API.

**Assigned Files:**
- `Feature/process.h`
- `Process/child.cpp`
- `Process/countdown.cpp`

**Key Tasks:**
- Implement `start_foreground` (create process and wait) and `start_background` (create process without waiting).
- Implement `terminate <PID>` to kill running background processes.
- Use Toolhelp32 to snapshot and list all running processes (`list_processes`).
- **Track the shell's own background processes** with status (PID, Cmd name, Running/Suspended/Terminated) using an internal tracking table (`list` command).
- Implement `suspend` and `resume` to pause and unpause programs.
- **Handle CTRL+C signal** using `SetConsoleCtrlHandler()` to kill only the running foreground child process without terminating the shell itself.

**Libraries:** `<windows.h>`, `<tlhelp32.h>`

---

## 🛠️ Role 4: System & Shell Utilities
**Focus:** System Info, Environment Variables, Help System, Batch Execution, and Testing.
This role provides the supporting utilities that make the shell complete. You will manage environment variables (like `PATH`), query the OS for time/date, document all commands, and enable batch script execution.

**Assigned Files:**
- `Feature/environment.h`
- `Feature/system_utils.h`
- `Feature/help.h`
- `Testcase/test_basic.bat`

**Key Tasks:**
- Implement `path` and `addpath` using `getenv` and `_putenv_s` to view/modify the PATH environment variable.
- Implement `set_env`, `unset_env`, and `print_env` using `std::map` and Windows environment functions.
- Implement commands to query `time` and `date` (`GetLocalTime`).
- Implement the `help` system to list and describe ALL shell commands (coordinate with other roles for command names).
- **Implement `.bat` file execution**: read a `.bat` file line-by-line and feed each line into the shell's `execute_command()` function. Agree with Role 1 on the function signature.
- **Write test scripts** in `Testcase/test_basic.bat` to demonstrate and test shell features.

**Libraries:** `<windows.h>`, `<cstdlib>`, `<map>`, `<fstream>`, `<iostream>`

---

## 🤝 How to Collaborate
1. **The Mac User (Role 1)** can compile `main.cpp` with `g++ -std=c++17 main.cpp` on their MacBook and test the tokenizer, history, and help commands. They can comment out the `#include` lines for Windows-specific features temporarily if needed to test locally.
2. Roles 2, 3, and 4 will write their `.h` files on Windows.
3. Once Role 1 finishes the dispatcher in `main.cpp`, all the feature files from Roles 2, 3, and 4 can be seamlessly plugged in.
4. **Role 4** needs to agree with **Role 1** on the `execute_command()` function signature for `.bat` file execution. All other roles can work **fully in parallel**.

## 📋 Interface Agreement
Role 1 must expose the following function that Role 4 will call for `.bat` execution:
```cpp
void execute_command(const string& command, const vector<string>& args);
```
