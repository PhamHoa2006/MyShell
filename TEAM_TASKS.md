# 👥 Team Workload Division

Based on a team of 4 members, the MyShell project has been divided into 4 distinct roles. The workload is balanced, and the tasks are grouped by functional similarity. 

> **🍏 Mac User Notice:** The **Core Architect (Role 1)** role is designed specifically for the macOS user. It uses only Standard C++ and requires no Windows-specific APIs (`windows.h`).

---

## 💻 Role 1: The Core Architect (Perfect for Mac User 🍏)
**Focus:** The Engine, Input Parsing, and User Experience.
This role builds the brain of the shell. You will handle the REPL loop, make sure the shell understands complex commands, and build the history/help systems. Since this relies entirely on Standard C++ string manipulation and data structures, it can be developed and tested fully on macOS.

**Assigned Files:**
- `main.cpp` (REPL loop, tokenizer, command dispatcher)
- `Feature/history.h` (Save and load command history)
- `Feature/help.h` (Print help instructions)

**Key Tasks:**
- Write the `while(true)` REPL loop.
- Upgrade the tokenizer to handle quotation marks (e.g., `write_file "hello world" out.txt`).
- Implement the `history` command (read/write to `history.txt`).
- Implement the `help` system to list all commands.

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
- Implement `terminate <PID>` to kill running processes.
- Use Toolhelp32 to snapshot and list all running processes (`list_processes`).
- Implement `suspend` and `resume` to pause and unpause programs.

**Libraries:** `<windows.h>`, `<tlhelp32.h>`

---

## 🛠️ Role 4: System & Environment Administrator
**Focus:** OS Configuration and Hardware Information.
This role bridges the gap between the shell and the underlying hardware/OS state. You will manage environment variables (like `PATH`) and query the OS for hardware statistics (RAM, CPU, Disk, Time).

**Assigned Files:**
- `Feature/environment.h`
- `Feature/system_utils.h`

**Key Tasks:**
- Implement `set_env`, `unset_env`, and `print_env` using `std::map` and Windows environment functions (`_putenv_s`).
- Implement commands to query time/date (`GetLocalTime`).
- Implement hardware queries: CPU info (`GetSystemInfo`), Memory (`GlobalMemoryStatusEx`), and Disk space (`GetDiskFreeSpaceExA`).

**Libraries:** `<windows.h>`, `<cstdlib>`, `<map>`

---

## 🤝 How to Collaborate
1. **The Mac User (Role 1)** can compile `main.cpp` with `g++ -std=c++17 main.cpp` on their MacBook and test the tokenizer, history, and help commands. They can comment out the `#include` lines for Windows-specific features temporarily if needed to test locally.
2. Roles 2, 3, and 4 will write their `.h` files on Windows.
3. Once Role 1 finishes the dispatcher in `main.cpp`, all the feature files from Roles 2, 3, and 4 can be seamlessly plugged in.
