# 📖 MyShell: Complete User Tutorial

Since you focused entirely on Role 3 (Process Management), here is a complete crash course on everything your teammates built. You can use this guide to test their features or explain them during your project presentation!

---

## 🧭 Role 1: Core Architect (Basic Navigation)
These are the foundational commands that run the shell itself.

*   `help` - Prints the master list of all commands.
*   `history` - Shows every command you've typed since you started using the shell (it saves across sessions!).
*   `clear_history` - Wipes your saved history file clean.
*   `exit` (or `quit`) - Safely kills all background processes and closes the shell.

---

## 📂 Role 2: File & Directory System
Your Role 2 teammate built a massive suite of commands that act just like the real Windows command prompt.

### Navigating Directories
*   `pwd` - Prints your current folder path.
*   `dir` (or `ls`) - Lists all files and folders in your current directory.
*   `cd <path>` - Changes your current folder.
    *   *Example:* `cd Desktop` or `cd ..` (to go up one folder).
*   `tree` - Prints a beautiful visual tree of your current folder structure.

### Creating & Deleting
*   `mkdir <name>` - Creates a new folder.
*   `rmdir <name>` - Deletes a folder and everything inside it.
*   `mkfile <name>` - Creates an empty file.
*   `del <name>` - Deletes a file.

### Editing & Viewing Files
*   `cat <name>` - Prints the text inside a file to the screen.
*   `write <text> <name>` - Writes text into a file.
    *   *Example:* `write "Hello World" test.txt`
*   `move <src> <dst>` - Moves a file to a new folder.
*   `rename <old> <new>` - Renames a file.
*   `cpdir <src> <dst>` - Copies an entire folder recursively.
*   `mvdir <src> <dst>` - Moves an entire folder.
*   `size <name>` - Tells you how big a file is in bytes.
*   `start <name>` - Opens a file using the default Windows application (e.g., opens a `.png` in your photo viewer).

---

## ⚙️ Role 3: Process Management (Your Masterpiece!)
You already know these, but here is how they fit into the bigger picture:

*   `start_foreground <exe>` - Runs a program and freezes the shell until the program finishes.
*   `start_background <exe>` - Runs a program in the background so you can keep typing in the shell.
*   `list` - Shows all the background processes you've launched.
*   `list_all_processes` - Shows a massive snapshot of every single process running on your Windows PC.
*   `suspend <PID>` - Freezes a running process.
*   `resume <PID>` - Unfreezes a suspended process.
*   `terminate <PID>` - Force-kills a process.

---

## 🌍 Role 4: Utilities & Environment
Your Role 4 teammate handled Windows environment variables and batch scripting.

### System Info
*   `time` - Prints the exact current time.
*   `date` - Prints the exact current date.

### Environment Variables
Environment variables are hidden system values (like your Windows username or temporary paths).
*   `list_env` - Shows all shell environment variables.
*   `set_env <var> <value>` - Creates or changes a variable.
    *   *Example:* `set_env MY_NAME Hoa`
*   `print_env <var>` - Prints the value of a specific variable.
*   `unset_env <var>` - Deletes a variable.
*   `path` - Prints your massive Windows System PATH string.
*   `addpath <dir>` - Adds a new folder to your system PATH so Windows can find executables there.

### Batch Scripting
*   `run <file.bat>` - This is the coolest utility command! If you write a text file containing a list of shell commands, this command will read the file and execute them all one-by-line automatically.
