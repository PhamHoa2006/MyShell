# MyShell — A Simple Shell for Windows

A command-line shell built in C++ for the Operating System course.

## Team Members

> Xuan Son, Pham Hoa, Viet Tung, Pham Khanh

**👉 See [TEAM_TASKS.md](TEAM_TASKS.md) for how the workload is divided among the 4 team members.**

## Project Structure

```
MyShell/
├── main.cpp                    # Entry point — the REPL loop
├── Feature/                    # All shell features (header-only modules)
│   ├── feature.h               # Master include — pulls in all headers
│   ├── file.h                  # File operations (create, delete, open, rename, write)
│   ├── directory.h             # Directory operations (create, delete, copy, tree)
│   ├── navigation.h            # cd, pwd, dir
│   ├── process.h               # Process management (foreground/background, kill, suspend, resume, CTRL+C)
│   ├── environment.h           # Environment variables (path, addpath, set_env)
│   ├── system_utils.h          # System info (time, date) + batch file execution
│   ├── history.h               # Command history
│   └── help.h                  # Help system
├── Process/                    # Separate child process executables
│   ├── child.cpp               # Minimal child process
│   └── countdown.cpp           # Countdown timer
├── Testcase/                   # Test scripts
│   └── test_basic.bat          # Test commands (runs via .bat execution feature)
├── USER_MANUAL.md              # 📖 Complete tutorial and list of all commands
└── README.md                   # This file
```

## Build & Run

### Requirements
- Windows 10 or later
- g++ (MinGW) or MSVC compiler
- C++17 support

### Compile (simple, no CMake)
```bash
g++ -std=c++17 -o MyShell.exe main.cpp
```

### Run
```bash
MyShell.exe
```

## Development Phases

| Phase | What to build | Key APIs to learn |
|-------|--------------|-------------------|
| 1 | REPL loop + tokenizer | `std::getline`, `std::stringstream` |
| 2 | Navigation (cd, pwd, dir) | `SetCurrentDirectoryA`, `GetCurrentDirectoryA`, `FindFirstFileA` |
| 3 | File operations | `std::filesystem`, `ShellExecuteA` |
| 4 | Directory operations | `CreateDirectoryA`, `RemoveDirectoryA`, recursive traversal |
| 5 | Process management | `CreateProcess`, `TerminateProcess`, `WaitForSingleObject` |
| 6 | CTRL+C signal handling | `SetConsoleCtrlHandler` |
| 7 | Environment variables | `getenv`, `_putenv_s` |
| 8 | System utilities (time, date) | `GetLocalTime` |
| 9 | Help system + .bat execution | `std::ifstream`, `execute_command` |

## Reference

The reference TinyShell project is available at: [HaiAu2501/Operating-System-Projects](https://github.com/HaiAu2501/Operating-System-Projects).
For a complete tutorial of all commands, see [USER_MANUAL.md](USER_MANUAL.md).
