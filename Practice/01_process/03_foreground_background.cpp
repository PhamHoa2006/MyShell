/*
 * ============================================================================
 * BÀI TẬP 03: TIẾN TRÌNH FOREGROUND VÀ BACKGROUND
 * ============================================================================
 * Mô tả / Description:
 *   Minh họa sự khác biệt giữa chạy tiến trình ở foreground (chờ kết thúc)
 *   và background (không chờ, tiếp tục ngay). Đây là khái niệm quan trọng
 *   trong shell (tương tự lệnh "command &" trong Linux).
 *
 *   Demonstrate the difference between running a process in foreground
 *   (wait for completion) vs background (continue immediately).
 *   This is a key concept in shell design (similar to "command &" in Linux).
 *
 * API chính / Key APIs:
 *   - CreateProcess()       : Tạo tiến trình
 *   - WaitForSingleObject() : Chờ tiến trình (foreground)
 *   - GetExitCodeProcess()  : Kiểm tra trạng thái (background)
 *   - CloseHandle()         : Đóng handle
 *
 * Biên dịch / Compile:
 *   g++ 03_foreground_background.cpp -o 03_foreground_background.exe
 *   cl 03_foreground_background.cpp /Fe:03_foreground_background.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// Hàm tạo tiến trình con (trả về TRUE nếu thành công)
// Helper to create a child process (returns TRUE on success)
BOOL CreateChildProcess(const char* command, PROCESS_INFORMATION* pi)
{
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(pi, sizeof(*pi));

    // Sao chép command vì CreateProcess cần chuỗi có thể sửa đổi
    // Copy command because CreateProcess needs a modifiable string
    char cmdLine[1024];
    strncpy(cmdLine, command, sizeof(cmdLine) - 1);
    cmdLine[sizeof(cmdLine) - 1] = '\0';

    BOOL success = CreateProcessA(
        NULL,       // Lấy tên chương trình từ cmdLine
        cmdLine,    // Dòng lệnh / Command line
        NULL, NULL, // Bảo mật mặc định / Default security
        FALSE,      // Không kế thừa handle / No handle inheritance
        0,          // Không cờ đặc biệt / No special flags
        NULL, NULL, // Kế thừa env và thư mục / Inherit env and directory
        &si, pi
    );

    if (!success) {
        printf("[LOI/ERROR] Khong tao duoc tien trinh: %s\n", command);
        printf("  Ma loi / Error code: %lu\n", GetLastError());
    }

    return success;
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 03: FOREGROUND vs BACKGROUND PROCESS\n");
    printf("  Exercise 03: Foreground vs Background Process\n");
    printf("============================================================\n\n");

    PROCESS_INFORMATION pi;

    // =================================================================
    // PHẦN 1: FOREGROUND PROCESS (Chờ tiến trình con kết thúc)
    //         FOREGROUND PROCESS (Wait for child to finish)
    // =================================================================
    // Trong shell: khi người dùng gõ "notepad" -> shell CHỜ notepad đóng
    // In a shell: when user types "notepad" -> shell WAITS for notepad to close
    // =================================================================

    printf("=== PHAN 1: FOREGROUND PROCESS ===\n");
    printf("=== PART 1: FOREGROUND PROCESS ===\n\n");

    char fgCmd[] = "cmd.exe /C echo [FOREGROUND] Tien trinh nay chay o foreground && timeout /t 3 /nobreak > nul && echo [FOREGROUND] Hoan thanh!";
    printf("Lenh / Command: cmd.exe /C echo ... && timeout 3s ...\n");
    printf("Shell se CHO tien trinh nay ket thuc truoc khi tiep tuc.\n");
    printf("Shell will WAIT for this process to finish before continuing.\n\n");

    if (CreateChildProcess(fgCmd, &pi)) {
        printf("  PID con / Child PID: %lu\n", pi.dwProcessId);
        printf("  [CHA/PARENT] Dang cho con ket thuc... / Waiting for child...\n\n");

        // === FOREGROUND: Chờ vô hạn ===
        // === FOREGROUND: Wait indefinitely ===
        DWORD result = WaitForSingleObject(pi.hProcess, INFINITE);

        if (result == WAIT_OBJECT_0) {
            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            printf("  [CHA/PARENT] Con da ket thuc! Exit code: %lu\n", exitCode);
            printf("  [CHA/PARENT] Child finished! Exit code: %lu\n", exitCode);
        }

        // Đóng handle / Close handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    printf("\n--- Shell da san sang nhan lenh moi (sau khi foreground ket thuc) ---\n");
    printf("--- Shell is ready for new commands (after foreground finished) ---\n\n");

    // =================================================================
    // PHẦN 2: BACKGROUND PROCESS (Không chờ, tiếp tục ngay)
    //         BACKGROUND PROCESS (Don't wait, continue immediately)
    // =================================================================
    // Trong shell: khi người dùng gõ "notepad &" -> shell KHÔNG CHỜ
    // In a shell: when user types "notepad &" -> shell does NOT wait
    // =================================================================

    printf("=== PHAN 2: BACKGROUND PROCESS ===\n");
    printf("=== PART 2: BACKGROUND PROCESS ===\n\n");

    char bgCmd[] = "cmd.exe /C echo [BACKGROUND] Tien trinh nay chay o background && timeout /t 5 /nobreak > nul && echo [BACKGROUND] Hoan thanh!";
    printf("Lenh / Command: cmd.exe /C echo ... && timeout 5s ...\n");
    printf("Shell se KHONG CHO, tiep tuc ngay lap tuc.\n");
    printf("Shell will NOT WAIT, continues immediately.\n\n");

    PROCESS_INFORMATION bgPi;
    if (CreateChildProcess(bgCmd, &bgPi)) {
        printf("  PID con (background) / Child PID: %lu\n", bgPi.dwProcessId);

        // === BACKGROUND: KHÔNG gọi WaitForSingleObject() ===
        // === BACKGROUND: Do NOT call WaitForSingleObject() ===
        printf("  [CHA/PARENT] Tien trinh con dang chay o background!\n");
        printf("  [CHA/PARENT] Child is running in background!\n\n");

        // Shell tiếp tục làm việc khác trong khi con chạy nền
        // Shell continues doing other work while child runs in background
        printf("  [CHA/PARENT] Shell dang lam viec khac...\n");
        printf("  [CHA/PARENT] Shell is doing other work...\n\n");

        // Mô phỏng shell làm việc khác
        // Simulate shell doing other work
        for (int i = 1; i <= 3; i++) {
            printf("  [CHA/PARENT] Dang xu ly cong viec %d / Processing task %d...\n", i, i);
            Sleep(1000);  // Chờ 1 giây / Wait 1 second
        }

        // Kiểm tra xem tiến trình nền đã kết thúc chưa (không chờ)
        // Check if background process has finished (non-blocking check)
        printf("\n  [CHA/PARENT] Kiem tra trang thai tien trinh nen...\n");
        printf("  [CHA/PARENT] Checking background process status...\n");

        DWORD exitCode;
        GetExitCodeProcess(bgPi.hProcess, &exitCode);

        if (exitCode == STILL_ACTIVE) {
            // STILL_ACTIVE (259): tiến trình vẫn đang chạy
            // STILL_ACTIVE (259): process is still running
            printf("  [CHA/PARENT] Tien trinh nen van dang chay (STILL_ACTIVE)\n");
            printf("  [CHA/PARENT] Background process is still running\n\n");

            // Tùy chọn: có thể chờ hoặc bỏ qua
            // Option: we can choose to wait or ignore
            printf("  [CHA/PARENT] Cho tien trinh nen ket thuc...\n");
            printf("  [CHA/PARENT] Now waiting for background process...\n");
            WaitForSingleObject(bgPi.hProcess, INFINITE);

            GetExitCodeProcess(bgPi.hProcess, &exitCode);
            printf("  [CHA/PARENT] Tien trinh nen da ket thuc! Exit code: %lu\n", exitCode);
        } else {
            printf("  [CHA/PARENT] Tien trinh nen da ket thuc. Exit code: %lu\n", exitCode);
        }

        CloseHandle(bgPi.hProcess);
        CloseHandle(bgPi.hThread);
    }

    // =================================================================
    // PHẦN 3: SO SÁNH TỔNG KẾT / COMPARISON SUMMARY
    // =================================================================
    printf("\n============================================================\n");
    printf("  TONG KET / SUMMARY:\n");
    printf("============================================================\n");
    printf("  FOREGROUND:                     BACKGROUND:\n");
    printf("  - Goi WaitForSingleObject()     - KHONG goi Wait\n");
    printf("  - Shell bi block cho den khi    - Shell tiep tuc ngay\n");
    printf("    tien trinh con ket thuc         lam viec khac\n");
    printf("  - Tuong tu: ./program            - Tuong tu: ./program &\n");
    printf("  - Calls WaitForSingleObject()   - Does NOT call Wait\n");
    printf("  - Shell blocks until child      - Shell continues\n");
    printf("    process finishes                immediately\n");
    printf("============================================================\n");
    printf("  Ket thuc bai tap 03. / End of Exercise 03.\n");
    printf("============================================================\n");

    return 0;
}
