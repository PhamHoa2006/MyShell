/*
 * ============================================================================
 * Exercise 04: Thread Synchronization with Critical Section
 * Bai tap 04: Dong bo luong voi Critical Section
 * ============================================================================
 * Description (Mô tả):
 *   Solves the same race condition problem from Exercise 03, but uses
 *   CRITICAL_SECTION instead of Mutex. Compares performance and explains
 *   when to use each approach.
 *
 *   Giải quyết cùng vấn đề race condition từ Bài tập 03, nhưng dùng
 *   CRITICAL_SECTION thay vì Mutex. So sánh hiệu năng và giải thích
 *   khi nào dùng mỗi cách.
 *
 * Key APIs (Các API chính):
 *   - InitializeCriticalSection()   : Initialize / Khởi tạo
 *   - EnterCriticalSection()        : Enter (lock) / Vào (khóa)
 *   - LeaveCriticalSection()        : Leave (unlock) / Ra (mở khóa)
 *   - DeleteCriticalSection()       : Destroy / Hủy
 *   - TryEnterCriticalSection()     : Try to enter (non-blocking) / Thử vào
 *
 * Compile (Biên dịch):
 *   g++ 04_thread_sync_critical_section.cpp -o 04_thread_sync_critical_section.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

#define NUM_THREADS       4
#define INCREMENTS_EACH   100000

// ============================================================================
// Shared data / Dữ liệu chia sẻ
// ============================================================================
volatile long g_counterMutex = 0;
volatile long g_counterCS    = 0;

HANDLE           g_hMutex = NULL;
CRITICAL_SECTION g_cs;          // Not a HANDLE - it's a struct!
                                // Không phải HANDLE - nó là struct!

// ============================================================================
// CRITICAL_SECTION vs Mutex - Key Differences
// CRITICAL_SECTION vs Mutex - Khác biệt chính
// ============================================================================
//
// +---------------------------+-------------------+---------------------+
// | Feature / Tính năng       | CRITICAL_SECTION  | Mutex               |
// +---------------------------+-------------------+---------------------+
// | Speed / Tốc độ            | FAST (user-mode)  | SLOW (kernel-mode)  |
// |                           | NHANH (chế độ     | CHẬM (chế độ        |
// |                           | người dùng)       | nhân)               |
// +---------------------------+-------------------+---------------------+
// | Cross-process /           | NO                | YES (with name)     |
// | Liên tiến trình           | KHÔNG              | CÓ (có tên)         |
// +---------------------------+-------------------+---------------------+
// | Timeout / Hết giờ         | NO (blocks        | YES (via            |
// |                           | forever)          | WaitForSingleObject)|
// |                           | KHÔNG (chặn mãi)  | CÓ                  |
// +---------------------------+-------------------+---------------------+
// | Type / Kiểu               | Struct (stack/    | HANDLE (kernel      |
// |                           | global)           | object)             |
// +---------------------------+-------------------+---------------------+
// | Recursive / Đệ quy        | YES (same thread  | YES (same thread    |
// |                           | can enter again)  | can acquire again)  |
// +---------------------------+-------------------+---------------------+
//
// RULE OF THUMB / QUY TẮC CHUNG:
//   - Same process only? Use CRITICAL_SECTION (faster)
//     Chỉ cùng tiến trình? Dùng CRITICAL_SECTION (nhanh hơn)
//   - Need cross-process sync? Use Mutex
//     Cần đồng bộ liên tiến trình? Dùng Mutex
//   - Need timeout? Use Mutex
//     Cần timeout? Dùng Mutex
// ============================================================================

// ============================================================================
// Thread function using Mutex / Hàm luồng dùng Mutex
// ============================================================================
DWORD WINAPI MutexThread(LPVOID lpParameter) {
    int idx = *(int*)lpParameter;

    for (int i = 0; i < INCREMENTS_EACH; i++) {
        WaitForSingleObject(g_hMutex, INFINITE);  // Lock / Khóa
        g_counterMutex++;
        ReleaseMutex(g_hMutex);                   // Unlock / Mở khóa
    }

    printf("  [Mutex Thread #%d] Done.\n", idx);
    return 0;
}

// ============================================================================
// Thread function using Critical Section / Hàm luồng dùng Critical Section
// ============================================================================
DWORD WINAPI CriticalSectionThread(LPVOID lpParameter) {
    int idx = *(int*)lpParameter;

    for (int i = 0; i < INCREMENTS_EACH; i++) {
        // ====================================================================
        // EnterCriticalSection() - Acquire the lock
        // EnterCriticalSection() - Chiếm khóa
        //
        // This function BLOCKS until the critical section is available.
        // Unlike WaitForSingleObject, there is NO timeout option.
        //
        // Hàm này CHẶN cho đến khi vùng tới hạn khả dụng.
        // Không giống WaitForSingleObject, KHÔNG CÓ tùy chọn timeout.
        //
        // Under the hood, it uses a spin-lock first (fast, user-mode),
        // then falls back to a kernel wait if contention is high.
        //
        // Bên trong, nó dùng spin-lock trước (nhanh, chế độ người dùng),
        // rồi chuyển sang chờ kernel nếu tranh chấp cao.
        // ====================================================================
        EnterCriticalSection(&g_cs);

        // CRITICAL SECTION - only one thread at a time
        // VÙNG TỚI HẠN - chỉ một luồng tại một thời điểm
        g_counterCS++;

        // ====================================================================
        // LeaveCriticalSection() - Release the lock
        // LeaveCriticalSection() - Nhả khóa
        //
        // MUST be called from the SAME THREAD that called Enter!
        // PHẢI được gọi từ CÙNG LUỒNG đã gọi Enter!
        // ====================================================================
        LeaveCriticalSection(&g_cs);
    }

    printf("  [CS Thread #%d] Done.\n", idx);
    return 0;
}

// ============================================================================
// Demonstrate TryEnterCriticalSection / Minh họa TryEnterCriticalSection
// ============================================================================
DWORD WINAPI TryEnterThread(LPVOID lpParameter) {
    int idx = *(int*)lpParameter;
    int succeeded = 0;
    int failed    = 0;

    for (int i = 0; i < 1000; i++) {
        // ====================================================================
        // TryEnterCriticalSection() - Non-blocking attempt to acquire
        // TryEnterCriticalSection() - Thử chiếm không chặn
        //
        // Returns TRUE if acquired, FALSE if already owned by another thread.
        // Does NOT block! Useful for "check and skip" patterns.
        //
        // Trả về TRUE nếu chiếm được, FALSE nếu đã bị luồng khác sở hữu.
        // KHÔNG chặn! Hữu ích cho mẫu "kiểm tra và bỏ qua".
        // ====================================================================
        if (TryEnterCriticalSection(&g_cs)) {
            // Got it! Do work / Chiếm được! Thực hiện công việc
            g_counterCS++;
            succeeded++;
            LeaveCriticalSection(&g_cs);
        } else {
            // Couldn't get it - do something else
            // Không chiếm được - làm việc khác
            failed++;
        }
    }

    printf("  [TryEnter Thread #%d] Succeeded: %d, Failed: %d\n",
           idx, succeeded, failed);
    return 0;
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 04: Critical Section vs Mutex\n");
    printf("  Bai tap 04: Critical Section vs Mutex\n");
    printf("============================================================\n\n");

    HANDLE hThreads[NUM_THREADS];
    DWORD  threadIds[NUM_THREADS];
    int    indices[NUM_THREADS];

    long expectedValue = (long)NUM_THREADS * INCREMENTS_EACH;

    printf("Configuration: %d threads x %d increments = %ld expected\n\n",
           NUM_THREADS, INCREMENTS_EACH, expectedValue);

    // ========================================================================
    // PART 1: Mutex performance benchmark
    // PHẦN 1: Đo hiệu năng Mutex
    // ========================================================================
    printf("=== PART 1: Mutex Performance ===\n\n");

    g_counterMutex = 0;
    g_hMutex = CreateMutex(NULL, FALSE, NULL);
    if (g_hMutex == NULL) {
        printf("[ERROR] CreateMutex failed!\n");
        return 1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        indices[i] = i;
        hThreads[i] = CreateThread(NULL, 0, MutexThread,
                                   &indices[i], 0, &threadIds[i]);
        if (hThreads[i] == NULL) {
            printf("[ERROR] CreateThread failed!\n");
            return 1;
        }
    }

    DWORD startTime = GetTickCount();
    WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);
    DWORD mutexTime = GetTickCount() - startTime;

    for (int i = 0; i < NUM_THREADS; i++)
        CloseHandle(hThreads[i]);
    CloseHandle(g_hMutex);

    printf("\n  Mutex result: %ld (expected %ld) - Time: %lu ms\n\n",
           g_counterMutex, expectedValue, mutexTime);

    // ========================================================================
    // PART 2: Critical Section performance benchmark
    // PHẦN 2: Đo hiệu năng Critical Section
    // ========================================================================
    printf("=== PART 2: Critical Section Performance ===\n\n");

    g_counterCS = 0;

    // ========================================================================
    // InitializeCriticalSection() - Must be called before use!
    // InitializeCriticalSection() - Phải gọi trước khi sử dụng!
    //
    // Unlike CreateMutex (which returns a HANDLE), this initializes
    // a CRITICAL_SECTION struct in-place. No handle is returned.
    //
    // Không giống CreateMutex (trả về HANDLE), hàm này khởi tạo
    // struct CRITICAL_SECTION tại chỗ. Không trả về handle.
    // ========================================================================
    InitializeCriticalSection(&g_cs);

    for (int i = 0; i < NUM_THREADS; i++) {
        indices[i] = i;
        hThreads[i] = CreateThread(NULL, 0, CriticalSectionThread,
                                   &indices[i], 0, &threadIds[i]);
        if (hThreads[i] == NULL) {
            printf("[ERROR] CreateThread failed!\n");
            DeleteCriticalSection(&g_cs);
            return 1;
        }
    }

    startTime = GetTickCount();
    WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);
    DWORD csTime = GetTickCount() - startTime;

    for (int i = 0; i < NUM_THREADS; i++)
        CloseHandle(hThreads[i]);

    printf("\n  CritSec result: %ld (expected %ld) - Time: %lu ms\n\n",
           g_counterCS, expectedValue, csTime);

    // ========================================================================
    // PART 3: TryEnterCriticalSection demonstration
    // PHẦN 3: Minh họa TryEnterCriticalSection
    // ========================================================================
    printf("=== PART 3: TryEnterCriticalSection Demo ===\n\n");

    g_counterCS = 0;
    // g_cs is already initialized / g_cs đã được khởi tạo

    for (int i = 0; i < NUM_THREADS; i++) {
        indices[i] = i;
        hThreads[i] = CreateThread(NULL, 0, TryEnterThread,
                                   &indices[i], 0, &threadIds[i]);
        if (hThreads[i] == NULL) {
            printf("[ERROR] CreateThread failed!\n");
            break;
        }
    }

    WaitForMultipleObjects(NUM_THREADS, hThreads, TRUE, INFINITE);

    for (int i = 0; i < NUM_THREADS; i++)
        CloseHandle(hThreads[i]);

    printf("\n  TryEnter result: %ld (some increments may be skipped)\n",
           g_counterCS);
    printf("  Ket qua TryEnter: %ld (mot so lan tang co the bi bo qua)\n",
           g_counterCS);

    // ========================================================================
    // DeleteCriticalSection() - Must be called when done!
    // DeleteCriticalSection() - Phải gọi khi hoàn tất!
    //
    // Releases system resources associated with the critical section.
    // Giải phóng tài nguyên hệ thống liên quan đến vùng tới hạn.
    //
    // Do NOT delete while any thread might still be using it!
    // KHÔNG xóa khi bất kỳ luồng nào vẫn đang sử dụng!
    // ========================================================================
    DeleteCriticalSection(&g_cs);

    // ========================================================================
    // Performance comparison / So sánh hiệu năng
    // ========================================================================
    printf("\n============================================================\n");
    printf("  Performance Comparison / So sanh hieu nang:\n");
    printf("------------------------------------------------------------\n");
    printf("  Mutex:            %lu ms\n", mutexTime);
    printf("  Critical Section: %lu ms\n", csTime);

    if (csTime > 0 && mutexTime > 0) {
        double speedup = (double)mutexTime / (double)csTime;
        printf("  Speedup:          %.1fx faster with CriticalSection\n",
               speedup);
        printf("                    %.1fx nhanh hon voi CriticalSection\n",
               speedup);
    }

    printf("------------------------------------------------------------\n");
    printf("\n  When to use what / Khi nao dung cai nao:\n\n");
    printf("  CRITICAL_SECTION:\n");
    printf("    + Faster (user-mode spin-lock) / Nhanh hon\n");
    printf("    + Simpler API / API don gian hon\n");
    printf("    - Only within same process / Chi trong cung tien trinh\n");
    printf("    - No timeout support / Khong ho tro timeout\n\n");
    printf("  Mutex:\n");
    printf("    + Works across processes / Hoat dong lien tien trinh\n");
    printf("    + Supports timeout / Ho tro timeout\n");
    printf("    + Can be named / Co the dat ten\n");
    printf("    - Slower (kernel mode) / Cham hon (che do nhan)\n");
    printf("============================================================\n");

    return 0;
}
