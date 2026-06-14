/*
 * ============================================================================
 * Case Study 01: Mini Task Manager / Trình quản lý tác vụ mini
 * ============================================================================
 * Description (EN): A mini task manager that lists all running processes,
 *                   displays PID/name/thread count, allows terminating
 *                   a process by PID, and can auto-refresh the list.
 *                   Integrates process, console, and synchronization APIs.
 *
 * Mô tả (VI):      Trình quản lý tác vụ mini liệt kê tất cả tiến trình
 *                   đang chạy, hiển thị PID/tên/số luồng, cho phép kết thúc
 *                   tiến trình theo PID, và tự động làm mới danh sách.
 *                   Tích hợp API tiến trình, console, và đồng bộ.
 *
 * Key APIs:  CreateToolhelp32Snapshot, Process32First, Process32Next,
 *            OpenProcess, TerminateProcess, GetProcessMemoryInfo,
 *            SetConsoleTextAttribute, SetConsoleCursorPosition, Sleep
 *
 * Compile:   g++ 01_mini_task_manager.cpp -o 01_mini_task_manager.exe -lpsapi
 * ============================================================================
 */

#include <windows.h>
#include <tlhelp32.h>   // CreateToolhelp32Snapshot, PROCESSENTRY32
#include <psapi.h>      // GetProcessMemoryInfo
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ============================================================================
// Cấu trúc lưu thông tin tiến trình / Structure to store process info
// ============================================================================
struct ProcessInfo {
    DWORD  pid;
    DWORD  parentPid;
    DWORD  threadCount;
    LONG   priority;
    SIZE_T memoryUsageKB;  // Working set size in KB
    char   name[MAX_PATH];
};

// Mảng lưu danh sách tiến trình / Array to store process list
#define MAX_PROCESSES 4096
static ProcessInfo g_processes[MAX_PROCESSES];
static int g_processCount = 0;

// ============================================================================
// Hàm tiện ích Console / Console utility functions
// ============================================================================
static HANDLE g_hConsole = INVALID_HANDLE_VALUE;
static WORD   g_originalAttr = 0x07;

void InitConsole()
{
    g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(g_hConsole, &csbi)) {
        g_originalAttr = csbi.wAttributes;
    }
}

void SetColor(WORD attr)
{
    SetConsoleTextAttribute(g_hConsole, attr);
}

void ResetColor()
{
    SetConsoleTextAttribute(g_hConsole, g_originalAttr);
}

// ============================================================================
// Chuyển byte sang dạng dễ đọc / Convert bytes to human-readable format
// ============================================================================
void FormatKB(SIZE_T kb, char* buffer, int bufSize)
{
    if (kb >= 1048576) {  // >= 1 GB
        snprintf(buffer, bufSize, "%.1f GB", (double)kb / 1048576.0);
    } else if (kb >= 1024) {
        snprintf(buffer, bufSize, "%.1f MB", (double)kb / 1024.0);
    } else {
        snprintf(buffer, bufSize, "%zu KB", kb);
    }
}

// ============================================================================
// Lấy bộ nhớ đang dùng của tiến trình
// Get memory usage of a process
// ============================================================================
SIZE_T GetProcessMemoryUsageKB(DWORD pid)
{
    // OpenProcess: Mở handle đến tiến trình để truy vấn thông tin
    // OpenProcess: Open a handle to the process for querying info
    // PROCESS_QUERY_INFORMATION | PROCESS_VM_READ: Quyền đọc thông tin
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProc == NULL) return 0;

    PROCESS_MEMORY_COUNTERS pmc;
    pmc.cb = sizeof(pmc);
    SIZE_T memKB = 0;

    // GetProcessMemoryInfo: Lấy thông tin bộ nhớ tiến trình
    // GetProcessMemoryInfo: Retrieves memory usage info for the process
    if (GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc))) {
        // WorkingSetSize: Bộ nhớ vật lý đang dùng (bytes)
        // WorkingSetSize: Physical memory currently in use (bytes)
        memKB = pmc.WorkingSetSize / 1024;
    }

    CloseHandle(hProc);
    return memKB;
}

// ============================================================================
// Liệt kê tất cả tiến trình đang chạy
// Enumerate all running processes
// ============================================================================
int EnumerateProcesses()
{
    g_processCount = 0;

    // CreateToolhelp32Snapshot: Tạo snapshot (ảnh chụp) của hệ thống
    // CreateToolhelp32Snapshot: Creates a snapshot of the system
    // TH32CS_SNAPPROCESS: Chụp danh sách tất cả tiến trình
    // TH32CS_SNAPPROCESS: Capture list of all processes
    // Tham số 2 = 0: Chụp toàn bộ hệ thống (không lọc theo PID)
    // Parameter 2 = 0: Capture entire system (no PID filter)
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE) {
        printf("[ERROR] CreateToolhelp32Snapshot failed / Lỗi: %lu\n", GetLastError());
        return 0;
    }

    // PROCESSENTRY32: Cấu trúc chứa thông tin một tiến trình
    // PROCESSENTRY32: Structure containing info about one process
    // QUAN TRỌNG: Phải set dwSize trước khi gọi Process32First
    // IMPORTANT: Must set dwSize before calling Process32First
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Process32First: Lấy tiến trình đầu tiên trong snapshot
    // Process32First: Gets first process from snapshot
    if (!Process32First(hSnap, &pe)) {
        printf("[ERROR] Process32First failed / Lỗi: %lu\n", GetLastError());
        CloseHandle(hSnap);
        return 0;
    }

    do {
        if (g_processCount >= MAX_PROCESSES) break;

        ProcessInfo& pi = g_processes[g_processCount];
        pi.pid         = pe.th32ProcessID;
        pi.parentPid   = pe.th32ParentProcessID;
        pi.threadCount = pe.cntThreads;
        pi.priority    = pe.pcPriClassBase;
        strncpy(pi.name, pe.szExeFile, MAX_PATH - 1);
        pi.name[MAX_PATH - 1] = '\0';

        // Lấy bộ nhớ (có thể thất bại nếu không đủ quyền)
        // Get memory (may fail if insufficient permissions)
        pi.memoryUsageKB = GetProcessMemoryUsageKB(pe.th32ProcessID);

        g_processCount++;

    // Process32Next: Lấy tiến trình tiếp theo trong snapshot
    // Process32Next: Gets next process from snapshot
    // Trả về FALSE khi hết danh sách
    // Returns FALSE when no more entries
    } while (Process32Next(hSnap, &pe));

    // Đóng handle snapshot / Close snapshot handle
    CloseHandle(hSnap);

    return g_processCount;
}

// ============================================================================
// Hiển thị danh sách tiến trình
// Display process list
// ============================================================================
void DisplayProcessList()
{
    printf("\n");
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("  Total Processes / Tổng tiến trình: %d\n\n", g_processCount);
    ResetColor();

    // Tiêu đề bảng / Table header
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY |
             BACKGROUND_BLUE);
    printf("  %-8s %-35s %-10s %-8s %-10s %-12s\n",
           "PID", "Name / Tên", "ParentPID", "Threads", "Priority", "Memory");
    ResetColor();

    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf("  %-8s %-35s %-10s %-8s %-10s %-12s\n",
           "--------", "-----------------------------------",
           "----------", "--------", "----------", "------------");
    ResetColor();

    for (int i = 0; i < g_processCount; i++) {
        ProcessInfo& pi = g_processes[i];

        char memBuf[32];
        if (pi.memoryUsageKB > 0) {
            FormatKB(pi.memoryUsageKB, memBuf, sizeof(memBuf));
        } else {
            strcpy(memBuf, "N/A");
        }

        // Tô màu khác nhau cho dòng chẵn/lẻ / Alternate row colors
        if (i % 2 == 0) {
            SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        } else {
            SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        printf("  %-8lu %-35.35s %-10lu %-8lu %-10ld %-12s\n",
               pi.pid, pi.name, pi.parentPid,
               pi.threadCount, pi.priority, memBuf);
    }
    ResetColor();
    printf("\n");
}

// ============================================================================
// Tìm tiến trình theo tên
// Search processes by name
// ============================================================================
void SearchByName(const char* searchTerm)
{
    printf("\n  Search results for / Kết quả tìm kiếm \"%s\":\n\n", searchTerm);

    int found = 0;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("  %-8s %-35s %-10s %-12s\n", "PID", "Name / Tên", "Threads", "Memory");
    printf("  %-8s %-35s %-10s %-12s\n", "--------", "-----------------------------------",
           "----------", "------------");
    ResetColor();

    for (int i = 0; i < g_processCount; i++) {
        // Tìm kiếm không phân biệt hoa thường / Case-insensitive search
        char nameLower[MAX_PATH], searchLower[MAX_PATH];
        strncpy(nameLower, g_processes[i].name, MAX_PATH - 1);
        strncpy(searchLower, searchTerm, MAX_PATH - 1);

        for (char* p = nameLower; *p; p++) *p = (char)tolower(*p);
        for (char* p = searchLower; *p; p++) *p = (char)tolower(*p);

        if (strstr(nameLower, searchLower)) {
            char memBuf[32];
            FormatKB(g_processes[i].memoryUsageKB, memBuf, sizeof(memBuf));
            printf("  %-8lu %-35s %-10lu %-12s\n",
                   g_processes[i].pid, g_processes[i].name,
                   g_processes[i].threadCount, memBuf);
            found++;
        }
    }

    if (found == 0) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("  No processes found / Không tìm thấy tiến trình.\n");
        ResetColor();
    } else {
        printf("\n  Found / Tìm thấy: %d process(es)\n", found);
    }
    printf("\n");
}

// ============================================================================
// Kết thúc tiến trình theo PID
// Terminate a process by PID
// ============================================================================
void TerminateByPID(DWORD pid)
{
    // Kiểm tra PID hợp lệ / Validate PID
    if (pid == 0) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("  [ERROR] Cannot terminate System Idle Process (PID 0).\n");
        printf("  [LỖI]  Không thể kết thúc tiến trình System Idle (PID 0).\n");
        ResetColor();
        return;
    }
    if (pid == 4) {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("  [ERROR] Cannot terminate System Process (PID 4).\n");
        printf("  [LỖI]  Không thể kết thúc tiến trình System (PID 4).\n");
        ResetColor();
        return;
    }

    // Tìm tên tiến trình / Find process name
    const char* procName = "Unknown";
    for (int i = 0; i < g_processCount; i++) {
        if (g_processes[i].pid == pid) {
            procName = g_processes[i].name;
            break;
        }
    }

    printf("\n  Attempting to terminate / Đang cố kết thúc:\n");
    printf("    PID:  %lu\n", pid);
    printf("    Name: %s\n\n", procName);

    // OpenProcess: Mở handle với quyền PROCESS_TERMINATE
    // OpenProcess: Open handle with PROCESS_TERMINATE access
    HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if (hProc == NULL) {
        DWORD err = GetLastError();
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("  [FAILED] OpenProcess failed / Không thể mở tiến trình.\n");
        printf("  Error code / Mã lỗi: %lu", err);
        if (err == ERROR_ACCESS_DENIED)
            printf(" (Access Denied / Từ chối truy cập - try Run as Admin)");
        else if (err == ERROR_INVALID_PARAMETER)
            printf(" (Invalid PID / PID không hợp lệ)");
        printf("\n");
        ResetColor();
        return;
    }

    // TerminateProcess: Kết thúc tiến trình ngay lập tức
    // TerminateProcess: Immediately terminates the process
    // Tham số 2 = exit code (mã thoát) cho tiến trình
    // Parameter 2 = exit code for the terminated process
    // CẢNH BÁO: Tiến trình sẽ bị kết thúc mà không cleanup!
    // WARNING: Process is terminated without cleanup!
    if (TerminateProcess(hProc, 1)) {
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("  [SUCCESS] Process %lu (%s) terminated.\n", pid, procName);
        printf("  [THÀNH CÔNG] Tiến trình %lu (%s) đã bị kết thúc.\n", pid, procName);
        ResetColor();
    } else {
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("  [FAILED] TerminateProcess failed / Không thể kết thúc.\n");
        printf("  Error code / Mã lỗi: %lu\n", GetLastError());
        ResetColor();
    }

    CloseHandle(hProc);
    printf("\n");
}

// ============================================================================
// Hiển thị menu chính
// Display main menu
// ============================================================================
void ShowMenu()
{
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("  ============= MINI TASK MANAGER / QUẢN LÝ TÁC VỤ MINI =============\n");
    ResetColor();
    printf("  [1] List all processes     / Liệt kê tất cả tiến trình\n");
    printf("  [2] Search by name         / Tìm kiếm theo tên\n");
    printf("  [3] Terminate by PID       / Kết thúc theo PID\n");
    printf("  [4] Refresh process list   / Làm mới danh sách\n");
    printf("  [5] Summary statistics     / Thống kê tổng quan\n");
    printf("  [0] Exit                   / Thoát\n");
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("  =====================================================================\n");
    ResetColor();
    printf("  Your choice / Lựa chọn: ");
}

// ============================================================================
// Thống kê tổng quan
// Summary statistics
// ============================================================================
void ShowStatistics()
{
    printf("\n");
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("  === Process Statistics / Thống kê tiến trình ===\n\n");
    ResetColor();

    DWORD totalThreads = 0;
    SIZE_T totalMemKB = 0;
    SIZE_T maxMemKB = 0;
    int maxMemIdx = 0;

    for (int i = 0; i < g_processCount; i++) {
        totalThreads += g_processes[i].threadCount;
        totalMemKB   += g_processes[i].memoryUsageKB;
        if (g_processes[i].memoryUsageKB > maxMemKB) {
            maxMemKB = g_processes[i].memoryUsageKB;
            maxMemIdx = i;
        }
    }

    char memBuf[32];
    printf("  Total processes / Tổng tiến trình:   %d\n", g_processCount);
    printf("  Total threads / Tổng luồng:          %lu\n", totalThreads);
    printf("  Avg threads/process / TB luồng/TT:   %.1f\n",
           g_processCount > 0 ? (double)totalThreads / g_processCount : 0.0);

    FormatKB(totalMemKB, memBuf, sizeof(memBuf));
    printf("  Total memory (tracked) / Tổng nhớ:   %s\n", memBuf);

    FormatKB(maxMemKB, memBuf, sizeof(memBuf));
    printf("  Largest process / TT lớn nhất:       %s (PID=%lu, %s)\n",
           memBuf, g_processes[maxMemIdx].pid, g_processes[maxMemIdx].name);

    printf("\n  Top 5 by memory / Top 5 theo bộ nhớ:\n\n");

    // Sắp xếp tìm top 5 (selection kiểu đơn giản)
    // Find top 5 by memory (simple selection)
    BOOL used[MAX_PROCESSES] = {FALSE};
    for (int rank = 0; rank < 5 && rank < g_processCount; rank++) {
        SIZE_T best = 0;
        int bestIdx = -1;
        for (int i = 0; i < g_processCount; i++) {
            if (!used[i] && g_processes[i].memoryUsageKB > best) {
                best = g_processes[i].memoryUsageKB;
                bestIdx = i;
            }
        }
        if (bestIdx >= 0) {
            used[bestIdx] = TRUE;
            FormatKB(g_processes[bestIdx].memoryUsageKB, memBuf, sizeof(memBuf));
            printf("    #%d: %-30s PID=%-8lu %s\n",
                   rank + 1, g_processes[bestIdx].name,
                   g_processes[bestIdx].pid, memBuf);
        }
    }
    printf("\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    InitConsole();

    printf("************************************************************\n");
    printf("*  Case Study 01: Mini Task Manager / Quản lý tác vụ mini *\n");
    printf("************************************************************\n\n");

    printf("  Loading process list... / Đang tải danh sách tiến trình...\n");
    EnumerateProcesses();
    printf("  Loaded %d processes. / Đã tải %d tiến trình.\n\n", g_processCount, g_processCount);

    char input[256];
    BOOL running = TRUE;

    while (running) {
        ShowMenu();

        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Loại bỏ newline / Strip newline
        input[strcspn(input, "\n")] = '\0';

        switch (input[0]) {
            case '1':
                DisplayProcessList();
                break;

            case '2': {
                printf("  Enter search term / Nhập từ khóa: ");
                char search[256];
                if (fgets(search, sizeof(search), stdin)) {
                    search[strcspn(search, "\n")] = '\0';
                    if (search[0]) {
                        SearchByName(search);
                    }
                }
                break;
            }

            case '3': {
                printf("  Enter PID to terminate / Nhập PID cần kết thúc: ");
                char pidStr[64];
                if (fgets(pidStr, sizeof(pidStr), stdin)) {
                    DWORD pid = (DWORD)atol(pidStr);
                    if (pid > 0) {
                        printf("  Are you sure? / Bạn chắc chắn? (y/n): ");
                        char confirm[16];
                        if (fgets(confirm, sizeof(confirm), stdin)) {
                            if (confirm[0] == 'y' || confirm[0] == 'Y') {
                                TerminateByPID(pid);
                                // Làm mới danh sách / Refresh list
                                Sleep(500);
                                EnumerateProcesses();
                            } else {
                                printf("  Cancelled. / Đã hủy.\n\n");
                            }
                        }
                    } else {
                        printf("  Invalid PID. / PID không hợp lệ.\n\n");
                    }
                }
                break;
            }

            case '4':
                printf("  Refreshing... / Đang làm mới...\n");
                EnumerateProcesses();
                printf("  Loaded %d processes. / Đã tải %d tiến trình.\n\n",
                       g_processCount, g_processCount);
                break;

            case '5':
                ShowStatistics();
                break;

            case '0':
            case 'q':
            case 'Q':
                running = FALSE;
                break;

            default:
                printf("  Invalid option. / Lựa chọn không hợp lệ.\n\n");
                break;
        }
    }

    printf("\n  Goodbye! / Tạm biệt!\n");
    return 0;
}
