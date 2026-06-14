/*
 * ============================================================================
 * Exercise 03: Thread Synchronization with Mutex
 * Bai tap 03: Dong bo luong voi Mutex
 * ============================================================================
 * Description (Mô tả):
 *   First demonstrates the RACE CONDITION problem when multiple threads
 *   modify a shared variable without synchronization. Then fixes it using
 *   a Windows Mutex (Mutual Exclusion object).
 *
 *   Đầu tiên minh họa vấn đề RACE CONDITION (điều kiện tranh chấp) khi
 *   nhiều luồng sửa đổi biến chia sẻ mà không đồng bộ. Sau đó sửa bằng
 *   Mutex (đối tượng loại trừ lẫn nhau) của Windows.
 *
 * Key APIs (Các API chính):
 *   - CreateMutex()           : Create a mutex / Tạo mutex
 *   - WaitForSingleObject()   : Acquire (lock) the mutex / Chiếm (khóa) mutex
 *   - ReleaseMutex()          : Release (unlock) the mutex / Nhả (mở khóa) mutex
 *   - CloseHandle()           : Destroy the mutex / Hủy mutex
 *
 * Compile (Biên dịch):
 *   g++ 03_thread_sync_mutex.cpp -o 03_thread_sync_mutex.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

#define NUM_THREADS       4
#define INCREMENTS_EACH   100000  // Each thread increments this many times
                                  // Mỗi luồng tăng từng này lần

// ============================================================================
// Shared data - this is what causes problems without synchronization!
// Dữ liệu chia sẻ - đây là nguyên nhân gây ra vấn đề nếu không đồng bộ!
// ============================================================================
volatile long g_unsafeCounter = 0;   // No protection / Không bảo vệ
volatile long g_safeCounter   = 0;   // Protected by mutex / Được bảo vệ bởi mutex

HANDLE g_hMutex = NULL;  // Global mutex handle / Handle mutex toàn cục

// ============================================================================
// UNSAFE thread function - NO synchronization (race condition!)
// Hàm luồng KHÔNG AN TOÀN - KHÔNG đồng bộ (điều kiện tranh chấp!)
// ============================================================================
// WHY RACE CONDITION HAPPENS / TẠI SAO RACE CONDITION XẢY RA:
//
//   The operation "counter++" looks atomic but is actually 3 steps:
//   Phép toán "counter++" trông nguyên tử nhưng thực tế là 3 bước:
//
//   1. READ:  Load counter from memory to register
//             Đọc counter từ bộ nhớ vào thanh ghi
//   2. ADD:   Increment the register value
//             Tăng giá trị thanh ghi
//   3. WRITE: Store register back to memory
//             Ghi thanh ghi trở lại bộ nhớ
//
//   If Thread A reads counter=100, then Thread B also reads counter=100,
//   both increment to 101 and write back 101. We lost one increment!
//
//   Nếu Luồng A đọc counter=100, rồi Luồng B cũng đọc counter=100,
//   cả hai tăng lên 101 và ghi lại 101. Chúng ta mất một lần tăng!
// ============================================================================
DWORD WINAPI UnsafeIncrementThread(LPVOID lpParameter) {
    int threadIndex = *(int*)lpParameter;

    for (int i = 0; i < INCREMENTS_EACH; i++) {
        g_unsafeCounter++;  // NOT thread-safe! / KHÔNG an toàn cho luồng!
    }

    printf("  [Unsafe Thread #%d] Done. Counter seen = %ld\n",
           threadIndex, g_unsafeCounter);
    return 0;
}

// ============================================================================
// SAFE thread function - Protected by Mutex
// Hàm luồng AN TOÀN - Được bảo vệ bởi Mutex
// ============================================================================
// HOW MUTEX WORKS / MUTEX HOẠT ĐỘNG NHƯ THẾ NÀO:
//
//   A Mutex is like a key to a room. Only one thread can hold the key
//   at a time. Other threads must wait at the door until the key is
//   released.
//
//   Mutex giống như chìa khóa phòng. Chỉ một luồng có thể giữ chìa khóa
//   tại một thời điểm. Các luồng khác phải chờ ở cửa cho đến khi chìa
//   khóa được trả lại.
//
//   WaitForSingleObject(mutex) = "Try to grab the key" / "Cố lấy chìa khóa"
//   ReleaseMutex(mutex)        = "Put the key back" / "Trả lại chìa khóa"
// ============================================================================
DWORD WINAPI SafeIncrementThread(LPVOID lpParameter) {
    int threadIndex = *(int*)lpParameter;

    for (int i = 0; i < INCREMENTS_EACH; i++) {
        // ====================================================================
        // Acquire the mutex (enter critical section)
        // Chiếm mutex (vào vùng tới hạn)
        // ====================================================================
        // WaitForSingleObject blocks until the mutex is available
        // WaitForSingleObject sẽ chặn cho đến khi mutex khả dụng
        DWORD waitResult = WaitForSingleObject(g_hMutex, INFINITE);

        if (waitResult == WAIT_OBJECT_0) {
            // ================================================================
            // CRITICAL SECTION - Only ONE thread executes this at a time
            // VÙNG TỚI HẠN - Chỉ MỘT luồng thực thi phần này tại một thời điểm
            // ================================================================
            g_safeCounter++;

            // ================================================================
            // Release the mutex (exit critical section)
            // Nhả mutex (thoát vùng tới hạn)
            // ================================================================
            ReleaseMutex(g_hMutex);
        } else {
            printf("  [Safe Thread #%d] WaitForSingleObject failed!\n",
                   threadIndex);
        }
    }

    printf("  [Safe Thread #%d] Done. Counter seen = %ld\n",
           threadIndex, g_safeCounter);
    return 0;
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 03: Thread Synchronization with Mutex\n");
    printf("  Bai tap 03: Dong bo luong voi Mutex\n");
    printf("============================================================\n\n");

    HANDLE hThreads[NUM_THREADS];
    DWORD  threadIds[NUM_THREADS];
    int    threadIndices[NUM_THREADS];  // To pass unique index to each thread

    long expectedValue = (long)NUM_THREADS * INCREMENTS_EACH;

    printf("Configuration:\n");
    printf("  Threads: %d\n", NUM_THREADS);
    printf("  Increments per thread: %d\n", INCREMENTS_EACH);
    printf("  Expected final value: %ld\n\n", expectedValue);

    // ========================================================================
    // PART 1: Demonstrate RACE CONDITION (no synchronization)
    // PHẦN 1: Minh họa RACE CONDITION (không đồng bộ)
    // ========================================================================
    printf("=== PART 1: WITHOUT Mutex (Race Condition) ===\n");
    printf("=== PHAN 1: KHONG co Mutex (Dieu kien tranh chap) ===\n\n");

    g_unsafeCounter = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        threadIndices[i] = i;
        hThreads[i] = CreateThread(NULL, 0, UnsafeIncrementThread,
                                   &threadIndices[i], 0, &threadIds[i]);
        if (hThreads[i] == NULL) {
            printf("[ERROR] Failed to create thread %d\n", i);
            return 1;
        }
    }

    WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);

    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(hThreads[i]);
    }

    printf("\n  [RESULT] Expected: %ld\n", expectedValue);
    printf("  [RESULT] Actual:   %ld\n", g_unsafeCounter);

    long lost = expectedValue - g_unsafeCounter;
    if (lost > 0) {
        printf("  [RESULT] LOST %ld increments due to race condition!\n", lost);
        printf("           MAT %ld lan tang do dieu kien tranh chap!\n", lost);
    } else {
        printf("  [RESULT] Got lucky this time - but it's still unsafe!\n");
        printf("           May man lan nay - nhung van khong an toan!\n");
    }

    // ========================================================================
    // PART 2: FIX with Mutex (proper synchronization)
    // PHẦN 2: SỬA bằng Mutex (đồng bộ đúng cách)
    // ========================================================================
    printf("\n=== PART 2: WITH Mutex (Proper Synchronization) ===\n");
    printf("=== PHAN 2: CO Mutex (Dong bo dung cach) ===\n\n");

    g_safeCounter = 0;

    // ========================================================================
    // CreateMutex() - Create a mutex object
    // CreateMutex() - Tạo đối tượng mutex
    //
    // HANDLE CreateMutex(
    //   LPSECURITY_ATTRIBUTES lpMutexAttributes,  // Security / Bảo mật
    //   BOOL                  bInitialOwner,       // TRUE = creating thread
    //                                              // owns it immediately
    //                                              // TRUE = luồng tạo sở hữu
    //                                              // ngay lập tức
    //   LPCSTR                lpName               // Name (for cross-process)
    //                                              // Tên (dùng cho liên tiến trình)
    //                                              // NULL = unnamed / không tên
    // );
    // ========================================================================

    g_hMutex = CreateMutex(
        NULL,     // Default security / Bảo mật mặc định
        FALSE,    // No initial owner / Không ai sở hữu ban đầu
        NULL      // Unnamed mutex (process-local only)
                  // Mutex không tên (chỉ dùng trong tiến trình)
    );

    if (g_hMutex == NULL) {
        printf("[ERROR] CreateMutex failed! Error: %lu\n", GetLastError());
        return 1;
    }

    printf("  Mutex created successfully!\n\n");

    // Create threads that use the mutex
    // Tạo luồng sử dụng mutex
    for (int i = 0; i < NUM_THREADS; i++) {
        threadIndices[i] = i;
        hThreads[i] = CreateThread(NULL, 0, SafeIncrementThread,
                                   &threadIndices[i], 0, &threadIds[i]);
        if (hThreads[i] == NULL) {
            printf("[ERROR] Failed to create thread %d\n", i);
            CloseHandle(g_hMutex);
            return 1;
        }
    }

    printf("  (This will be slower because of mutex overhead...)\n");
    printf("  (Se cham hon vi chi phi mutex...)\n\n");

    DWORD startTime = GetTickCount();
    WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);
    DWORD elapsed = GetTickCount() - startTime;

    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(hThreads[i]);
    }

    // Close the mutex handle / Đóng handle mutex
    CloseHandle(g_hMutex);

    printf("\n  [RESULT] Expected: %ld\n", expectedValue);
    printf("  [RESULT] Actual:   %ld\n", g_safeCounter);
    printf("  [RESULT] Time: %lu ms\n", elapsed);

    if (g_safeCounter == expectedValue) {
        printf("  [RESULT] PERFECT! No lost increments!\n");
        printf("           HOAN HAO! Khong mat lan tang nao!\n");
    }

    // ========================================================================
    // COMPARISON / SO SÁNH
    // ========================================================================
    printf("\n============================================================\n");
    printf("  Comparison / So sanh:\n");
    printf("------------------------------------------------------------\n");
    printf("  Without Mutex: counter = %ld (expected %ld)\n",
           g_unsafeCounter, expectedValue);
    printf("  With Mutex:    counter = %ld (expected %ld)\n",
           g_safeCounter, expectedValue);
    printf("------------------------------------------------------------\n");
    printf("\n");
    printf("  Key points / Diem chinh:\n");
    printf("  - Mutex ensures mutual exclusion (only 1 thread at a time)\n");
    printf("    Mutex dam bao loai tru lan nhau (chi 1 luong tai 1 thoi diem)\n");
    printf("  - WaitForSingleObject = lock/acquire\n");
    printf("  - ReleaseMutex = unlock/release\n");
    printf("  - Mutex can be used across processes (with a name)\n");
    printf("    Mutex co the dung lien tien trinh (voi ten)\n");
    printf("  - Mutex is SLOWER than CriticalSection (see Exercise 04)\n");
    printf("    Mutex CHAM hon CriticalSection (xem Bai tap 04)\n");
    printf("============================================================\n");

    return 0;
}
