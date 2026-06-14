/*
 * ============================================================================
 * BÀI TẬP 05: LIỆT KÊ CÁC TIẾN TRÌNH ĐANG CHẠY (Enumerate Processes)
 * ============================================================================
 * Mô tả / Description:
 *   Liệt kê tất cả tiến trình đang chạy trên hệ thống bằng Toolhelp32
 *   Snapshot API. Hiển thị PID, PPID, tên tiến trình, số thread.
 *   Tương tự lệnh "tasklist" hoặc "ps" trong shell.
 *
 *   List all running processes using Toolhelp32 Snapshot API.
 *   Display PID, PPID, process name, thread count.
 *   Similar to "tasklist" or "ps" commands in a shell.
 *
 * API chính / Key APIs:
 *   - CreateToolhelp32Snapshot() : Chụp "ảnh" danh sách tiến trình
 *   - Process32First()          : Lấy tiến trình đầu tiên
 *   - Process32Next()           : Lấy tiến trình tiếp theo
 *   - CloseHandle()             : Đóng handle snapshot
 *
 * Biên dịch / Compile:
 *   g++ 05_enumerate_processes.cpp -o 05_enumerate_processes.exe
 *   cl 05_enumerate_processes.cpp /Fe:05_enumerate_processes.exe
 * ============================================================================
 */

#include <windows.h>
#include <tlhelp32.h>   // Thu vien Toolhelp32 / Toolhelp32 library
#include <stdio.h>

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 05: LIET KE TIEN TRINH DANG CHAY\n");
    printf("  Exercise 05: Enumerate Running Processes\n");
    printf("============================================================\n\n");

    // =================================================================
    // BƯỚC 1: Tạo Toolhelp32 Snapshot
    //         Create Toolhelp32 Snapshot
    // =================================================================
    // CreateToolhelp32Snapshot() chụp "ảnh" trạng thái hệ thống
    // tại thời điểm gọi. Snapshot bao gồm danh sách tiến trình,
    // thread, module, và heap.
    //
    // CreateToolhelp32Snapshot() captures a "snapshot" of the system
    // state at the time of the call. The snapshot includes processes,
    // threads, modules, and heaps.
    //
    // Tham số / Parameters:
    //   dwFlags: loại thông tin cần chụp / type of info to capture
    //     TH32CS_SNAPPROCESS  - tất cả tiến trình / all processes
    //     TH32CS_SNAPTHREAD   - tất cả thread / all threads
    //     TH32CS_SNAPMODULE   - module của 1 tiến trình / modules of a process
    //     TH32CS_SNAPHEAPLIST - heap của 1 tiến trình / heaps of a process
    //     TH32CS_SNAPALL      - tất cả / all of the above
    //   th32ProcessID: 0 cho toàn hệ thống / 0 for entire system

    printf("[BUOC 1] Dang chup snapshot cac tien trinh...\n");
    printf("[STEP 1] Taking snapshot of all processes...\n\n");

    HANDLE hSnapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,  // Chỉ chụp danh sách tiến trình
                             // Only capture process list
        0                    // 0 = toàn hệ thống / 0 = entire system
    );

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[LOI/ERROR] CreateToolhelp32Snapshot that bai! Ma loi: %lu\n",
               GetLastError());
        return 1;
    }

    printf("[BUOC 2] Snapshot thanh cong! Dang liet ke...\n");
    printf("[STEP 2] Snapshot taken! Enumerating...\n\n");

    // =================================================================
    // BƯỚC 2: Duyệt qua danh sách tiến trình
    //         Iterate through the process list
    // =================================================================

    // PROCESSENTRY32: Cấu trúc chứa thông tin 1 tiến trình
    // PROCESSENTRY32: Structure containing info about one process
    //
    // Các trường quan trọng / Important fields:
    //   dwSize           - PHẢI set trước khi gọi / MUST set before calling
    //   th32ProcessID    - PID
    //   th32ParentProcessID - PPID (PID cha)
    //   cntThreads       - Số thread / Thread count
    //   pcPriClassBase   - Độ ưu tiên cơ sở / Base priority
    //   szExeFile        - Tên file thực thi / Executable filename
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);  // BAT BUOC! / MANDATORY!
    // Nếu quên set dwSize, Process32First() sẽ thất bại
    // If you forget to set dwSize, Process32First() will fail

    // In header bảng / Print table header
    printf("%-8s %-8s %-8s %-6s  %s\n",
           "PID", "PPID", "Threads", "Pri", "Ten tien trinh / Process Name");
    printf("-------- -------- -------- ------  ");
    printf("----------------------------------------\n");

    int processCount = 0;

    // Process32First: lấy tiến trình đầu tiên từ snapshot
    // Process32First: get the first process from the snapshot
    BOOL found = Process32First(hSnapshot, &pe);

    if (!found) {
        printf("[LOI/ERROR] Process32First that bai! Ma loi: %lu\n", GetLastError());
        CloseHandle(hSnapshot);
        return 1;
    }

    // Lặp qua tất cả tiến trình trong snapshot
    // Loop through all processes in the snapshot
    do {
        printf("%-8lu %-8lu %-8lu %-6ld  %s\n",
               pe.th32ProcessID,        // PID
               pe.th32ParentProcessID,  // PPID
               pe.cntThreads,           // Số thread / Thread count
               pe.pcPriClassBase,       // Độ ưu tiên / Priority
               pe.szExeFile             // Tên file / Filename
        );
        processCount++;

    } while (Process32Next(hSnapshot, &pe));
    // Process32Next: lấy tiến trình tiếp theo
    // Process32Next: get the next process
    // Trả về FALSE khi hết danh sách / Returns FALSE when list is exhausted

    printf("\n-------- -------- -------- ------  ");
    printf("----------------------------------------\n");
    printf("Tong cong / Total: %d tien trinh / processes\n\n", processCount);

    // =================================================================
    // BƯỚC 3: Đóng handle snapshot
    //         Close the snapshot handle
    // =================================================================
    // QUAN TRỌNG: Luôn đóng handle khi xong
    // IMPORTANT: Always close handle when done
    CloseHandle(hSnapshot);

    // =================================================================
    // PHẦN THƯỞNG: Tìm tiến trình theo tên
    //              BONUS: Find a process by name
    // =================================================================

    printf("=== TIM TIEN TRINH THEO TEN / FIND PROCESS BY NAME ===\n\n");

    const char* targetName = "explorer.exe";
    printf("Dang tim / Searching for: %s\n\n", targetName);

    // Tạo snapshot mới (snapshot cũ đã đóng)
    // Create new snapshot (old one was closed)
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[LOI/ERROR] Khong tao duoc snapshot!\n");
        return 1;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);  // PHẢI set lại / MUST set again
    int foundCount = 0;

    if (Process32First(hSnapshot, &pe)) {
        do {
            // So sánh tên tiến trình (không phân biệt hoa thường)
            // Compare process name (case-insensitive)
            if (_stricmp(pe.szExeFile, targetName) == 0) {
                printf("  Tim thay / Found: PID=%lu, PPID=%lu, Threads=%lu\n",
                       pe.th32ProcessID,
                       pe.th32ParentProcessID,
                       pe.cntThreads);
                foundCount++;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    if (foundCount == 0) {
        printf("  Khong tim thay tien trinh '%s'\n", targetName);
        printf("  Process '%s' not found\n", targetName);
    } else {
        printf("\n  Tong cong tim thay / Total found: %d instance(s)\n", foundCount);
    }

    CloseHandle(hSnapshot);

    printf("\n============================================================\n");
    printf("  GHI CHU / NOTE:\n");
    printf("  - Snapshot la \"anh chup\" tai 1 thoi diem, KHONG cap nhat.\n");
    printf("  - Snapshot is a point-in-time capture, NOT live-updating.\n");
    printf("  - De cap nhat, phai tao snapshot moi.\n");
    printf("  - To update, create a new snapshot.\n");
    printf("============================================================\n");
    printf("  Ket thuc bai tap 05. / End of Exercise 05.\n");
    printf("============================================================\n");

    return 0;
}
