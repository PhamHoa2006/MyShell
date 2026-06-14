/*
 * ============================================================================
 * BÀI TẬP 06: TẠM DỪNG VÀ TIẾP TỤC TIẾN TRÌNH (Suspend & Resume)
 * ============================================================================
 * Mô tả / Description:
 *   Tạo tiến trình ở trạng thái tạm dừng (CREATE_SUSPENDED), sau đó
 *   tiếp tục chạy bằng ResumeThread(). Minh họa vòng đời tiến trình:
 *     Created (Suspended) -> Running -> Finished
 *
 *   Create a process in suspended state (CREATE_SUSPENDED flag), then
 *   resume it with ResumeThread(). Demonstrate the process lifecycle:
 *     Created (Suspended) -> Running -> Finished
 *
 * API chính / Key APIs:
 *   - CreateProcess() với CREATE_SUSPENDED : Tạo tiến trình tạm dừng
 *   - ResumeThread()                       : Tiếp tục thread đã tạm dừng
 *   - SuspendThread()                      : Tạm dừng thread đang chạy
 *   - GetExitCodeProcess()                 : Kiểm tra trạng thái
 *   - WaitForSingleObject()                : Chờ kết thúc
 *
 * Biên dịch / Compile:
 *   g++ 06_suspend_resume.cpp -o 06_suspend_resume.exe
 *   cl 06_suspend_resume.cpp /Fe:06_suspend_resume.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// Hàm kiểm tra và in trạng thái tiến trình
// Function to check and print process status
void PrintProcessStatus(HANDLE hProcess, DWORD pid)
{
    DWORD exitCode = 0;
    GetExitCodeProcess(hProcess, &exitCode);

    if (exitCode == STILL_ACTIVE) {
        printf("  PID %lu: DANG CHAY (STILL_ACTIVE) / RUNNING\n", pid);
    } else {
        printf("  PID %lu: DA KET THUC / FINISHED (exit code: %lu)\n", pid, exitCode);
    }
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 06: TAM DUNG VA TIEP TUC TIEN TRINH\n");
    printf("  Exercise 06: Suspend and Resume Process\n");
    printf("============================================================\n\n");

    // =================================================================
    // PHẦN 1: Tạo tiến trình ở trạng thái SUSPENDED
    //         Create process in SUSPENDED state
    // =================================================================

    printf("=== PHAN 1: TAO TIEN TRINH SUSPENDED ===\n\n");

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmdLine[] = "cmd.exe /C echo [CON/CHILD] Toi da bat dau chay! && echo [CON/CHILD] I am now running! && timeout /t 3 /nobreak > nul && echo [CON/CHILD] Hoan thanh! / Done!";

    printf("Dang tao tien trinh voi co CREATE_SUSPENDED...\n");
    printf("Creating process with CREATE_SUSPENDED flag...\n\n");

    // CREATE_SUSPENDED: Tạo tiến trình nhưng thread chính KHÔNG chạy
    // Tiến trình ở trạng thái "tạm dừng" cho đến khi gọi ResumeThread()
    //
    // CREATE_SUSPENDED: Creates the process but the primary thread
    // does NOT start running. Process stays "suspended" until ResumeThread()
    //
    // Tại sao cần CREATE_SUSPENDED? / Why use CREATE_SUSPENDED?
    //   1. Thiết lập thêm trước khi chạy (ví dụ: set priority, affinity)
    //      Configure before running (e.g., set priority, CPU affinity)
    //   2. Chuẩn bị I/O redirection, debug hooks
    //      Prepare I/O redirection, debug hooks
    //   3. Đồng bộ hóa: đảm bảo mọi thứ sẵn sàng trước khi bắt đầu
    //      Synchronization: ensure everything is ready before starting

    BOOL success = CreateProcessA(
        NULL, cmdLine, NULL, NULL, FALSE,
        CREATE_SUSPENDED | CREATE_NEW_CONSOLE,
        //  ^^^^^^^^^^^^^^^^
        //  Co quan trong: tao tien trinh nhung CHUA chay
        //  Key flag: create process but DON'T start it
        NULL, NULL, &si, &pi
    );

    if (!success) {
        printf("[LOI/ERROR] CreateProcessA that bai! Ma loi: %lu\n", GetLastError());
        return 1;
    }

    printf("Tien trinh da tao thanh cong (nhung chua chay)!\n");
    printf("Process created successfully (but NOT running yet)!\n");
    printf("  PID: %lu\n", pi.dwProcessId);
    printf("  TID (Thread chinh / Primary thread): %lu\n\n", pi.dwThreadId);

    // =================================================================
    // PHẦN 2: Kiểm tra trạng thái SUSPENDED
    //         Verify SUSPENDED status
    // =================================================================

    printf("=== PHAN 2: KIEM TRA TRANG THAI ===\n\n");

    printf("Trang thai hien tai / Current status:\n");
    PrintProcessStatus(pi.hProcess, pi.dwProcessId);

    // Chờ kiểu non-blocking (timeout = 0) để kiểm tra
    // Non-blocking wait (timeout = 0) to check
    DWORD waitCheck = WaitForSingleObject(pi.hProcess, 0);
    if (waitCheck == WAIT_TIMEOUT) {
        printf("  -> WaitForSingleObject(0) = WAIT_TIMEOUT\n");
        printf("     Tien trinh van ton tai (suspended, chua ket thuc)\n");
        printf("     Process still exists (suspended, not finished)\n\n");
    }

    printf("Tien trinh dang o trang thai SUSPENDED.\n");
    printf("Thread chinh chua bat dau thuc thi bat ky lenh nao.\n");
    printf("Process is in SUSPENDED state.\n");
    printf("Primary thread has not executed any instructions yet.\n\n");

    // =================================================================
    // PHẦN 3: Thực hiện cấu hình trước khi resume (ví dụ)
    //         Perform pre-resume configuration (example)
    // =================================================================

    printf("=== PHAN 3: CAU HINH TRUOC KHI RESUME ===\n\n");

    // Ví dụ: Đặt độ ưu tiên cho tiến trình trước khi chạy
    // Example: Set process priority before running
    BOOL priResult = SetPriorityClass(pi.hProcess, BELOW_NORMAL_PRIORITY_CLASS);
    if (priResult) {
        printf("  Da dat do uu tien: BELOW_NORMAL_PRIORITY_CLASS\n");
        printf("  Priority set to: BELOW_NORMAL_PRIORITY_CLASS\n\n");
    }

    // Có thể làm thêm / Could also do:
    //   - SetProcessAffinityMask()  : giới hạn CPU cores
    //   - SetProcessWorkingSetSize(): giới hạn bộ nhớ
    //   - Job Objects                : nhóm tiến trình

    printf("Cho 2 giay truoc khi resume... / Waiting 2s before resume...\n\n");
    Sleep(2000);

    // =================================================================
    // PHẦN 4: RESUME - Tiếp tục chạy tiến trình
    //         RESUME - Continue running the process
    // =================================================================

    printf("=== PHAN 4: RESUME TIEN TRINH ===\n\n");

    // ResumeThread(hThread) giảm "suspend count" đi 1
    //   - Khi suspend count về 0, thread bắt đầu chạy
    //   - Trả về suspend count TRƯỚC KHI giảm
    //   - Trả về (DWORD)-1 nếu lỗi
    //
    // ResumeThread(hThread) decrements the "suspend count" by 1
    //   - When suspend count reaches 0, thread starts running
    //   - Returns the PREVIOUS suspend count before decrementing
    //   - Returns (DWORD)-1 on error

    printf("Dang goi ResumeThread()...\n");
    printf("Calling ResumeThread()...\n");

    DWORD prevSuspendCount = ResumeThread(pi.hThread);

    if (prevSuspendCount == (DWORD)-1) {
        printf("[LOI/ERROR] ResumeThread that bai! Ma loi: %lu\n", GetLastError());
    } else {
        printf("  Suspend count truoc do / Previous suspend count: %lu\n", prevSuspendCount);
        printf("  Suspend count hien tai / Current suspend count : %lu\n", prevSuspendCount - 1);
        if (prevSuspendCount - 1 == 0) {
            printf("  -> Thread da bat dau chay! / Thread is now running!\n\n");
        }
    }

    // =================================================================
    // PHẦN 5: Minh họa SUSPEND lại trong khi đang chạy
    //         Demonstrate RE-SUSPENDING while running
    // =================================================================

    printf("=== PHAN 5: TAM DUNG LAI (SuspendThread) ===\n\n");

    Sleep(1000);  // Cho chạy 1 giây / Let it run for 1 second

    printf("Dang tam dung tien trinh lai...\n");
    printf("Suspending the process again...\n");

    // SuspendThread(hThread) tăng suspend count lên 1
    //   - Thread sẽ bị tạm dừng (không chạy tiếp)
    //   - Trả về suspend count TRƯỚC KHI tăng
    //
    // SuspendThread(hThread) increments suspend count by 1
    //   - Thread will be suspended (stops running)
    //   - Returns the PREVIOUS suspend count before incrementing
    //
    // CẢNH BÁO / WARNING:
    //   SuspendThread() có thể gây DEADLOCK nếu thread đang giữ lock!
    //   Chỉ dùng cho debugging hoặc khi biết chắc an toàn.
    //   SuspendThread() can cause DEADLOCK if thread holds a lock!
    //   Only use for debugging or when you know it's safe.

    prevSuspendCount = SuspendThread(pi.hThread);

    if (prevSuspendCount == (DWORD)-1) {
        printf("[LOI/ERROR] SuspendThread that bai! Ma loi: %lu\n", GetLastError());
    } else {
        printf("  Suspend count truoc do / Previous: %lu -> Hien tai / Current: %lu\n",
               prevSuspendCount, prevSuspendCount + 1);
        printf("  Tien trinh da bi tam dung! / Process is now suspended!\n\n");
    }

    printf("Cho 2 giay (tien trinh bi dong bang)...\n");
    printf("Waiting 2s (process is frozen)...\n\n");
    Sleep(2000);

    // =================================================================
    // PHẦN 6: Resume lại và chờ kết thúc
    //         Resume again and wait for completion
    // =================================================================

    printf("=== PHAN 6: RESUME LAN 2 VA CHO KET THUC ===\n\n");

    printf("Dang resume lai...\n");
    printf("Resuming again...\n");

    prevSuspendCount = ResumeThread(pi.hThread);
    printf("  Suspend count: %lu -> %lu\n\n", prevSuspendCount, prevSuspendCount - 1);

    printf("Dang cho tien trinh ket thuc...\n");
    printf("Waiting for process to finish...\n");

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("  Tien trinh da ket thuc! Exit code: %lu\n\n", exitCode);

    // Đóng handle / Close handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // =================================================================
    // TỔNG KẾT VÒNG ĐỜI / LIFECYCLE SUMMARY
    // =================================================================

    printf("============================================================\n");
    printf("  VONG DOI TIEN TRINH / PROCESS LIFECYCLE:\n");
    printf("============================================================\n");
    printf("\n");
    printf("  CreateProcess(SUSPENDED)    ResumeThread()     Ket thuc\n");
    printf("        |                         |                 |\n");
    printf("        v                         v                 v\n");
    printf("  +-----------+             +-----------+     +-----------+\n");
    printf("  | SUSPENDED | ----------> | RUNNING   | --> | FINISHED  |\n");
    printf("  +-----------+  Resume     +-----------+     +-----------+\n");
    printf("                              |      ^              \n");
    printf("                    Suspend   |      |  Resume      \n");
    printf("                              v      |              \n");
    printf("                            +-----------+           \n");
    printf("                            | SUSPENDED |           \n");
    printf("                            +-----------+           \n");
    printf("\n");
    printf("============================================================\n");
    printf("  Ket thuc bai tap 06. / End of Exercise 06.\n");
    printf("============================================================\n");

    return 0;
}
