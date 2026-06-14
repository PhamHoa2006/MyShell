# MyShell — A Simple Shell for Windows

A command-line shell built in C++ for the Operating System course.

## Team Members

> TODO: Xuan Son, Pham Hoa, Viet Tung, Pham Khanh

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
│   ├── process.h               # TODO: Process management
│   ├── environment.h           # TODO: Environment variables
│   ├── system_utils.h          # TODO: System info (time, CPU, memory)
│   ├── history.h               # TODO: Command history
│   └── help.h                  # TODO: Help system
├── Process/                    # Separate child process executables
│   ├── child.cpp               # TODO: Minimal child process
│   └── countdown.cpp           # TODO: Countdown timer
├── Testcase/                   # Test scripts
│   └── test_basic.bat          # TODO: Test commands
├── LEARN.md                    # 📖 Learning guide — APIs & libraries reference
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
| 6 | Environment variables | `getenv`, `_putenv_s` |
| 7 | System utilities | `GetLocalTime`, `GlobalMemoryStatusEx`, `GetSystemInfo` |

## Reference

The reference TinyShell project is located at `../Operating-System-Projects/`.
The learning guide is at [LEARN.md](LEARN.md).
