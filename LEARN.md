# 📖 MyShell — Learning Guide

Everything you need to learn to build this shell, organized by topic.

---

## Table of Contents

1. [C++ Standard Library](#1-c-standard-library)
2. [Windows API — Process Management](#2-windows-api--process-management)
3. [Windows API — File System](#3-windows-api--file-system)
4. [Windows API — Console](#4-windows-api--console)
5. [Windows API — System Info](#5-windows-api--system-info)
6. [Windows API — Thread & Synchronization](#6-windows-api--thread--synchronization)
7. [Key Concepts & Algorithms](#7-key-concepts--algorithms)
8. [Build by Phase](#8-build-by-phase)

---

## 1. C++ Standard Library

These are **non-Windows** C++ features you'll use everywhere.

### 1.1 Input/Output

| Function / Class | Header | What it does | Where you'll use it |
|---|---|---|---|
| `std::cout` | `<iostream>` | Print text to screen | Everywhere |
| `std::cerr` | `<iostream>` | Print error messages | Error handling |
| `std::cin` | `<iostream>` | Read user input | REPL loop |
| `std::getline(cin, str)` | `<string>` | Read a full line (with spaces) | REPL loop — reading commands |
| `std::stringstream` | `<sstream>` | Parse a string like a stream | Tokenizer (splitting input) |

**Practice exercise**: Write a program that reads lines in a loop and prints each word separately.

### 1.2 Strings

| Function | Header | What it does |
|---|---|---|
| `str.find(substr)` | `<string>` | Find a substring position |
| `str.substr(pos, len)` | `<string>` | Extract a substring |
| `str.empty()` | `<string>` | Check if string is empty |
| `str.c_str()` | `<string>` | Convert to C-style `char*` (needed for Windows APIs) |
| `std::stoi(str)` | `<string>` | Convert string to int |
| `std::stod(str)` | `<string>` | Convert string to double |
| `std::to_string(num)` | `<string>` | Convert number to string |

### 1.3 Containers

| Container | Header | What it does | Where you'll use it |
|---|---|---|---|
| `std::vector<T>` | `<vector>` | Dynamic array | Token lists, argument lists |
| `std::map<K,V>` | `<map>` | Sorted key-value store | Environment variables |
| `std::unordered_map<K,V>` | `<unordered_map>` | Fast key-value store (hash) | Command registry, aliases |
| `std::unordered_set<T>` | `<unordered_set>` | Fast unique set | Supported commands list |
| `std::stack<T>` | `<stack>` | LIFO stack | Expression evaluator (Shunting-Yard) |

### 1.4 File I/O

| Class | Header | What it does | Where you'll use it |
|---|---|---|---|
| `std::ifstream` | `<fstream>` | Read from a file | `read_file`, `load_env`, history |
| `std::ofstream` | `<fstream>` | Write to a file | `write_file`, `save_env`, history |
| `std::fstream` | `<fstream>` | Read and write | Advanced file operations |

### 1.5 Filesystem (C++17)

| Function | Header | What it does | Where you'll use it |
|---|---|---|---|
| `fs::exists(path)` | `<filesystem>` | Check if file/dir exists | File/dir operations |
| `fs::remove(path)` | `<filesystem>` | Delete a file | `delete_file` |
| `fs::remove_all(path)` | `<filesystem>` | Delete directory recursively | `delete_directory` |
| `fs::create_directories(path)` | `<filesystem>` | Create dir (including parents) | `create_directory` |
| `fs::copy(src, dst)` | `<filesystem>` | Copy file or directory | `copy_file`, `copy_directory` |
| `fs::rename(old, new)` | `<filesystem>` | Move or rename | `move_file`, `rename` |
| `fs::current_path()` | `<filesystem>` | Get or set working directory | `pwd`, `cd` |
| `fs::directory_iterator(path)` | `<filesystem>` | List directory contents | `dir`, `list_tree` |
| `fs::is_directory(path)` | `<filesystem>` | Check if path is a directory | Tree traversal |
| `fs::file_size(path)` | `<filesystem>` | Get file size in bytes | `file_size` |

**Tip**: Use `namespace fs = std::filesystem;` to save typing.

### 1.6 Signal Handling

| Function | Header | What it does | Where you'll use it |
|---|---|---|---|
| `signal(SIGINT, SIG_IGN)` | `<csignal>` | Ignore Ctrl+C | `main()` — prevent shell crash |
| `signal(SIGINT, handler)` | `<csignal>` | Custom Ctrl+C handler | File reading with interrupt |

### 1.7 Threads & Timing

| Function | Header | What it does | Where you'll use it |
|---|---|---|---|
| `std::thread t(func, args)` | `<thread>` | Create a new thread | Scheduler (`after` command) |
| `t.detach()` | `<thread>` | Run thread independently | Background scheduled commands |
| `std::this_thread::sleep_for(...)` | `<thread>` | Sleep current thread | Countdown, scheduler |
| `std::chrono::seconds(n)` | `<chrono>` | Time duration | With `sleep_for` |

---

## 2. Windows API — Process Management

**Header**: `<windows.h>`

This is the **most important** section for the OS course.

### 2.1 Creating Processes

| Function | What it does | When to use |
|---|---|---|
| `CreateProcess()` | Create a new process from an executable | `start_foreground`, `start_background` |
| `CreateProcessA()` | ANSI version of CreateProcess | Use this for `char*` strings |

**Key structures you must understand**:

```
STARTUPINFOA si        — Describes how the new process window should look
PROCESS_INFORMATION pi — OUTPUT: contains the new process handle & PID

You MUST initialize:  si.cb = sizeof(si);
```

**CreateProcess parameters** (10 total — memorize these):

```
CreateProcessA(
    NULL,           // lpApplicationName  — usually NULL
    cmdLine,        // lpCommandLine      — the command to run (e.g., "notepad.exe")
    NULL,           // lpProcessAttributes
    NULL,           // lpThreadAttributes
    FALSE,          // bInheritHandles
    0,              // dwCreationFlags    — 0 for normal, CREATE_NEW_PROCESS_GROUP for background
    NULL,           // lpEnvironment
    NULL,           // lpCurrentDirectory
    &si,            // lpStartupInfo
    &pi             // lpProcessInformation — OUTPUT
);
```

### 2.2 Waiting for Processes

| Function | What it does | When to use |
|---|---|---|
| `WaitForSingleObject(handle, timeout)` | Block until a process/thread finishes | Foreground process — wait for child to exit |
| `INFINITE` | Constant — wait forever | With WaitForSingleObject |

**Key concept**: 
- **Foreground** = `CreateProcess` + `WaitForSingleObject(pi.hProcess, INFINITE)`
- **Background** = `CreateProcess` only (no waiting)

### 2.3 Terminating Processes

| Function | What it does | When to use |
|---|---|---|
| `TerminateProcess(handle, exitCode)` | Force-kill a process | `terminate <PID>` command |
| `OpenProcess(access, inherit, PID)` | Get a handle to an existing process by PID | Before terminating |
| `CloseHandle(handle)` | Release a handle (MUST do this!) | After every CreateProcess/OpenProcess |
| `GetCurrentProcessId()` | Get PID of current process | Show shell's own PID |
| `GetExitCodeProcess(handle, &code)` | Check if process is still running | Process status check |

### 2.4 Enumerating Processes (Toolhelp API)

| Function | What it does | When to use |
|---|---|---|
| `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)` | Take a snapshot of all processes | `list_processes` |
| `Process32First(snapshot, &entry)` | Get first process from snapshot | Start iteration |
| `Process32Next(snapshot, &entry)` | Get next process | Continue iteration |

**Key structure**: 
```
PROCESSENTRY32 pe;
pe.cb = sizeof(pe);        // MUST set this!
pe.szExeFile               // Process name
pe.th32ProcessID           // PID
pe.th32ParentProcessID     // Parent PID
```

### 2.5 Thread Management

| Function | What it does | When to use |
|---|---|---|
| `CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)` | Snapshot all threads | `suspend`, `resume` |
| `Thread32First/Next(snapshot, &entry)` | Iterate threads | Find threads of a process |
| `OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadID)` | Get thread handle | Before suspend/resume |
| `SuspendThread(handle)` | Pause a thread | `suspend <PID>` |
| `ResumeThread(handle)` | Unpause a thread | `resume <PID>` |

**Key**: To suspend a PROCESS, you must suspend ALL its threads.

---

## 3. Windows API — File System

You can use **either** Windows APIs or C++17 `<filesystem>`. The reference TinyShell uses a mix of both.

| Function | What it does | C++17 equivalent |
|---|---|---|
| `CopyFileA(src, dst, failIfExists)` | Copy a file | `fs::copy()` |
| `MoveFileA(src, dst)` | Move/rename a file | `fs::rename()` |
| `CreateDirectoryA(path, NULL)` | Create a directory | `fs::create_directories()` |
| `RemoveDirectoryA(path)` | Delete empty directory | `fs::remove()` |
| `FindFirstFileA(pattern, &data)` | Start searching files | `fs::directory_iterator` |
| `FindNextFileA(handle, &data)` | Continue search | (iterator++) |
| `FindClose(handle)` | End search | (automatic) |
| `ShellExecuteA(NULL, "open", file, ...)` | Open file with default app | No C++17 equivalent |
| `SetCurrentDirectoryA(path)` | Change working directory | `fs::current_path(path)` |
| `GetCurrentDirectoryA(size, buffer)` | Get working directory | `fs::current_path()` |

**Recommendation**: Use `<filesystem>` for most things (simpler). Use Windows APIs only for `ShellExecuteA` (open file) and process management.

---

## 4. Windows API — Console

| Function | What it does | When to use |
|---|---|---|
| `GetStdHandle(STD_OUTPUT_HANDLE)` | Get console output handle | Before any console API call |
| `SetConsoleTextAttribute(handle, color)` | Change text color | Prompt color, `change_color` |
| `GetConsoleScreenBufferInfo(handle, &info)` | Get screen size | `clear` command |
| `FillConsoleOutputCharacter(...)` | Fill screen with a character | `clear` command |
| `SetConsoleCursorPosition(handle, coord)` | Move cursor | `clear` command |

**Color values** (for SetConsoleTextAttribute):

| Value | Color | Value | Color |
|-------|-------|-------|-------|
| 0 | Black | 8 | Dark Gray |
| 1 | Dark Blue | 9 | Blue |
| 2 | Dark Green | 10 | Green |
| 3 | Dark Cyan | 11 | Cyan |
| 4 | Dark Red | 12 | Red |
| 7 | Light Gray (default) | 14 | Yellow |
| 15 | White | | |

---

## 5. Windows API — System Info

| Function | What it does | Command |
|---|---|---|
| `GetLocalTime(&st)` | Get current date & time | `time`, `date` |
| `GetTickCount64()` | System uptime in milliseconds | `uptime` |
| `GetSystemInfo(&si)` | CPU info (processor count, type) | `cpuinfo` |
| `GlobalMemoryStatusEx(&mem)` | RAM usage (total, available) | `meminfo` |
| `GetDiskFreeSpaceExA(drive, ...)` | Disk space info | `diskinfo` |

---

## 6. Windows API — Thread & Synchronization

| Function | What it does | When to use |
|---|---|---|
| `CreateThread(NULL, 0, func, param, 0, &id)` | Create a new thread | Thread management demo |
| `WaitForMultipleObjects(n, handles, TRUE, INFINITE)` | Wait for multiple threads | Wait for all threads to finish |
| `CreateSemaphore(NULL, initial, max, NULL)` | Create a semaphore | Synchronization demo |
| `WaitForSingleObject(semaphore, INFINITE)` | Acquire semaphore (lock) | Enter critical section |
| `ReleaseSemaphore(semaphore, 1, NULL)` | Release semaphore (unlock) | Leave critical section |
| `CreateMutexA(NULL, FALSE, name)` | Create a named mutex | Inter-process sync (countdown) |

---

## 7. Key Concepts & Algorithms

### 7.1 The REPL Loop (Read-Eval-Print Loop)

```
while (true) {
    1. Print prompt    →  cout << "myshell> "
    2. Read input      →  getline(cin, input)
    3. Tokenize        →  split input into ["command", "arg1", "arg2", ...]
    4. Execute         →  look up command, call its function
}
```

### 7.2 Tokenizer (Input Parser)

Your current `split_input()` uses `stringstream` — this works for basic splitting but **doesn't handle**:
- Quoted strings: `write_file "Hello World" out.txt`
- Operators as separate tokens: `calculate (1+2)`

You'll need to upgrade to a character-by-character parser later.

### 7.3 Foreground vs Background Process

```
FOREGROUND:                          BACKGROUND:
  CreateProcess(...)                   CreateProcess(...)  + CREATE_NEW_PROCESS_GROUP
  WaitForSingleObject(INFINITE)        // NO wait — return to prompt
  // shell blocks here                 // shell continues
```

### 7.4 Recursive Directory Traversal

```
function listTree(path, depth):
    for each entry in directory(path):
        print indent(depth) + entry.name
        if entry is a directory:
            listTree(entry.path, depth + 1)    ← RECURSION
```

### 7.5 Shunting-Yard Algorithm (for `calculate` command)

Converts `3 + 4 * 2` → `3 4 2 * +` (postfix), then evaluates with a stack.
**Learn this only when you reach Phase 6** (expression evaluator).

---

## 8. Build by Phase

### Phase 1 — REPL Loop ✍️ (You are HERE)
**What to learn**: `std::getline`, `std::stringstream`, `std::vector`, `signal(SIGINT, SIG_IGN)`, `GetCurrentProcessId()`

**What to build**: 
- Complete the main loop in `main.cpp` (prompt → read → tokenize → dispatch)
- Add `if/else if` chain in `execute_command()` for: `help`, `exit`, `clear`

---

### Phase 2 — Navigation 🧭
**What to learn**: `fs::current_path()`, `fs::directory_iterator`, `fs::is_directory()`

**What to build in `navigation.h`**:
- `cd <path>` — change directory
- `pwd` — print working directory
- `dir [path]` — list directory contents

---

### Phase 3 — File Operations 📄 (Partially done ✅)
**What to learn**: `std::ifstream`, `std::ofstream`, `fs::exists`, `fs::remove`, `ShellExecuteA`

**What to build in `file.h`** (some already done):
- ✅ `checkFileExistence` — done
- ✅ `createFile` — done
- ✅ `deleteFile` — done
- ✅ `openFile` — done
- ✅ `renameFile` — done
- 🔧 `writeFile` — partially done (needs ~HEAD/~FOOT/~LINE modes)
- ❌ `readFile` — TODO
- ❌ `copyFile` — TODO
- ❌ `moveFile` — TODO
- ❌ `showFileSize` — TODO

---

### Phase 4 — Directory Operations 📁
**What to learn**: `fs::create_directories`, `fs::remove_all`, `fs::copy`, recursion

**What to build in `directory.h`** (currently empty):
- `createDirectory`
- `deleteDirectory` (recursive)
- `copyDirectory` (recursive)
- `moveDirectory`
- `listDirectoryTree` (recursive with `├──` / `└──` formatting)

---

### Phase 5 — Process Management ⚙️ (Most important for OS course!)
**What to learn**: `CreateProcess`, `WaitForSingleObject`, `TerminateProcess`, `OpenProcess`, `CreateToolhelp32Snapshot`, `Process32First/Next`, `SuspendThread`, `ResumeThread`

**What to build in `process.h`** (new file):
- `start_foreground <exe>` — create process + wait
- `start_background <exe>` — create process, no wait
- `terminate <PID>` — kill process
- `list_processes` — show all running processes
- `list_children` — show child processes of shell
- `suspend <PID>` — pause a process
- `resume <PID>` — unpause a process

---

### Phase 6 — Environment & Extras 🔧
**What to learn**: `getenv()`, `_putenv_s()`, `GetLocalTime()`, `GlobalMemoryStatusEx()`

**What to build**:
- `environment.h` — set_env, unset_env, print_env, list_env
- `system_utils.h` — time, date, uptime, cpuinfo, meminfo
- `history.h` — command history (save/load from file)
- `help.h` — help command

---

## Quick Reference Card

```
┌─────────────────────────────────────────────────────────┐
│                    WINDOWS API CHEATSHEET                │
├─────────────────────────────────────────────────────────┤
│ CREATE PROCESS:                                         │
│   STARTUPINFOA si = { sizeof(si) };                     │
│   PROCESS_INFORMATION pi = { 0 };                       │
│   CreateProcessA(NULL, cmd, 0,0, FALSE, 0, 0,0, &si, &pi);│
│   WaitForSingleObject(pi.hProcess, INFINITE); // fg     │
│   CloseHandle(pi.hProcess);                             │
│   CloseHandle(pi.hThread);                              │
│                                                         │
│ KILL PROCESS:                                           │
│   HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);│
│   TerminateProcess(h, 0);                               │
│   CloseHandle(h);                                       │
│                                                         │
│ LIST PROCESSES:                                         │
│   HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); │
│   PROCESSENTRY32 pe = { sizeof(pe) };                   │
│   Process32First(snap, &pe);                            │
│   do { /* pe.szExeFile, pe.th32ProcessID */ }           │
│   while (Process32Next(snap, &pe));                     │
│   CloseHandle(snap);                                    │
│                                                         │
│ CHANGE COLOR:                                           │
│   HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);           │
│   SetConsoleTextAttribute(h, 14); // 14 = yellow        │
└─────────────────────────────────────────────────────────┘
```
