/*
 * ============================================================================
 * Exercise 02: Multiple Threads / Nhiều luồng
 * ============================================================================
 * Description (Mô tả):
 *   Create multiple threads that each perform different work. Wait for all
 *   threads to complete using WaitForMultipleObjects(). Demonstrate how
 *   thread IDs work and how to pass unique data to each thread.
 *
 *   Tạo nhiều luồng, mỗi luồng thực hiện công việc khác nhau. Chờ tất cả
 *   luồng hoàn thành bằng WaitForMultipleObjects(). Minh họa cách ID luồng
 *   hoạt động và cách truyền dữ liệu riêng cho từng luồng.
 *
 * Key APIs (Các API chính):
 *   - CreateThread()              : Create threads / Tạo luồng
 *   - WaitForMultipleObjects()    : Wait for all/any threads / Chờ tất cả/bất kỳ
 *   - GetCurrentThreadId()        : Get thread ID / Lấy ID luồng
 *   - CloseHandle()               : Close handle / Đóng handle
 *
 * Compile (Biên dịch):
 *   g++ 02_multiple_threads.cpp -o 02_multiple_threads.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

#define NUM_THREADS 5

// ============================================================================
// Data structure for each thread / Cấu trúc dữ liệu cho mỗi luồng
// Each thread gets its own copy of data to avoid conflicts
// Mỗi luồng nhận bản sao dữ liệu riêng để tránh xung đột
// ============================================================================
struct WorkItem {
    int    threadIndex;       // 0-based index / chỉ số bắt đầu từ 0
    char   taskName[64];     // Name of the task / Tên công việc
    int    iterations;       // How many iterations / Số lần lặp
    int    sleepMs;          // Sleep between iterations / Nghỉ giữa các lần lặp
    DWORD  result;           // Store computation result / Lưu kết quả tính toán
};

// ============================================================================
// Worker thread function / Hàm luồng công nhân
// ============================================================================
DWORD WINAPI WorkerThread(LPVOID lpParameter) {
    WorkItem* work = (WorkItem*)lpParameter;
    DWORD tid = GetCurrentThreadId();

    printf("[Thread #%d | TID=%lu] Starting task: %s (%d iterations)\n",
           work->threadIndex, tid, work->taskName, work->iterations);

    // Simulate computation / Giả lập tính toán
    DWORD sum = 0;
    for (int i = 0; i < work->iterations; i++) {
        sum += (i + 1) * (work->threadIndex + 1);

        // Print progress every few iterations / In tiến trình
        if ((i + 1) % (work->iterations / 3 + 1) == 0 ||
            i == work->iterations - 1) {
            printf("[Thread #%d | TID=%lu] Progress: %d/%d (sum=%lu)\n",
                   work->threadIndex, tid, i + 1, work->iterations, sum);
        }

        Sleep(work->sleepMs);
    }

    // Store result back in the struct (shared memory between threads)
    // Lưu kết quả vào struct (bộ nhớ chia sẻ giữa các luồng)
    work->result = sum;

    printf("[Thread #%d | TID=%lu] DONE! Result = %lu\n",
           work->threadIndex, tid, sum);

    return sum;  // Also returned as exit code / Cũng trả về làm mã thoát
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 02: Multiple Threads\n");
    printf("  Bai tap 02: Nhieu luong\n");
    printf("============================================================\n\n");

    printf("[Main Thread %lu] Creating %d threads...\n\n",
           GetCurrentThreadId(), NUM_THREADS);

    // ========================================================================
    // Prepare data for each thread / Chuẩn bị dữ liệu cho mỗi luồng
    // ========================================================================
    // IMPORTANT: Each thread gets its OWN WorkItem to avoid data races!
    // QUAN TRỌNG: Mỗi luồng nhận WorkItem RIÊNG để tránh tranh chấp dữ liệu!
    WorkItem workItems[NUM_THREADS];
    HANDLE   hThreads[NUM_THREADS];
    DWORD    threadIds[NUM_THREADS];

    const char* taskNames[] = {
        "Calculate-Sum",
        "Process-Data",
        "Analyze-Input",
        "Generate-Report",
        "Validate-Output"
    };

    // Different workloads for each thread to show they run concurrently
    // Khối lượng công việc khác nhau cho mỗi luồng để thấy chúng chạy đồng thời
    int iterations[] = { 6, 4, 8, 3, 5 };
    int sleepTimes[] = { 200, 300, 150, 400, 250 };

    // ========================================================================
    // Create all threads / Tạo tất cả luồng
    // ========================================================================
    for (int i = 0; i < NUM_THREADS; i++) {
        workItems[i].threadIndex = i;
        snprintf(workItems[i].taskName, sizeof(workItems[i].taskName),
                 "%s", taskNames[i]);
        workItems[i].iterations = iterations[i];
        workItems[i].sleepMs    = sleepTimes[i];
        workItems[i].result     = 0;

        hThreads[i] = CreateThread(
            NULL,                   // Default security
            0,                      // Default stack size
            WorkerThread,           // Thread function
            &workItems[i],          // UNIQUE data for this thread
                                    // Dữ liệu RIÊNG cho luồng này
            0,                      // Start immediately
            &threadIds[i]           // Store thread ID
        );

        if (hThreads[i] == NULL) {
            printf("[ERROR] Failed to create thread %d! Error: %lu\n",
                   i, GetLastError());
            // Clean up already-created threads
            // Dọn dẹp các luồng đã tạo
            for (int j = 0; j < i; j++) {
                WaitForSingleObject(hThreads[j], INFINITE);
                CloseHandle(hThreads[j]);
            }
            return 1;
        }

        printf("[Main] Created Thread #%d with TID=%lu\n",
               i, threadIds[i]);
    }

    // ========================================================================
    // APPROACH 1: WaitForMultipleObjects - Wait for ALL threads
    // CÁCH 1: WaitForMultipleObjects - Chờ TẤT CẢ luồng
    // ========================================================================
    //
    // DWORD WaitForMultipleObjects(
    //   DWORD        nCount,         // Number of handles / Số lượng handle
    //   const HANDLE *lpHandles,     // Array of handles / Mảng handle
    //   BOOL         bWaitAll,       // TRUE = wait ALL, FALSE = wait ANY
    //                                // TRUE = chờ TẤT CẢ, FALSE = chờ BẤT KỲ
    //   DWORD        dwMilliseconds  // Timeout / Thời gian chờ tối đa
    // );
    //
    // Return values when bWaitAll = TRUE:
    //   WAIT_OBJECT_0        = All objects signaled / Tất cả đã báo hiệu
    //   WAIT_TIMEOUT         = Timeout / Hết thời gian
    //   WAIT_FAILED          = Error / Lỗi
    //
    // Return values when bWaitAll = FALSE:
    //   WAIT_OBJECT_0 + index = Which object was signaled first
    //                           Đối tượng nào được báo hiệu đầu tiên
    //
    // NOTE: Maximum handles = MAXIMUM_WAIT_OBJECTS (64)
    // GHI CHÚ: Số handle tối đa = MAXIMUM_WAIT_OBJECTS (64)
    // ========================================================================

    printf("\n[Main] All threads created. Waiting for ALL to finish...\n");
    printf("[Main] (Using WaitForMultipleObjects with bWaitAll=TRUE)\n\n");

    DWORD waitResult = WaitForMultipleObjects(
        NUM_THREADS,    // Number of handles / Số handle
        hThreads,       // Array of thread handles / Mảng handle luồng
        TRUE,           // Wait for ALL threads / Chờ TẤT CẢ luồng
        INFINITE        // No timeout / Không giới hạn thời gian
    );

    if (waitResult == WAIT_OBJECT_0) {
        printf("\n[Main] All threads completed!\n");
    } else if (waitResult == WAIT_FAILED) {
        printf("\n[Main] WaitForMultipleObjects failed! Error: %lu\n",
               GetLastError());
    }

    // ========================================================================
    // Collect results / Thu thập kết quả
    // ========================================================================
    printf("\n--- Results Summary / Tong ket ket qua ---\n\n");

    DWORD totalSum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        // Method 1: Read from shared struct
        // Cách 1: Đọc từ struct chia sẻ
        printf("  Thread #%d (TID=%lu): task=%-18s result=%lu\n",
               i, threadIds[i], workItems[i].taskName, workItems[i].result);
        totalSum += workItems[i].result;

        // Method 2: Could also use GetExitCodeThread
        // Cách 2: Cũng có thể dùng GetExitCodeThread
        DWORD exitCode;
        GetExitCodeThread(hThreads[i], &exitCode);
        // exitCode should match workItems[i].result

        // Close the handle / Đóng handle
        CloseHandle(hThreads[i]);
    }

    printf("\n  Total sum across all threads: %lu\n", totalSum);

    // ========================================================================
    // BONUS: Demonstrate WaitForMultipleObjects with bWaitAll=FALSE
    // BONUS: Minh họa WaitForMultipleObjects với bWaitAll=FALSE
    // ========================================================================
    printf("\n--- BONUS: Wait for ANY (first to finish) ---\n\n");

    // Create 3 threads with different durations
    // Tạo 3 luồng với thời gian khác nhau
    #define BONUS_THREADS 3
    HANDLE   bonusHandles[BONUS_THREADS];
    WorkItem bonusWork[BONUS_THREADS];
    DWORD    bonusIds[BONUS_THREADS];

    int bonusIter[]  = { 10, 3, 7 };   // Thread 1 (index=1) finishes first
    int bonusSleep[] = { 100, 100, 100 };

    for (int i = 0; i < BONUS_THREADS; i++) {
        bonusWork[i].threadIndex = i + 10;  // Use different indices to distinguish
        snprintf(bonusWork[i].taskName, sizeof(bonusWork[i].taskName),
                 "BonusTask-%d", i);
        bonusWork[i].iterations = bonusIter[i];
        bonusWork[i].sleepMs    = bonusSleep[i];
        bonusWork[i].result     = 0;

        bonusHandles[i] = CreateThread(NULL, 0, WorkerThread,
                                       &bonusWork[i], 0, &bonusIds[i]);
        if (bonusHandles[i] == NULL) {
            printf("[ERROR] Failed to create bonus thread %d\n", i);
            return 1;
        }
    }

    // Wait for ANY thread to finish (bWaitAll = FALSE)
    // Chờ BẤT KỲ luồng nào kết thúc (bWaitAll = FALSE)
    waitResult = WaitForMultipleObjects(
        BONUS_THREADS,
        bonusHandles,
        FALSE,          // Wait for ANY / Chờ BẤT KỲ
        INFINITE
    );

    if (waitResult >= WAIT_OBJECT_0 &&
        waitResult < WAIT_OBJECT_0 + BONUS_THREADS) {
        int idx = waitResult - WAIT_OBJECT_0;
        printf("\n[Main] First thread to finish: Thread #%d (TID=%lu)\n",
               bonusWork[idx].threadIndex, bonusIds[idx]);
    }

    // Still need to wait for the rest / Vẫn cần chờ phần còn lại
    printf("[Main] Waiting for remaining threads...\n");
    WaitForMultipleObjects(BONUS_THREADS, bonusHandles, TRUE, INFINITE);

    for (int i = 0; i < BONUS_THREADS; i++) {
        CloseHandle(bonusHandles[i]);
    }

    // ========================================================================
    printf("\n============================================================\n");
    printf("  Summary / Tom tat:\n");
    printf("  - Each thread should get its OWN data (avoid sharing)\n");
    printf("    Moi luong nen co du lieu RIENG (tranh chia se)\n");
    printf("  - WaitForMultipleObjects(bWaitAll=TRUE): wait ALL\n");
    printf("    Cho TAT CA luong ket thuc\n");
    printf("  - WaitForMultipleObjects(bWaitAll=FALSE): wait ANY\n");
    printf("    Cho BAT KY luong nao ket thuc\n");
    printf("  - Max 64 handles (MAXIMUM_WAIT_OBJECTS)\n");
    printf("============================================================\n");

    return 0;
}
