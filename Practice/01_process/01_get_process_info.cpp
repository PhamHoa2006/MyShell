/*
 * ============================================================================
 * BÀI TẬP 01: LẤY THÔNG TIN TIẾN TRÌNH (Get Process Information)
 * ============================================================================
 * Mô tả / Description:
 *   Lấy thông tin cơ bản của tiến trình hiện tại: PID, PPID, số handle,
 *   thời gian chạy, bộ nhớ sử dụng, v.v.
 *   Retrieve basic information about the current process: PID, PPID,
 *   handle count, running time, memory usage, etc.
 *
 * API chính / Key APIs:
 *   - GetCurrentProcessId()   : Lấy PID của tiến trình hiện tại
 *   - GetCurrentProcess()     : Lấy pseudo-handle của tiến trình hiện tại
 *   - OpenProcess()           : Mở handle tới một tiến trình khác
 *   - GetProcessTimes()       : Lấy thời gian CPU của tiến trình
 *   - GetProcessMemoryInfo()  : Lấy thông tin bộ nhớ (Working Set, Page Faults)
 *   - CreateToolhelp32Snapshot: Dùng để tìm PPID (Parent Process ID)
 *
 * Biên dịch / Compile:
 *   g++ 01_get_process_info.cpp -o 01_get_process_info.exe
 *   cl 01_get_process_info.cpp /Fe:01_get_process_info.exe
 * ============================================================================
 */

#include <windows.h>
#include <tlhelp32.h>   // Để dùng CreateToolhelp32Snapshot / For snapshot APIs
#include <psapi.h>      // Để dùng GetProcessMemoryInfo / For memory info
#include <stdio.h>

// Hàm tìm PPID (Process ID của tiến trình cha)
// Function to find the Parent Process ID (PPID)
DWORD GetParentProcessId(DWORD pid)
{
    DWORD ppid = 0;

    // Chụp "ảnh" danh sách tiến trình hiện tại
    // Take a snapshot of all running processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[LOI/ERROR] CreateToolhelp32Snapshot that bai! Ma loi: %lu\n",
               GetLastError());
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);  // BAT BUOC phai set truoc khi goi
                                         // MUST set before calling Process32First

    // Lấy tiến trình đầu tiên trong snapshot
    // Get the first process in the snapshot
    if (Process32First(hSnapshot, &pe)) {
        do {
            // Tìm entry có PID trùng với PID của ta
            // Find the entry matching our PID
            if (pe.th32ProcessID == pid) {
                ppid = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
        // Lặp qua các tiến trình tiếp theo
        // Iterate through the remaining processes
    }

    CloseHandle(hSnapshot);  // Đóng handle snapshot / Close snapshot handle
    return ppid;
}

// Hàm chuyển FILETIME sang giây (dạng double)
// Convert FILETIME to seconds (as double)
double FileTimeToSeconds(FILETIME ft)
{
    // FILETIME lưu thời gian dạng 100-nanosecond intervals
    // FILETIME stores time as 100-nanosecond intervals
    ULARGE_INTEGER uli;
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (double)uli.QuadPart / 10000000.0;  // 10^7 để ra giây / to get seconds
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 01: LAY THONG TIN TIEN TRINH\n");
    printf("  Exercise 01: Get Process Information\n");
    printf("============================================================\n\n");

    // ---------------------------------------------------------------
    // 1. Lấy PID của tiến trình hiện tại
    //    Get the current process ID
    // ---------------------------------------------------------------
    // GetCurrentProcessId() trả về DWORD (số nguyên không dấu 32-bit)
    // GetCurrentProcessId() returns a DWORD (unsigned 32-bit integer)
    DWORD currentPID = GetCurrentProcessId();
    printf("[1] PID cua tien trinh hien tai / Current Process ID: %lu\n", currentPID);

    // ---------------------------------------------------------------
    // 2. Lấy pseudo-handle của tiến trình hiện tại
    //    Get the pseudo-handle of the current process
    // ---------------------------------------------------------------
    // GetCurrentProcess() trả về pseudo-handle (giá trị -1)
    // Pseudo-handle KHÔNG cần CloseHandle()
    // GetCurrentProcess() returns a pseudo-handle (value -1)
    // Pseudo-handle does NOT need CloseHandle()
    HANDLE hProcess = GetCurrentProcess();
    printf("[2] Pseudo-handle cua tien trinh / Process pseudo-handle: %p\n", hProcess);

    // ---------------------------------------------------------------
    // 3. Tìm PPID (Parent Process ID - PID của tiến trình cha)
    //    Find the Parent Process ID
    // ---------------------------------------------------------------
    DWORD parentPID = GetParentProcessId(currentPID);
    if (parentPID != 0) {
        printf("[3] PID cua tien trinh cha / Parent Process ID (PPID): %lu\n", parentPID);
    } else {
        printf("[3] Khong tim duoc PPID / Could not find PPID\n");
    }

    // ---------------------------------------------------------------
    // 4. Lấy thời gian CPU của tiến trình
    //    Get process CPU times
    // ---------------------------------------------------------------
    FILETIME ftCreation, ftExit, ftKernel, ftUser;

    // GetProcessTimes() trả về 4 giá trị FILETIME:
    //   - Creation time: thời điểm tạo tiến trình
    //   - Exit time    : thời điểm kết thúc (0 nếu đang chạy)
    //   - Kernel time  : thời gian CPU ở kernel mode
    //   - User time    : thời gian CPU ở user mode
    if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
        printf("[4] Thoi gian CPU / CPU Times:\n");
        printf("    - Kernel mode: %.6f giay/seconds\n", FileTimeToSeconds(ftKernel));
        printf("    - User mode  : %.6f giay/seconds\n", FileTimeToSeconds(ftUser));

        // Chuyển Creation time sang SYSTEMTIME để đọc được
        // Convert Creation time to SYSTEMTIME for readability
        SYSTEMTIME stCreation;
        FILETIME ftLocal;
        FileTimeToLocalFileTime(&ftCreation, &ftLocal);  // UTC -> Local
        FileTimeToSystemTime(&ftLocal, &stCreation);
        printf("    - Thoi diem tao / Created at: %02d:%02d:%02d.%03d\n",
               stCreation.wHour, stCreation.wMinute,
               stCreation.wSecond, stCreation.wMilliseconds);
    } else {
        printf("[4] GetProcessTimes that bai! Ma loi: %lu\n", GetLastError());
    }

    // ---------------------------------------------------------------
    // 5. Lấy thông tin bộ nhớ của tiến trình
    //    Get process memory information
    // ---------------------------------------------------------------
    // PROCESS_MEMORY_COUNTERS chứa thông tin về bộ nhớ
    // PROCESS_MEMORY_COUNTERS contains memory usage data
    PROCESS_MEMORY_COUNTERS pmc;
    pmc.cb = sizeof(pmc);  // Phải set kích thước / Must set the size

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        printf("[5] Thong tin bo nho / Memory Info:\n");
        printf("    - Working Set Size      : %zu KB\n", pmc.WorkingSetSize / 1024);
        printf("    - Peak Working Set Size  : %zu KB\n", pmc.PeakWorkingSetSize / 1024);
        printf("    - Page Fault Count       : %lu\n", pmc.PageFaultCount);
        printf("    - Pagefile Usage         : %zu KB\n", pmc.PagefileUsage / 1024);
    } else {
        printf("[5] GetProcessMemoryInfo that bai! Ma loi: %lu\n", GetLastError());
    }

    // ---------------------------------------------------------------
    // 6. Lấy thông tin tiến trình cha (nếu còn chạy)
    //    Get parent process info (if still running)
    // ---------------------------------------------------------------
    if (parentPID != 0) {
        // Mở handle tới tiến trình cha với quyền đọc thông tin
        // Open a handle to the parent process with query rights
        HANDLE hParent = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,  // Quyền truy cập / Access rights
            FALSE,       // Không kế thừa handle / Do not inherit handle
            parentPID    // PID của tiến trình cha / Parent's PID
        );

        if (hParent != NULL) {
            // Lấy tên file thực thi của tiến trình cha
            // Get the executable name of the parent process
            char parentExe[MAX_PATH] = {0};
            // GetModuleFileNameEx lấy đường dẫn đầy đủ của tiến trình
            // GetModuleFileNameEx gets the full path of the process executable
            if (GetModuleFileNameExA(hParent, NULL, parentExe, MAX_PATH)) {
                printf("[6] Tien trinh cha / Parent Process:\n");
                printf("    - PID : %lu\n", parentPID);
                printf("    - Path: %s\n", parentExe);
            } else {
                printf("[6] Tien trinh cha PID=%lu (khong doc duoc ten)\n", parentPID);
                printf("    Cannot read parent name, error: %lu\n", GetLastError());
            }
            CloseHandle(hParent);  // QUAN TRONG: dong handle / IMPORTANT: close handle
        } else {
            printf("[6] Khong mo duoc tien trinh cha (PID=%lu), Ma loi: %lu\n",
                   parentPID, GetLastError());
            printf("    Could not open parent process. May need elevated privileges.\n");
        }
    }

    // ---------------------------------------------------------------
    // 7. Đếm số handle đang mở trong tiến trình hiện tại
    //    Count open handles in the current process
    // ---------------------------------------------------------------
    DWORD handleCount = 0;
    if (GetProcessHandleCount(hProcess, &handleCount)) {
        printf("[7] So handle dang mo / Open handle count: %lu\n", handleCount);
    }

    printf("\n============================================================\n");
    printf("  Ket thuc bai tap 01. / End of Exercise 01.\n");
    printf("============================================================\n");

    return 0;
}
