/*
 * ============================================================================
 * BÀI TẬP 02: TẠO TIẾN TRÌNH (Create Process)
 * ============================================================================
 * Mô tả / Description:
 *   Tạo tiến trình con bằng CreateProcess(). Giải thích chi tiết từng
 *   tham số. Chờ tiến trình con kết thúc bằng WaitForSingleObject().
 *   Create a child process using CreateProcess(). Explain each parameter
 *   in detail. Wait for the child to finish with WaitForSingleObject().
 *
 * API chính / Key APIs:
 *   - CreateProcess()        : Tạo tiến trình mới
 *   - WaitForSingleObject()  : Chờ tiến trình kết thúc
 *   - GetExitCodeProcess()   : Lấy mã thoát của tiến trình
 *   - CloseHandle()          : Đóng handle (rất quan trọng!)
 *
 * Biên dịch / Compile:
 *   g++ 02_create_process.cpp -o 02_create_process.exe
 *   cl 02_create_process.cpp /Fe:02_create_process.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// Hàm tiện ích: in thông báo lỗi Windows
// Utility: print Windows error message
void PrintError(const char* functionName)
{
    DWORD err = GetLastError();
    char* msgBuf = NULL;

    // FormatMessage chuyển mã lỗi thành chuỗi mô tả
    // FormatMessage converts error code to descriptive string
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err, 0, (LPSTR)&msgBuf, 0, NULL
    );

    printf("[LOI/ERROR] %s that bai! Ma loi / Error code: %lu\n", functionName, err);
    if (msgBuf) {
        printf("  Mo ta / Description: %s", msgBuf);
        LocalFree(msgBuf);
    }
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 02: TAO TIEN TRINH VOI CreateProcess()\n");
    printf("  Exercise 02: Create Process with CreateProcess()\n");
    printf("============================================================\n\n");

    // ---------------------------------------------------------------
    // GIẢI THÍCH CreateProcess() / CreateProcess() EXPLAINED
    // ---------------------------------------------------------------
    // Nguyên mẫu / Prototype:
    //
    // BOOL CreateProcess(
    //   LPCSTR lpApplicationName,    // [1] Tên ứng dụng (có thể NULL)
    //   LPSTR  lpCommandLine,        // [2] Dòng lệnh (có thể NULL, nhưng ít nhất 1 cái phải có)
    //   LPSECURITY_ATTRIBUTES lpProcessAttributes,  // [3] Bảo mật tiến trình
    //   LPSECURITY_ATTRIBUTES lpThreadAttributes,    // [4] Bảo mật thread
    //   BOOL   bInheritHandles,      // [5] Kế thừa handle từ cha?
    //   DWORD  dwCreationFlags,      // [6] Cờ tạo tiến trình
    //   LPVOID lpEnvironment,        // [7] Biến môi trường
    //   LPCSTR lpCurrentDirectory,   // [8] Thư mục làm việc
    //   LPSTARTUPINFOA lpStartupInfo,       // [9] Thông tin khởi động
    //   LPPROCESS_INFORMATION lpProcessInformation  // [10] Thông tin tiến trình con
    // );
    // ---------------------------------------------------------------

    // === PHẦN A: Tạo tiến trình cmd.exe và chờ ===
    // === PART A: Create cmd.exe process and wait ===
    printf("--- PHAN A: Tao tien trinh cmd.exe ---\n");
    printf("--- PART A: Creating cmd.exe process ---\n\n");

    // [9] STARTUPINFO: Cấu hình cửa sổ, I/O của tiến trình con
    //     STARTUPINFO: Configure window and I/O for child process
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));    // Xóa sạch bộ nhớ / Clear memory
    si.cb = sizeof(si);             // PHẢI set kích thước / MUST set size
    // Các trường quan trọng khác (không dùng ở đây):
    // Other important fields (not used here):
    //   si.dwFlags     = STARTF_USESHOWWINDOW;   // Dùng wShowWindow
    //   si.wShowWindow = SW_HIDE;                 // Ẩn cửa sổ / Hide window
    //   si.hStdInput   = handle;                  // Redirect stdin
    //   si.hStdOutput  = handle;                  // Redirect stdout
    //   si.hStdError   = handle;                  // Redirect stderr

    // [10] PROCESS_INFORMATION: Nhận thông tin về tiến trình con sau khi tạo
    //      PROCESS_INFORMATION: Receives child process info after creation
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Chuẩn bị command line
    // Prepare the command line
    // LƯU Ý: lpCommandLine PHẢI là chuỗi có thể sửa đổi (non-const)
    // NOTE: lpCommandLine MUST be a modifiable string (non-const)
    char cmdLine[] = "cmd.exe /C echo Hello from child process! && echo PID cua toi la... && timeout /t 2 /nobreak";

    printf("Dang tao tien trinh / Creating process: %s\n\n", cmdLine);

    // Gọi CreateProcess với giải thích từng tham số
    // Call CreateProcess with each parameter explained
    BOOL success = CreateProcessA(
        NULL,           // [1] lpApplicationName: NULL -> lấy từ lpCommandLine
                        //     NULL means the executable name is taken from cmdLine

        cmdLine,        // [2] lpCommandLine: Dòng lệnh đầy đủ
                        //     Full command line to execute

        NULL,           // [3] lpProcessAttributes: NULL -> bảo mật mặc định cho tiến trình
                        //     NULL means default security for the process object

        NULL,           // [4] lpThreadAttributes: NULL -> bảo mật mặc định cho thread
                        //     NULL means default security for the primary thread

        FALSE,          // [5] bInheritHandles: FALSE -> con KHÔNG kế thừa handle của cha
                        //     FALSE means child does NOT inherit parent's handles

        0,              // [6] dwCreationFlags: 0 -> không có cờ đặc biệt
                        //     0 means no special creation flags
                        //     Các giá trị hữu ích / Useful values:
                        //       CREATE_NEW_CONSOLE     - tạo console riêng
                        //       CREATE_SUSPENDED       - tạo nhưng chưa chạy
                        //       CREATE_NO_WINDOW       - không tạo cửa sổ
                        //       DETACHED_PROCESS       - tách khỏi console cha

        NULL,           // [7] lpEnvironment: NULL -> kế thừa biến môi trường của cha
                        //     NULL means inherit parent's environment variables

        NULL,           // [8] lpCurrentDirectory: NULL -> dùng thư mục hiện tại của cha
                        //     NULL means use parent's current directory

        &si,            // [9] lpStartupInfo: Con trỏ tới STARTUPINFO
                        //     Pointer to STARTUPINFO structure

        &pi             // [10] lpProcessInformation: Nhận thông tin tiến trình con
                        //      Receives the child process information
    );

    if (!success) {
        PrintError("CreateProcessA");
        return 1;
    }

    // Sau khi CreateProcess thành công, pi chứa:
    // After CreateProcess succeeds, pi contains:
    //   pi.hProcess    - Handle của tiến trình con / Child process handle
    //   pi.hThread     - Handle của thread chính của con / Child's primary thread handle
    //   pi.dwProcessId - PID của tiến trình con / Child's process ID
    //   pi.dwThreadId  - TID của thread chính / Primary thread ID

    printf("=== Tao tien trinh thanh cong! / Process created successfully! ===\n");
    printf("  PID cua cha  / Parent PID : %lu\n", GetCurrentProcessId());
    printf("  PID cua con  / Child PID  : %lu\n", pi.dwProcessId);
    printf("  TID cua con  / Child TID  : %lu\n", pi.dwThreadId);
    printf("  Process Handle            : %p\n", pi.hProcess);
    printf("  Thread Handle             : %p\n\n", pi.hThread);

    // ---------------------------------------------------------------
    // Chờ tiến trình con kết thúc bằng WaitForSingleObject()
    // Wait for child process to finish using WaitForSingleObject()
    // ---------------------------------------------------------------
    // WaitForSingleObject(handle, timeout_ms)
    //   - INFINITE: chờ vô hạn / wait forever
    //   - 0       : kiểm tra ngay (non-blocking)
    //   - N       : chờ tối đa N ms
    //
    // Giá trị trả về / Return values:
    //   WAIT_OBJECT_0  (0) : Đối tượng được báo hiệu (tiến trình kết thúc)
    //   WAIT_TIMEOUT (258) : Hết thời gian chờ
    //   WAIT_FAILED  (-1)  : Lỗi
    // ---------------------------------------------------------------

    printf("Dang cho tien trinh con ket thuc...\n");
    printf("Waiting for child process to finish...\n\n");

    DWORD waitResult = WaitForSingleObject(pi.hProcess, INFINITE);

    switch (waitResult) {
        case WAIT_OBJECT_0:
            printf("Tien trinh con da ket thuc! / Child process has finished!\n");
            break;
        case WAIT_TIMEOUT:
            printf("Het thoi gian cho! / Wait timed out!\n");
            break;
        case WAIT_FAILED:
            PrintError("WaitForSingleObject");
            break;
        default:
            printf("Ket qua khong mong doi: %lu\n", waitResult);
            break;
    }

    // ---------------------------------------------------------------
    // Lấy mã thoát (exit code) của tiến trình con
    // Get the exit code of the child process
    // ---------------------------------------------------------------
    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        printf("Ma thoat cua tien trinh con / Child exit code: %lu\n", exitCode);
        // exitCode == 0        : thành công / success
        // exitCode == STILL_ACTIVE (259) : vẫn đang chạy / still running
        // exitCode != 0        : lỗi hoặc mã tùy chỉnh / error or custom code
    } else {
        PrintError("GetExitCodeProcess");
    }

    // ---------------------------------------------------------------
    // QUAN TRỌNG: Đóng handle khi không dùng nữa
    // IMPORTANT: Close handles when no longer needed
    // ---------------------------------------------------------------
    // Nếu không đóng handle sẽ gây RÒ RỈ HANDLE (handle leak)
    // Not closing handles causes HANDLE LEAKS
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("\nDa dong handle cua tien trinh con. / Child handles closed.\n");

    printf("\n============================================================\n");
    printf("  Ket thuc bai tap 02. / End of Exercise 02.\n");
    printf("============================================================\n");

    return 0;
}
