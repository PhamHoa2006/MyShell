/*
 * ============================================================================
 * BÀI TẬP 04: KẾT THÚC TIẾN TRÌNH (Terminate Process)
 * ============================================================================
 * Mô tả / Description:
 *   Tạo một tiến trình con, sau đó kết thúc bắt buộc bằng TerminateProcess().
 *   Lấy mã thoát bằng GetExitCodeProcess(). Minh họa cách "kill" tiến trình
 *   (tương tự lệnh "kill" trong shell).
 *
 *   Create a child process, then forcefully terminate it using
 *   TerminateProcess(). Retrieve exit code with GetExitCodeProcess().
 *   Demonstrates how to "kill" a process (similar to shell "kill" command).
 *
 * API chính / Key APIs:
 *   - CreateProcess()       : Tạo tiến trình con
 *   - TerminateProcess()    : Kết thúc bắt buộc tiến trình
 *   - GetExitCodeProcess()  : Lấy mã thoát
 *   - WaitForSingleObject() : Chờ tiến trình thực sự kết thúc sau khi terminate
 *
 * LƯU Ý QUAN TRỌNG / IMPORTANT NOTE:
 *   TerminateProcess() là cách KẾT THÚC BẮT BUỘC (không clean up).
 *   Tiến trình bị terminate sẽ KHÔNG chạy các hàm cleanup, destructor,
 *   hoặc finally block. Chỉ dùng khi thực sự cần thiết!
 *
 *   TerminateProcess() is a FORCEFUL termination (no cleanup).
 *   The terminated process will NOT run cleanup handlers, destructors,
 *   or finally blocks. Use only when absolutely necessary!
 *
 * Biên dịch / Compile:
 *   g++ 04_terminate_process.cpp -o 04_terminate_process.exe
 *   cl 04_terminate_process.cpp /Fe:04_terminate_process.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

void PrintError(const char* func)
{
    DWORD err = GetLastError();
    printf("[LOI/ERROR] %s that bai! Ma loi: %lu\n", func, err);
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 04: KET THUC TIEN TRINH (TerminateProcess)\n");
    printf("  Exercise 04: Terminate Process\n");
    printf("============================================================\n\n");

    // =================================================================
    // PHẦN 1: Tạo tiến trình sẽ chạy lâu (để có thời gian terminate)
    //         Create a long-running process (so we have time to terminate)
    // =================================================================

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Tạo tiến trình ping (chạy lâu) để minh họa
    // Create a ping process (runs for a while) to demonstrate
    char cmdLine[] = "cmd.exe /C ping -n 30 127.0.0.1";

    printf("[BUOC 1] Tao tien trinh con chay lau...\n");
    printf("[STEP 1] Creating a long-running child process...\n");
    printf("  Lenh / Command: %s\n\n", cmdLine);

    BOOL success = CreateProcessA(
        NULL, cmdLine, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE,  // Tạo console riêng để dễ quan sát
                             // Create separate console for observation
        NULL, NULL, &si, &pi
    );

    if (!success) {
        PrintError("CreateProcessA");
        return 1;
    }

    printf("  Tao thanh cong! / Created successfully!\n");
    printf("  PID cua con / Child PID: %lu\n\n", pi.dwProcessId);

    // =================================================================
    // PHẦN 2: Kiểm tra trạng thái trước khi terminate
    //         Check status before terminating
    // =================================================================

    printf("[BUOC 2] Cho 3 giay de tien trinh con chay...\n");
    printf("[STEP 2] Waiting 3 seconds to let child run...\n\n");
    Sleep(3000);

    // Kiểm tra xem tiến trình còn chạy không
    // Check if the process is still running
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    printf("[BUOC 3] Kiem tra trang thai / Checking status:\n");
    if (exitCode == STILL_ACTIVE) {
        // STILL_ACTIVE = 259: tiến trình vẫn đang chạy
        // STILL_ACTIVE = 259: process is still running
        printf("  Trang thai: STILL_ACTIVE (259) - Van dang chay\n");
        printf("  Status: STILL_ACTIVE (259) - Still running\n\n");
    } else {
        printf("  Trang thai: Da ket thuc voi ma thoat %lu\n", exitCode);
        printf("  Status: Already exited with code %lu\n\n", exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 0;
    }

    // =================================================================
    // PHẦN 3: KẾT THÚC BẮT BUỘC bằng TerminateProcess()
    //         FORCEFULLY TERMINATE using TerminateProcess()
    // =================================================================

    printf("[BUOC 4] Goi TerminateProcess()...\n");
    printf("[STEP 4] Calling TerminateProcess()...\n\n");

    // TerminateProcess(hProcess, uExitCode)
    //   hProcess  : Handle của tiến trình cần kết thúc
    //               Handle of the process to terminate
    //   uExitCode : Mã thoát mà ta muốn gán cho tiến trình
    //               Exit code we want to assign to the process
    //
    // ĐẶC ĐIỂM / CHARACTERISTICS:
    //   - Kết thúc NGAY LẬP TỨC, không chờ cleanup
    //     Terminates IMMEDIATELY, no cleanup
    //   - Tất cả thread trong tiến trình đều bị dừng
    //     All threads in the process are stopped
    //   - DLL sẽ KHÔNG được gọi DllMain(DLL_PROCESS_DETACH)
    //     DLLs will NOT get DllMain(DLL_PROCESS_DETACH) called
    //   - Dữ liệu chưa flush sẽ MẤT
    //     Unflushed data will be LOST

    UINT customExitCode = 42;  // Mã thoát tùy chỉnh / Custom exit code

    BOOL terminated = TerminateProcess(pi.hProcess, customExitCode);

    if (terminated) {
        printf("  TerminateProcess() thanh cong!\n");
        printf("  TerminateProcess() succeeded!\n");
        printf("  Ma thoat duoc gan / Assigned exit code: %u\n\n", customExitCode);
    } else {
        PrintError("TerminateProcess");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    // =================================================================
    // PHẦN 4: Chờ tiến trình thực sự kết thúc
    //         Wait for process to actually terminate
    // =================================================================
    // LƯU Ý: TerminateProcess() là bất đồng bộ (asynchronous)!
    // Tiến trình có thể chưa hoàn toàn kết thúc ngay sau khi gọi.
    // Phải WaitForSingleObject() để đảm bảo.
    //
    // NOTE: TerminateProcess() is asynchronous!
    // The process may not be fully terminated immediately after the call.
    // Must WaitForSingleObject() to be sure.

    printf("[BUOC 5] Cho tien trinh thuc su ket thuc...\n");
    printf("[STEP 5] Waiting for process to actually terminate...\n");

    DWORD waitResult = WaitForSingleObject(pi.hProcess, 5000);  // Chờ tối đa 5s

    if (waitResult == WAIT_OBJECT_0) {
        printf("  Tien trinh da ket thuc hoan toan.\n");
        printf("  Process has fully terminated.\n\n");
    } else if (waitResult == WAIT_TIMEOUT) {
        printf("  Canh bao: Tien trinh chua ket thuc sau 5 giay!\n");
        printf("  Warning: Process not terminated after 5 seconds!\n\n");
    }

    // =================================================================
    // PHẦN 5: Xác nhận mã thoát
    //         Verify exit code
    // =================================================================

    printf("[BUOC 6] Kiem tra ma thoat sau khi terminate:\n");
    printf("[STEP 6] Check exit code after termination:\n");

    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("  Ma thoat / Exit code: %lu\n", exitCode);

    if (exitCode == customExitCode) {
        printf("  -> Dung nhu mong doi! Ma thoat = %u (do ta gan khi TerminateProcess)\n",
               customExitCode);
        printf("  -> As expected! Exit code = %u (set by our TerminateProcess call)\n",
               customExitCode);
    }

    // =================================================================
    // PHẦN 6: Đóng handle
    //         Close handles
    // =================================================================

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // =================================================================
    // PHẦN 7: So sánh các cách kết thúc tiến trình
    //         Compare process termination methods
    // =================================================================

    printf("\n============================================================\n");
    printf("  SO SANH CAC CACH KET THUC TIEN TRINH:\n");
    printf("  COMPARISON OF PROCESS TERMINATION METHODS:\n");
    printf("============================================================\n");
    printf("  1. ExitProcess(code)   - Tu ket thuc (goi tu TRONG tien trinh)\n");
    printf("     ExitProcess(code)   - Self-terminate (called FROM WITHIN process)\n");
    printf("     -> Co cleanup DLL, nhung KHONG cleanup C runtime\n\n");
    printf("  2. TerminateProcess(h) - Ket thuc BAT BUOC (goi tu BEN NGOAI)\n");
    printf("     TerminateProcess(h) - FORCEFUL terminate (called from OUTSIDE)\n");
    printf("     -> KHONG co bat ky cleanup nao!\n\n");
    printf("  3. return tu main()    - Ket thuc binh thuong (tot nhat)\n");
    printf("     return from main()  - Normal termination (best practice)\n");
    printf("     -> Day du cleanup: destructors, atexit, DLL\n");
    printf("============================================================\n");
    printf("  Ket thuc bai tap 04. / End of Exercise 04.\n");
    printf("============================================================\n");

    return 0;
}
