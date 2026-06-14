# 📘 Practice - Luyện tập API Hệ điều hành Windows
# Practice - Windows OS API Exercises

> **Mục đích / Purpose:** Thư mục này chứa các bài tập luyện tập từ cơ bản đến nâng cao, giúp các thành viên trong nhóm nắm vững các API Windows trước khi phát triển dự án [Tiny Shell](../Operating-System-Projects/README.md).
>
> This folder contains exercises from basic to advanced, helping team members master Windows APIs before developing the [Tiny Shell](../Operating-System-Projects/README.md) project.

---

## 🗂️ Cấu trúc thư mục / Folder Structure

```
Practice/
├── README.md                    # File này / This file
├── CMakeLists.txt               # Build configuration
│
├── 01_process/                  # Quản lý tiến trình / Process Management
│   ├── 01_get_process_info.cpp      → GetCurrentProcessId, OpenProcess
│   ├── 02_create_process.cpp        → CreateProcess (chi tiết từng tham số)
│   ├── 03_foreground_background.cpp → Foreground vs Background execution
│   ├── 04_terminate_process.cpp     → TerminateProcess, GetExitCodeProcess
│   ├── 05_enumerate_processes.cpp   → CreateToolhelp32Snapshot, Process32First/Next
│   ├── 06_suspend_resume.cpp        → CREATE_SUSPENDED, ResumeThread
│   └── 07_child_processes.cpp       → WaitForMultipleObjects, parent-child tracking
│
├── 02_thread/                   # Quản lý luồng / Thread Management
│   ├── 01_create_thread.cpp         → CreateThread (chi tiết từng tham số)
│   ├── 02_multiple_threads.cpp      → WaitForMultipleObjects, thread IDs
│   ├── 03_thread_sync_mutex.cpp     → CreateMutex, race condition demo
│   ├── 04_thread_sync_critical_section.cpp → CRITICAL_SECTION
│   ├── 05_thread_sync_semaphore.cpp → CreateSemaphore, Producer-Consumer
│   ├── 06_thread_suspend_resume.cpp → SuspendThread, ResumeThread
│   └── 07_thread_pool_simulation.cpp → CreateEvent, thread pool pattern
│
├── 03_file_io/                  # Thao tác tệp tin / File I/O
│   ├── 01_create_read_write.cpp     → CreateFile, WriteFile, ReadFile
│   ├── 02_file_attributes.cpp       → GetFileAttributes, GetFileSize, GetFileTime
│   ├── 03_copy_move_delete.cpp      → CopyFile, MoveFile, DeleteFile
│   ├── 04_directory_operations.cpp  → CreateDirectory, RemoveDirectory
│   ├── 05_find_files.cpp            → FindFirstFile, FindNextFile (recursive)
│   ├── 06_file_mapping.cpp          → CreateFileMapping, MapViewOfFile
│   └── 07_async_io.cpp              → OVERLAPPED, async I/O
│
├── 04_ipc/                      # Giao tiếp liên tiến trình / Inter-Process Communication
│   ├── 01_anonymous_pipe.cpp        → CreatePipe, parent-child communication
│   ├── 02_named_pipe.cpp            → CreateNamedPipe, ConnectNamedPipe
│   ├── 03_shared_memory.cpp         → CreateFileMapping (shared memory)
│   ├── 04_mailslot.cpp              → CreateMailslot, one-to-many messaging
│   └── 05_signal_handling.cpp       → signal(), SetConsoleCtrlHandler
│
├── 05_environment/              # Biến môi trường / Environment Variables
│   ├── 01_get_set_env.cpp           → GetEnvironmentVariable, SetEnvironmentVariable
│   ├── 02_expand_env_strings.cpp    → ExpandEnvironmentStrings
│   ├── 03_environment_block.cpp     → GetEnvironmentStrings
│   └── 04_path_manipulation.cpp     → PATH parsing, add/remove/check
│
├── 06_system_info/              # Thông tin hệ thống / System Information
│   ├── 01_system_time.cpp           → GetSystemTime, GetLocalTime
│   ├── 02_cpu_info.cpp              → GetSystemInfo, processor features
│   ├── 03_memory_info.cpp           → GlobalMemoryStatusEx
│   ├── 04_disk_info.cpp             → GetDiskFreeSpaceEx, GetLogicalDrives
│   └── 05_uptime.cpp                → GetTickCount64
│
├── 07_console/                  # Console API
│   ├── 01_console_color.cpp         → SetConsoleTextAttribute
│   ├── 02_console_cursor.cpp        → SetConsoleCursorPosition
│   └── 03_console_input.cpp         → ReadConsoleInput, keyboard events
│
└── case_studies/                # Bài tập tổng hợp / Integrated Case Studies
    ├── 01_mini_task_manager.cpp      → Process listing + termination
    ├── 02_producer_consumer.cpp      → Threads + Semaphores + Mutex
    └── 03_file_watcher.cpp           → FindFirstChangeNotification
```

---

## 🔗 Ánh xạ API với Tiny Shell / API Mapping to Tiny Shell

| Bài tập / Exercise | Lệnh Tiny Shell / Tiny Shell Command | Module |
|---|---|---|
| `01_process/02_create_process` | `start_foreground`, `start_background` | `process.h` |
| `01_process/04_terminate_process` | `terminate` | `process.h` |
| `01_process/05_enumerate_processes` | `list_processes` | `process.h` |
| `01_process/06_suspend_resume` | `suspend`, `resume` | `process.h` |
| `01_process/07_child_processes` | `child`, `list_children` | `process.h` |
| `02_thread/01-07` | `manage_threads` | `manage_threads.h` |
| `03_file_io/01_create_read_write` | `create_file`, `write_file`, `read_file` | `file.h` |
| `03_file_io/02_file_attributes` | `file_size`, `check_file` | `file.h` |
| `03_file_io/03_copy_move_delete` | `copy_file`, `move_file`, `delete_file` | `file.h` |
| `03_file_io/04_directory_operations` | `create`, `delete`, `move`, `copy` | `directory.h` |
| `03_file_io/05_find_files` | `list_tree`, `list_file`, `dir` | `directory.h` |
| `05_environment/01_get_set_env` | `set_env`, `print_env`, `unset_env` | `environment.h` |
| `05_environment/03_environment_block` | `list_env` | `environment.h` |
| `05_environment/04_path_manipulation` | `add_path`, `remove_path`, `is_in_path` | `environment.h` |
| `06_system_info/01_system_time` | `time`, `date` | `system_utils.h` |
| `06_system_info/02_cpu_info` | `cpuinfo` | `system_utils.h` |
| `06_system_info/03_memory_info` | `meminfo` | `system_utils.h` |
| `06_system_info/04_disk_info` | `diskinfo` | `system_utils.h` |
| `06_system_info/05_uptime` | `uptime` | `system_utils.h` |
| `07_console/01_console_color` | `change_color` | `set_color.h` |
| `07_console/02_console_cursor` | `clear` | `clear_screen.h` |

---

## 🚀 Cách biên dịch / How to Build

### Biên dịch từng file / Compile Individual Files

```bash
# Với MinGW / With MinGW
g++ -o output.exe 01_process/01_get_process_info.cpp -lpsapi

# Với MSVC / With MSVC
cl 01_process/01_get_process_info.cpp /Fe:output.exe

# Với CMake / With CMake (build all)
mkdir build && cd build
cmake ..
cmake --build .
```

### Lưu ý / Notes

- Tất cả bài tập đều cần `windows.h` (chỉ chạy trên Windows)
- Một số bài cần thêm thư viện: `-lpsapi` (Process Status API), `-ltlhelp32` (Tool Help)
- Nên chạy cmd/terminal với quyền Administrator cho một số bài tập

---

## 📋 Lộ trình học / Learning Path

### Tuần 1: Nền tảng / Week 1: Foundations
1. ✅ `01_process/01-03` — Hiểu cơ bản về tiến trình
2. ✅ `02_thread/01-02` — Tạo và quản lý luồng
3. ✅ `03_file_io/01-03` — Thao tác tệp cơ bản

### Tuần 2: Đồng bộ hóa / Week 2: Synchronization
4. ✅ `02_thread/03-05` — Mutex, Critical Section, Semaphore
5. ✅ `01_process/04-06` — Quản lý vòng đời tiến trình
6. ✅ `04_ipc/01-03` — Pipe và Shared Memory

### Tuần 3: Nâng cao / Week 3: Advanced
7. ✅ `05_environment/01-04` — Biến môi trường
8. ✅ `06_system_info/01-05` — Thông tin hệ thống
9. ✅ `07_console/01-03` — Console API
10. ✅ `03_file_io/05-07` — File nâng cao

### Tuần 4: Tổng hợp / Week 4: Integration
11. ✅ `case_studies/01` — Mini Task Manager
12. ✅ `case_studies/02` — Producer-Consumer
13. ✅ `case_studies/03` — File Watcher
14. 🚀 Bắt đầu phát triển Tiny Shell!

---

## 👥 Thành viên / Team Members

> Mỗi thành viên nên hoàn thành tất cả bài tập. Có thể tạo branch riêng để luyện tập.

| Thành viên | Nhánh Git |
|---|---|
| (Tên thành viên 1) | `practice/member1` |
| (Tên thành viên 2) | `practice/member2` |
| (Tên thành viên 3) | `practice/member3` |
| (Tên thành viên 4) | `practice/member4` |

---

*Được tạo tự động để hỗ trợ dự án Tiny Shell - Bài tập lớn Nguyên lý Hệ điều hành*
