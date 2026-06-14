/*
 * ============================================================================
 * BÀI TẬP 07: QUẢN LÝ NHIỀU TIẾN TRÌNH CON (Multiple Child Processes)
 * ============================================================================
 * Mô tả / Description:
 *   Tạo nhiều tiến trình con, theo dõi quan hệ cha-con, và chờ tất cả
 *   kết thúc bằng WaitForMultipleObjects(). Đây là kỹ thuật quan trọng
 *   trong shell khi cần chạy nhiều lệnh song song.
 *
 *   Create multiple child processes, track parent-child relationships,
 *   and wait for all to finish using WaitForMultipleObjects(). This is
 *   an important technique in shell design for running parallel commands.
 *
 * API chính / Key APIs:
 *   - CreateProcess()           : Tạo tiến trình con
 *   - WaitForMultipleObjects()  : Chờ nhiều đối tượng cùng lúc
 *   - WaitForSingleObject()     : Chờ từng đối tượng
 *   - GetExitCodeProcess()      : Lấy mã thoát
 *   - GetCurrentProcessId()     : Lấy PID hiện tại
 *
 * Biên dịch / Compile:
 *   g++ 07_child_processes.cpp -o 07_child_processes.exe
 *   cl 07_child_processes.cpp /Fe:07_child_processes.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// Số tiến trình con tối đa / Maximum number of child processes
#define MAX_CHILDREN 5

// Cấu trúc lưu thông tin tiến trình con
// Structure to store child process information
typedef struct {
    PROCESS_INFORMATION pi;     // Thông tin từ CreateProcess
    char                command[512];  // Lệnh đã chạy / Command executed
    BOOL                active;       // Đang chạy? / Is active?
    int                 index;        // Số thứ tự / Index number
} ChildProcess;

// Hàm tạo tiến trình con
// Function to create a child process
BOOL CreateChild(const char* command, ChildProcess* child, int index)
{
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&child->pi, sizeof(child->pi));

    strncpy(child->command, command, sizeof(child->command) - 1);
    child->index = index;

    // Cần bản sao có thể sửa đổi / Need modifiable copy
    char cmdLine[512];
    strncpy(cmdLine, command, sizeof(cmdLine) - 1);
    cmdLine[sizeof(cmdLine) - 1] = '\0';

    BOOL success = CreateProcessA(
        NULL, cmdLine, NULL, NULL, FALSE,
        0,      // Dùng console chung của cha / Share parent's console
        NULL, NULL, &si, &child->pi
    );

    if (success) {
        child->active = TRUE;
        printf("  [+] Con #%d: PID=%lu, Lenh='%s'\n",
               index, child->pi.dwProcessId, command);
    } else {
        child->active = FALSE;
        printf("  [X] Con #%d: THAT BAI! Ma loi: %lu, Lenh='%s'\n",
               index, GetLastError(), command);
    }

    return success;
}

int main()
{
    printf("============================================================\n");
    printf("  BAI TAP 07: QUAN LY NHIEU TIEN TRINH CON\n");
    printf("  Exercise 07: Managing Multiple Child Processes\n");
    printf("============================================================\n\n");

    printf("PID cua tien trinh cha / Parent PID: %lu\n\n", GetCurrentProcessId());

    ChildProcess children[MAX_CHILDREN];
    ZeroMemory(children, sizeof(children));

    // =================================================================
    // PHẦN 1: Tạo nhiều tiến trình con với thời gian chạy khác nhau
    //         Create multiple children with different run times
    // =================================================================

    printf("=== PHAN 1: TAO %d TIEN TRINH CON ===\n\n", MAX_CHILDREN);

    // Mỗi tiến trình con sẽ chạy trong thời gian khác nhau
    // Each child process will run for a different duration
    // Dùng "timeout" để mô phỏng công việc kéo dài
    // Using "timeout" to simulate long-running work
    const char* commands[MAX_CHILDREN] = {
        "cmd.exe /C echo Con 1 bat dau (2s) && timeout /t 2 /nobreak > nul && echo Con 1 xong!",
        "cmd.exe /C echo Con 2 bat dau (4s) && timeout /t 4 /nobreak > nul && echo Con 2 xong!",
        "cmd.exe /C echo Con 3 bat dau (1s) && timeout /t 1 /nobreak > nul && echo Con 3 xong!",
        "cmd.exe /C echo Con 4 bat dau (3s) && timeout /t 3 /nobreak > nul && echo Con 4 xong!",
        "cmd.exe /C echo Con 5 bat dau (2s) && timeout /t 2 /nobreak > nul && echo Con 5 xong!",
    };

    int activeCount = 0;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (CreateChild(commands[i], &children[i], i + 1)) {
            activeCount++;
        }
    }

    printf("\n  Tong so tien trinh con tao thanh cong / Total created: %d\n\n", activeCount);

    if (activeCount == 0) {
        printf("[LOI/ERROR] Khong tao duoc tien trinh con nao!\n");
        return 1;
    }

    // =================================================================
    // PHẦN 2: Hiển thị quan hệ cha-con
    //         Display parent-child relationships
    // =================================================================

    printf("=== PHAN 2: QUAN HE CHA-CON / PARENT-CHILD TREE ===\n\n");
    printf("  Tien trinh cha / Parent Process (PID: %lu)\n", GetCurrentProcessId());

    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (children[i].active) {
            printf("    |--- Con / Child #%d (PID: %lu)\n",
                   children[i].index, children[i].pi.dwProcessId);
        }
    }
    printf("\n");

    // =================================================================
    // PHẦN 3: WaitForMultipleObjects - Chờ TẤT CẢ kết thúc
    //         WaitForMultipleObjects - Wait for ALL to finish
    // =================================================================

    printf("=== PHAN 3: CHO TAT CA KET THUC (WaitForMultipleObjects) ===\n\n");

    // Chuẩn bị mảng handle để chờ
    // Prepare array of handles to wait on
    HANDLE handles[MAX_CHILDREN];
    int handleCount = 0;

    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (children[i].active) {
            handles[handleCount++] = children[i].pi.hProcess;
        }
    }

    // WaitForMultipleObjects() - Chờ nhiều đối tượng
    // WaitForMultipleObjects() - Wait for multiple objects
    //
    // Nguyên mẫu / Prototype:
    // DWORD WaitForMultipleObjects(
    //   DWORD  nCount,          // Số lượng handle / Number of handles
    //   const HANDLE* lpHandles,// Mảng handle / Array of handles
    //   BOOL   bWaitAll,        // TRUE: chờ TẤT CẢ, FALSE: chờ BẤT KỲ
    //                           // TRUE: wait for ALL, FALSE: wait for ANY
    //   DWORD  dwMilliseconds   // Thời gian chờ tối đa / Maximum wait time
    // );
    //
    // Giá trị trả về khi bWaitAll = TRUE:
    // Return value when bWaitAll = TRUE:
    //   WAIT_OBJECT_0   : Tất cả đã hoàn thành / All completed
    //   WAIT_TIMEOUT    : Hết thời gian / Timed out
    //   WAIT_FAILED     : Lỗi / Error
    //
    // Giá trị trả về khi bWaitAll = FALSE:
    // Return value when bWaitAll = FALSE:
    //   WAIT_OBJECT_0 + index : Chỉ số handle đã hoàn thành
    //                           Index of the completed handle

    printf("Dang cho %d tien trinh con ket thuc...\n", handleCount);
    printf("Waiting for %d child processes to finish...\n\n", handleCount);

    // --- Cách 1: Chờ TẤT CẢ cùng lúc ---
    // --- Method 1: Wait for ALL at once ---
    DWORD startTime = GetTickCount();

    DWORD result = WaitForMultipleObjects(
        handleCount,    // Số handle / Number of handles
        handles,        // Mảng handle / Handle array
        TRUE,           // TRUE = chờ TẤT CẢ / TRUE = wait for ALL
        30000           // Timeout 30 giây / 30 second timeout
    );

    DWORD elapsed = GetTickCount() - startTime;

    if (result == WAIT_OBJECT_0) {
        printf("  TAT CA tien trinh con da ket thuc!\n");
        printf("  ALL child processes have finished!\n");
        printf("  Thoi gian cho / Wait time: %lu ms\n\n", elapsed);
    } else if (result == WAIT_TIMEOUT) {
        printf("  HET THOI GIAN CHO! Co tien trinh chua ket thuc.\n");
        printf("  TIMED OUT! Some processes haven't finished.\n\n");
    } else if (result == WAIT_FAILED) {
        printf("  [LOI/ERROR] WaitForMultipleObjects that bai! Ma loi: %lu\n",
               GetLastError());
    }

    // =================================================================
    // PHẦN 4: Lấy mã thoát của từng tiến trình con
    //         Get exit codes of each child process
    // =================================================================

    printf("=== PHAN 4: MA THOAT CUA TUNG TIEN TRINH CON ===\n\n");

    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (children[i].active) {
            DWORD exitCode;
            GetExitCodeProcess(children[i].pi.hProcess, &exitCode);
            printf("  Con #%d (PID %lu): exit code = %lu %s\n",
                   children[i].index,
                   children[i].pi.dwProcessId,
                   exitCode,
                   (exitCode == 0) ? "(thanh cong/success)" : "");
        }
    }

    // =================================================================
    // PHẦN 5 (BONUS): WaitForMultipleObjects với bWaitAll = FALSE
    //         Wait for ANY (demonstrate which finishes first)
    // =================================================================

    printf("\n=== PHAN 5 (BONUS): CHO TUNG CON KET THUC (bWaitAll=FALSE) ===\n\n");

    printf("Tao lai %d tien trinh con de minh hoa WaitAny...\n", 3);
    printf("Re-creating 3 children to demonstrate WaitAny...\n\n");

    ChildProcess newChildren[3];
    ZeroMemory(newChildren, sizeof(newChildren));

    const char* newCmds[3] = {
        "cmd.exe /C timeout /t 3 /nobreak > nul",  // 3 giây
        "cmd.exe /C timeout /t 1 /nobreak > nul",  // 1 giây (nhanh nhất / fastest)
        "cmd.exe /C timeout /t 2 /nobreak > nul",  // 2 giây
    };

    HANDLE newHandles[3];
    int newCount = 0;

    for (int i = 0; i < 3; i++) {
        if (CreateChild(newCmds[i], &newChildren[i], i + 1)) {
            newHandles[newCount++] = newChildren[i].pi.hProcess;
        }
    }

    printf("\n  Cho tung tien trinh ket thuc theo thu tu hoan thanh...\n");
    printf("  Waiting for processes in completion order...\n\n");

    // Lặp: mỗi lần chờ 1 tiến trình bất kỳ kết thúc
    // Loop: each iteration waits for ANY one process to finish
    int remaining = newCount;
    HANDLE waitHandles[3];
    int handleMap[3];  // Ánh xạ vị trí handle -> vị trí gốc / Map handle pos -> original pos

    // Khởi tạo / Initialize
    for (int i = 0; i < newCount; i++) {
        waitHandles[i] = newHandles[i];
        handleMap[i] = i;
    }

    int order = 1;
    while (remaining > 0) {
        // Chờ BẤT KỲ tiến trình nào kết thúc
        // Wait for ANY process to finish
        result = WaitForMultipleObjects(remaining, waitHandles, FALSE, INFINITE);

        if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + (DWORD)remaining) {
            int idx = result - WAIT_OBJECT_0;    // Chỉ số trong mảng chờ hiện tại
            int origIdx = handleMap[idx];          // Chỉ số gốc

            DWORD exitCode;
            GetExitCodeProcess(newChildren[origIdx].pi.hProcess, &exitCode);
            printf("  Thu tu #%d: Con #%d (PID %lu) da ket thuc, exit code: %lu\n",
                   order++, origIdx + 1,
                   newChildren[origIdx].pi.dwProcessId, exitCode);

            // Loại handle đã kết thúc khỏi mảng chờ bằng cách dịch
            // Remove finished handle from wait array by shifting
            for (int j = idx; j < remaining - 1; j++) {
                waitHandles[j] = waitHandles[j + 1];
                handleMap[j] = handleMap[j + 1];
            }
            remaining--;
        } else {
            printf("  [LOI/ERROR] WaitForMultipleObjects: %lu\n", result);
            break;
        }
    }

    printf("\n  Tat ca da ket thuc theo thu tu hoan thanh!\n");
    printf("  All finished in completion order!\n");

    // =================================================================
    // PHẦN 6: Dọn dẹp - Đóng TẤT CẢ handle
    //         Cleanup - Close ALL handles
    // =================================================================

    printf("\n=== PHAN 6: DON DEP / CLEANUP ===\n\n");

    // Đóng handle của đợt 1 / Close batch 1 handles
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (children[i].active) {
            CloseHandle(children[i].pi.hProcess);
            CloseHandle(children[i].pi.hThread);
        }
    }

    // Đóng handle của đợt 2 / Close batch 2 handles
    for (int i = 0; i < 3; i++) {
        if (newChildren[i].active) {
            CloseHandle(newChildren[i].pi.hProcess);
            CloseHandle(newChildren[i].pi.hThread);
        }
    }

    printf("  Da dong tat ca handle. / All handles closed.\n");

    // =================================================================
    // TỔNG KẾT / SUMMARY
    // =================================================================

    printf("\n============================================================\n");
    printf("  TONG KET WaitForMultipleObjects:\n");
    printf("  SUMMARY of WaitForMultipleObjects:\n");
    printf("============================================================\n");
    printf("  bWaitAll = TRUE:  Cho TAT CA -> WAIT_OBJECT_0 khi xong\n");
    printf("  bWaitAll = TRUE:  Wait ALL  -> WAIT_OBJECT_0 when done\n\n");
    printf("  bWaitAll = FALSE: Cho BAT KY -> WAIT_OBJECT_0 + index\n");
    printf("  bWaitAll = FALSE: Wait ANY   -> WAIT_OBJECT_0 + index\n\n");
    printf("  GIOI HAN: Toi da %d handle cung luc\n", MAXIMUM_WAIT_OBJECTS);
    printf("  LIMIT: Max %d handles at once\n", MAXIMUM_WAIT_OBJECTS);
    printf("============================================================\n");
    printf("  Ket thuc bai tap 07. / End of Exercise 07.\n");
    printf("============================================================\n");

    return 0;
}
