/*
 * ============================================================================
 * Exercise 06: Thread Suspend and Resume
 * Bai tap 06: Treo va Phuc hoi luong
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrates how to control thread execution using SuspendThread()
 *   and ResumeThread(). Shows how to create threads in suspended state,
 *   manage suspend counts, and control thread lifecycle.
 *
 *   Minh họa cách kiểm soát thực thi luồng bằng SuspendThread() và
 *   ResumeThread(). Cho thấy cách tạo luồng ở trạng thái treo, quản lý
 *   bộ đếm treo, và kiểm soát vòng đời luồng.
 *
 * Key APIs (Các API chính):
 *   - CreateThread(CREATE_SUSPENDED) : Create suspended thread / Tạo luồng bị treo
 *   - SuspendThread()                : Suspend a running thread / Treo luồng đang chạy
 *   - ResumeThread()                 : Resume a suspended thread / Phục hồi luồng bị treo
 *   - GetThreadContext()             : Get thread state / Lấy trạng thái luồng
 *
 * Compile (Biên dịch):
 *   g++ 06_thread_suspend_resume.cpp -o 06_thread_suspend_resume.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// SUSPEND COUNT CONCEPT / KHÁI NIỆM BỘ ĐẾM TREO
// ============================================================================
//
// Each thread has a "suspend count" (internal counter):
// Mỗi luồng có "bộ đếm treo" (bộ đếm nội bộ):
//
//   - suspend count == 0: thread is RUNNING (or ready to run)
//     suspend count == 0: luồng đang CHẠY (hoặc sẵn sàng chạy)
//
//   - suspend count > 0: thread is SUSPENDED (will not be scheduled)
//     suspend count > 0: luồng bị TREO (sẽ không được lên lịch)
//
//   SuspendThread(): increments suspend count, returns PREVIOUS count
//     Tăng bộ đếm treo, trả về giá trị TRƯỚC ĐÓ
//
//   ResumeThread(): decrements suspend count, returns PREVIOUS count
//     Giảm bộ đếm treo, trả về giá trị TRƯỚC ĐÓ
//
//   CreateThread(CREATE_SUSPENDED): starts with suspend count = 1
//     Bắt đầu với bộ đếm treo = 1
//
// IMPORTANT: If you call SuspendThread() twice, you must call
// ResumeThread() twice to actually resume the thread!
//
// QUAN TRỌNG: Nếu bạn gọi SuspendThread() hai lần, bạn phải gọi
// ResumeThread() hai lần để thực sự phục hồi luồng!
// ============================================================================

// ============================================================================
// Counter thread - counts up continuously / Luồng đếm - đếm lên liên tục
// ============================================================================
struct CounterData {
    int   id;
    volatile BOOL shouldStop;   // Flag to signal thread to stop
                                // Cờ báo hiệu luồng dừng lại
};

DWORD WINAPI CounterThread(LPVOID lpParameter) {
    CounterData* data = (CounterData*)lpParameter;
    int count = 0;

    printf("[Counter #%d | TID=%lu] Thread started!\n",
           data->id, GetCurrentThreadId());

    while (!data->shouldStop) {
        count++;
        if (count % 5 == 0) {
            printf("[Counter #%d] Count = %d\n", data->id, count);
        }
        Sleep(200);  // Count every 200ms
    }

    printf("[Counter #%d] Stopping at count = %d\n", data->id, count);
    return count;
}

// ============================================================================
// Worker thread for suspend demonstration
// Luồng công nhân cho minh họa treo
// ============================================================================
DWORD WINAPI WorkerThread(LPVOID lpParameter) {
    int id = *(int*)lpParameter;

    printf("[Worker #%d | TID=%lu] I'm alive and running!\n",
           id, GetCurrentThreadId());

    for (int i = 1; i <= 10; i++) {
        printf("[Worker #%d] Step %d/10\n", id, i);
        Sleep(300);
    }

    printf("[Worker #%d] Finished all work!\n", id);
    return 0;
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 06: Thread Suspend and Resume\n");
    printf("  Bai tap 06: Treo va Phuc hoi luong\n");
    printf("============================================================\n\n");

    // ========================================================================
    // PART 1: Create thread in SUSPENDED state
    // PHẦN 1: Tạo luồng ở trạng thái TREO
    // ========================================================================
    printf("=== PART 1: CREATE_SUSPENDED Flag ===\n\n");

    int workerId = 1;
    DWORD threadId;

    // ========================================================================
    // When dwCreationFlags = CREATE_SUSPENDED, the thread is created
    // but does NOT start executing. It has suspend count = 1.
    //
    // Khi dwCreationFlags = CREATE_SUSPENDED, luồng được tạo nhưng
    // KHÔNG bắt đầu thực thi. Nó có bộ đếm treo = 1.
    //
    // This is useful when you need to:
    // Điều này hữu ích khi bạn cần:
    //   - Set thread priority before it starts
    //     Đặt ưu tiên luồng trước khi nó bắt đầu
    //   - Store the handle/ID before the thread runs
    //     Lưu handle/ID trước khi luồng chạy
    //   - Coordinate startup of multiple threads
    //     Phối hợp khởi động nhiều luồng
    // ========================================================================

    HANDLE hWorker = CreateThread(
        NULL,                // Default security
        0,                   // Default stack size
        WorkerThread,        // Thread function
        &workerId,           // Parameter
        CREATE_SUSPENDED,    // START SUSPENDED / BẮT ĐẦU BỊ TREO
        &threadId            // Thread ID
    );

    if (hWorker == NULL) {
        printf("[ERROR] CreateThread failed! Error: %lu\n", GetLastError());
        return 1;
    }

    printf("[Main] Thread created with TID=%lu (SUSPENDED)\n", threadId);
    printf("[Main] Thread is frozen - it hasn't started yet.\n");
    printf("[Main] Luong da duoc tao nhung bi treo - chua bat dau.\n\n");

    // Do some setup work while thread is suspended
    // Làm công việc chuẩn bị khi luồng bị treo
    printf("[Main] Doing some setup work...\n");
    Sleep(1000);

    // ========================================================================
    // ResumeThread() - Resume the suspended thread
    // ResumeThread() - Phục hồi luồng bị treo
    //
    // DWORD ResumeThread(HANDLE hThread);
    //
    // Returns the PREVIOUS suspend count.
    // Trả về bộ đếm treo TRƯỚC ĐÓ.
    //   - Returns 1: thread was suspended, now it can run (count: 1 -> 0)
    //     Trả về 1: luồng đã bị treo, giờ có thể chạy (count: 1 -> 0)
    //   - Returns 0: thread was already running (shouldn't happen here)
    //     Trả về 0: luồng đã đang chạy
    //   - Returns (DWORD)-1: error
    //     Trả về (DWORD)-1: lỗi
    // ========================================================================

    printf("[Main] Now resuming the thread...\n\n");

    DWORD prevSuspendCount = ResumeThread(hWorker);
    printf("[Main] ResumeThread returned: %lu (previous suspend count)\n",
           prevSuspendCount);
    printf("[Main] Thread is now running!\n\n");

    // Wait a bit then let it finish / Chờ một chút rồi để nó kết thúc
    WaitForSingleObject(hWorker, INFINITE);
    CloseHandle(hWorker);

    // ========================================================================
    // PART 2: Suspend a RUNNING thread
    // PHẦN 2: Treo luồng đang CHẠY
    // ========================================================================
    printf("\n=== PART 2: Suspending a Running Thread ===\n\n");

    CounterData counterData;
    counterData.id = 1;
    counterData.shouldStop = FALSE;

    HANDLE hCounter = CreateThread(NULL, 0, CounterThread,
                                   &counterData, 0, &threadId);
    if (hCounter == NULL) {
        printf("[ERROR] CreateThread failed!\n");
        return 1;
    }

    printf("[Main] Counter thread running. Waiting 2 seconds...\n\n");
    Sleep(2000);

    // ========================================================================
    // SuspendThread() - Suspend a running thread
    // SuspendThread() - Treo luồng đang chạy
    //
    // DWORD SuspendThread(HANDLE hThread);
    //
    // Returns the PREVIOUS suspend count.
    // Trả về bộ đếm treo TRƯỚC ĐÓ.
    //   - Returns 0: thread was running, now suspended (count: 0 -> 1)
    //     Trả về 0: luồng đang chạy, giờ bị treo (count: 0 -> 1)
    //   - Returns N: thread was already suspended N times
    //     Trả về N: luồng đã bị treo N lần
    //
    // WARNING: SuspendThread is dangerous if the thread holds a lock!
    // CẢNH BÁO: SuspendThread nguy hiểm nếu luồng đang giữ khóa!
    // The thread could be in the middle of a critical operation.
    // Luồng có thể đang ở giữa thao tác tới hạn.
    // ========================================================================

    printf("[Main] >>> SUSPENDING the counter thread...\n");
    prevSuspendCount = SuspendThread(hCounter);
    printf("[Main] SuspendThread returned: %lu (previous count)\n",
           prevSuspendCount);
    printf("[Main] Thread is now FROZEN. Notice: no more counting!\n");
    printf("[Main] Luong da bi DONG BANG. Chu y: khong con dem nua!\n\n");

    // The counter thread is frozen - no output from it
    // Luồng đếm bị đóng băng - không có output từ nó
    printf("[Main] Sleeping 2 seconds while thread is suspended...\n");
    Sleep(2000);
    printf("[Main] (See? No counter output during suspension!)\n\n");

    // Resume it / Phục hồi nó
    printf("[Main] >>> RESUMING the counter thread...\n");
    prevSuspendCount = ResumeThread(hCounter);
    printf("[Main] ResumeThread returned: %lu (previous count)\n",
           prevSuspendCount);
    printf("[Main] Thread resumed! Counting continues...\n\n");

    Sleep(2000);

    // Stop the thread gracefully / Dừng luồng một cách nhẹ nhàng
    printf("[Main] Signaling counter thread to stop...\n");
    counterData.shouldStop = TRUE;
    WaitForSingleObject(hCounter, INFINITE);
    CloseHandle(hCounter);

    // ========================================================================
    // PART 3: Multiple suspend/resume (suspend count stacking)
    // PHẦN 3: Treo/phục hồi nhiều lần (chồng bộ đếm treo)
    // ========================================================================
    printf("\n=== PART 3: Suspend Count Stacking ===\n\n");

    int workerId2 = 2;
    HANDLE hWorker2 = CreateThread(NULL, 0, WorkerThread,
                                   &workerId2, 0, &threadId);
    if (hWorker2 == NULL) {
        printf("[ERROR] CreateThread failed!\n");
        return 1;
    }

    Sleep(500);  // Let it start / Cho nó bắt đầu

    // Suspend multiple times / Treo nhiều lần
    printf("[Main] Suspending thread 3 times...\n");

    DWORD sc1 = SuspendThread(hWorker2);
    printf("[Main] SuspendThread #1 returned: %lu (suspend count: 0->1)\n", sc1);

    DWORD sc2 = SuspendThread(hWorker2);
    printf("[Main] SuspendThread #2 returned: %lu (suspend count: 1->2)\n", sc2);

    DWORD sc3 = SuspendThread(hWorker2);
    printf("[Main] SuspendThread #3 returned: %lu (suspend count: 2->3)\n", sc3);

    printf("\n[Main] Thread has suspend count = 3.\n");
    printf("[Main] Need 3 ResumeThread calls to actually resume!\n\n");

    // Resume one at a time / Phục hồi từng lần một
    DWORD rc1 = ResumeThread(hWorker2);
    printf("[Main] ResumeThread #1 returned: %lu (suspend count: 3->2)\n", rc1);
    printf("[Main] Still suspended? YES (count=2)\n");
    Sleep(500);  // Thread doesn't run / Luồng không chạy

    DWORD rc2 = ResumeThread(hWorker2);
    printf("[Main] ResumeThread #2 returned: %lu (suspend count: 2->1)\n", rc2);
    printf("[Main] Still suspended? YES (count=1)\n");
    Sleep(500);  // Thread still doesn't run / Luồng vẫn không chạy

    DWORD rc3 = ResumeThread(hWorker2);
    printf("[Main] ResumeThread #3 returned: %lu (suspend count: 1->0)\n", rc3);
    printf("[Main] Still suspended? NO! (count=0) Thread RUNS now!\n\n");

    // Now the thread actually runs / Bây giờ luồng thực sự chạy
    WaitForSingleObject(hWorker2, INFINITE);
    CloseHandle(hWorker2);

    // ========================================================================
    // PART 4: Coordinated start of multiple threads
    // PHẦN 4: Khởi động phối hợp nhiều luồng
    // ========================================================================
    printf("\n=== PART 4: Coordinated Thread Start ===\n\n");

    #define COORD_THREADS 3
    HANDLE  coordHandles[COORD_THREADS];
    int     coordIds[COORD_THREADS];

    printf("[Main] Creating %d threads in SUSPENDED state...\n", COORD_THREADS);

    // Create all threads suspended / Tạo tất cả luồng bị treo
    for (int i = 0; i < COORD_THREADS; i++) {
        coordIds[i] = i + 10;
        coordHandles[i] = CreateThread(NULL, 0, WorkerThread,
                                       &coordIds[i],
                                       CREATE_SUSPENDED,
                                       NULL);
        if (coordHandles[i] == NULL) {
            printf("[ERROR] Failed to create thread %d\n", i);
            return 1;
        }
        printf("  Thread #%d created (suspended)\n", coordIds[i]);
    }

    printf("\n[Main] All threads created. Preparing to start them all at once.\n");
    printf("[Main] Tat ca luong da tao. Chuan bi bat dau tat ca cung luc.\n\n");
    Sleep(1000);

    // Resume all at once for synchronized start
    // Phục hồi tất cả cùng lúc để bắt đầu đồng bộ
    printf("[Main] >>> RESUMING ALL THREADS NOW! <<<\n\n");
    for (int i = 0; i < COORD_THREADS; i++) {
        ResumeThread(coordHandles[i]);
    }

    // Wait for all / Chờ tất cả
    WaitForMultipleObjects(COORD_THREADS, coordHandles, TRUE, INFINITE);

    for (int i = 0; i < COORD_THREADS; i++) {
        CloseHandle(coordHandles[i]);
    }

    // ========================================================================
    printf("\n============================================================\n");
    printf("  Summary / Tom tat:\n");
    printf("------------------------------------------------------------\n");
    printf("  - CREATE_SUSPENDED: thread starts frozen (count=1)\n");
    printf("    Luong bat dau bi dong bang (count=1)\n");
    printf("  - SuspendThread(): increments suspend count\n");
    printf("    Tang bo dem treo\n");
    printf("  - ResumeThread(): decrements suspend count\n");
    printf("    Giam bo dem treo\n");
    printf("  - Thread runs only when suspend count reaches 0\n");
    printf("    Luong chay chi khi bo dem treo giam ve 0\n");
    printf("  - WARNING: Don't suspend threads holding locks!\n");
    printf("    CANH BAO: Dung treo luong dang giu khoa!\n");
    printf("============================================================\n");

    return 0;
}
