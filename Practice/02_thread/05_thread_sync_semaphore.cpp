/*
 * ============================================================================
 * Exercise 05: Thread Synchronization with Semaphore (Producer-Consumer)
 * Bai tap 05: Dong bo luong voi Semaphore (Nha san xuat - Nha tieu thu)
 * ============================================================================
 * Description (Mô tả):
 *   Implements the classic Producer-Consumer problem using Windows Semaphores.
 *   Multiple producers add items to a bounded buffer, and multiple consumers
 *   remove items. Semaphores coordinate so producers don't overflow and
 *   consumers don't underflow the buffer.
 *
 *   Cài đặt bài toán cổ điển Nhà sản xuất - Nhà tiêu thụ bằng Semaphore
 *   Windows. Nhiều nhà sản xuất thêm mục vào bộ đệm có giới hạn, và nhiều
 *   nhà tiêu thụ lấy mục ra. Semaphore phối hợp để nhà sản xuất không tràn
 *   và nhà tiêu thụ không thiếu bộ đệm.
 *
 * Key APIs (Các API chính):
 *   - CreateSemaphore()       : Create semaphore / Tạo semaphore
 *   - WaitForSingleObject()   : Decrement (wait/P operation) / Giảm (thao tác chờ/P)
 *   - ReleaseSemaphore()      : Increment (signal/V operation) / Tăng (thao tác báo/V)
 *   - CreateMutex()           : Protect shared buffer / Bảo vệ bộ đệm chia sẻ
 *
 * Compile (Biên dịch):
 *   g++ 05_thread_sync_semaphore.cpp -o 05_thread_sync_semaphore.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Configuration / Cấu hình
// ============================================================================
#define BUFFER_SIZE       5      // Bounded buffer size / Kích thước bộ đệm
#define NUM_PRODUCERS     2      // Number of producer threads / Số luồng sản xuất
#define NUM_CONSUMERS     2      // Number of consumer threads / Số luồng tiêu thụ
#define ITEMS_PER_PROD    8      // Items each producer creates / Mục mỗi SX tạo
#define TOTAL_ITEMS       (NUM_PRODUCERS * ITEMS_PER_PROD)  // Total items

// ============================================================================
// SEMAPHORE CONCEPT / KHÁI NIỆM SEMAPHORE
// ============================================================================
//
// A semaphore is a counter that controls access to a limited resource.
// Semaphore là bộ đếm kiểm soát truy cập vào tài nguyên giới hạn.
//
// Unlike a Mutex (binary: locked/unlocked), a Semaphore can allow N
// threads to access a resource simultaneously.
// Khác với Mutex (nhị phân: khóa/mở), Semaphore cho phép N luồng
// truy cập tài nguyên đồng thời.
//
// Operations / Các thao tác:
//   Wait (P / Down / Acquire):
//     - If count > 0: decrement and continue
//       Nếu count > 0: giảm và tiếp tục
//     - If count == 0: BLOCK until someone signals
//       Nếu count == 0: CHẶN cho đến khi ai đó báo hiệu
//
//   Signal (V / Up / Release):
//     - Increment the count (possibly unblocking a waiter)
//       Tăng count (có thể mở chặn một luồng đang chờ)
//
// PRODUCER-CONSUMER PATTERN / MẪU NHÀ SẢN XUẤT - NHÀ TIÊU THỤ:
//
//   emptySlots semaphore: counts empty buffer slots (starts at BUFFER_SIZE)
//     Semaphore chỗ trống: đếm slot trống (bắt đầu = BUFFER_SIZE)
//   fullSlots  semaphore: counts filled buffer slots (starts at 0)
//     Semaphore chỗ đầy: đếm slot đã điền (bắt đầu = 0)
//
//   Producer:                       Consumer:
//     Wait(emptySlots)  // need space   Wait(fullSlots)   // need item
//     Lock(mutex)                       Lock(mutex)
//     Add item to buffer                Remove item from buffer
//     Unlock(mutex)                     Unlock(mutex)
//     Signal(fullSlots)  // added one   Signal(emptySlots) // freed one
// ============================================================================

// ============================================================================
// Bounded buffer (circular queue) / Bộ đệm có giới hạn (hàng đợi vòng)
// ============================================================================
struct BoundedBuffer {
    int  items[BUFFER_SIZE];  // The buffer / Bộ đệm
    int  in;                  // Next write position / Vị trí ghi tiếp theo
    int  out;                 // Next read position / Vị trí đọc tiếp theo
    int  count;               // Current item count / Số mục hiện tại
};

BoundedBuffer g_buffer;

// Synchronization objects / Các đối tượng đồng bộ
HANDLE g_hEmptySlots;    // Semaphore: counts empty slots / Đếm chỗ trống
HANDLE g_hFullSlots;     // Semaphore: counts full slots / Đếm chỗ đầy
HANDLE g_hMutex;         // Mutex: protects buffer access / Bảo vệ truy cập bộ đệm

// Tracking / Theo dõi
volatile long g_totalProduced = 0;
volatile long g_totalConsumed = 0;

// ============================================================================
// Helper: Print buffer state / In trạng thái bộ đệm
// ============================================================================
void PrintBufferState(const char* action, int threadType, int threadId, int item) {
    printf("  [%s #%d] %s item=%d | Buffer[%d/%d]: [",
           (threadType == 0) ? "PRODUCER" : "CONSUMER",
           threadId, action, item, g_buffer.count, BUFFER_SIZE);

    // Visual representation of buffer / Biểu diễn trực quan bộ đệm
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int pos = (g_buffer.out + i) % BUFFER_SIZE;
        if (i < g_buffer.count) {
            printf(" %2d", g_buffer.items[pos]);
        } else {
            printf("  _");
        }
    }
    printf(" ]\n");
}

// ============================================================================
// Producer thread / Luồng nhà sản xuất
// ============================================================================
DWORD WINAPI ProducerThread(LPVOID lpParameter) {
    int producerId = *(int*)lpParameter;

    printf("[Producer #%d | TID=%lu] Started\n",
           producerId, GetCurrentThreadId());

    for (int i = 0; i < ITEMS_PER_PROD; i++) {
        int item = producerId * 100 + i;  // Unique item ID

        // ====================================================================
        // STEP 1: Wait for an empty slot (P operation on emptySlots)
        // BƯỚC 1: Chờ một chỗ trống (thao tác P trên emptySlots)
        //
        // If buffer is full (emptySlots count == 0), this BLOCKS
        // until a consumer removes an item and signals emptySlots.
        //
        // Nếu bộ đệm đầy (emptySlots count == 0), hàm này CHẶN
        // cho đến khi nhà tiêu thụ lấy mục ra và báo hiệu emptySlots.
        // ====================================================================
        WaitForSingleObject(g_hEmptySlots, INFINITE);

        // ====================================================================
        // STEP 2: Lock the buffer (mutual exclusion)
        // BƯỚC 2: Khóa bộ đệm (loại trừ lẫn nhau)
        // ====================================================================
        WaitForSingleObject(g_hMutex, INFINITE);

        // CRITICAL SECTION: Add item to buffer
        // VÙNG TỚI HẠN: Thêm mục vào bộ đệm
        g_buffer.items[g_buffer.in] = item;
        g_buffer.in = (g_buffer.in + 1) % BUFFER_SIZE;  // Circular / Vòng
        g_buffer.count++;
        g_totalProduced++;

        PrintBufferState("PRODUCED", 0, producerId, item);

        // Unlock / Mở khóa
        ReleaseMutex(g_hMutex);

        // ====================================================================
        // STEP 3: Signal that a slot is now full (V operation on fullSlots)
        // BƯỚC 3: Báo hiệu rằng một chỗ đã đầy (thao tác V trên fullSlots)
        //
        // This increments fullSlots count, potentially waking up
        // a consumer that was waiting for items.
        //
        // Điều này tăng count của fullSlots, có thể đánh thức
        // nhà tiêu thụ đang chờ mục.
        // ====================================================================
        ReleaseSemaphore(g_hFullSlots, 1, NULL);

        // Simulate production time / Giả lập thời gian sản xuất
        Sleep(100 + (producerId * 50));
    }

    printf("[Producer #%d] Finished producing %d items.\n",
           producerId, ITEMS_PER_PROD);
    return 0;
}

// ============================================================================
// Consumer thread / Luồng nhà tiêu thụ
// ============================================================================
DWORD WINAPI ConsumerThread(LPVOID lpParameter) {
    int consumerId = *(int*)lpParameter;

    printf("[Consumer #%d | TID=%lu] Started\n",
           consumerId, GetCurrentThreadId());

    while (1) {
        // ====================================================================
        // STEP 1: Wait for a full slot (P operation on fullSlots)
        // BƯỚC 1: Chờ một chỗ đầy (thao tác P trên fullSlots)
        //
        // If buffer is empty (fullSlots count == 0), this BLOCKS
        // until a producer adds an item and signals fullSlots.
        //
        // Nếu bộ đệm trống (fullSlots count == 0), hàm này CHẶN
        // cho đến khi nhà sản xuất thêm mục và báo hiệu fullSlots.
        // ====================================================================
        DWORD result = WaitForSingleObject(g_hFullSlots, 2000);
        if (result == WAIT_TIMEOUT) {
            // No items for 2 seconds - check if production is done
            // Không có mục trong 2 giây - kiểm tra sản xuất đã xong chưa
            if (g_totalProduced >= TOTAL_ITEMS && g_buffer.count == 0) {
                printf("[Consumer #%d] No more items. Exiting.\n", consumerId);
                break;
            }
            continue;
        }

        // ====================================================================
        // STEP 2: Lock the buffer / Khóa bộ đệm
        // ====================================================================
        WaitForSingleObject(g_hMutex, INFINITE);

        // CRITICAL SECTION: Remove item from buffer
        // VÙNG TỚI HẠN: Lấy mục ra khỏi bộ đệm
        int item = g_buffer.items[g_buffer.out];
        g_buffer.out = (g_buffer.out + 1) % BUFFER_SIZE;
        g_buffer.count--;
        g_totalConsumed++;

        long consumed = g_totalConsumed;

        PrintBufferState("CONSUMED", 1, consumerId, item);

        // Unlock / Mở khóa
        ReleaseMutex(g_hMutex);

        // ====================================================================
        // STEP 3: Signal that a slot is now empty (V operation on emptySlots)
        // BƯỚC 3: Báo hiệu rằng một chỗ đã trống (thao tác V trên emptySlots)
        // ====================================================================
        ReleaseSemaphore(g_hEmptySlots, 1, NULL);

        // Simulate consumption time / Giả lập thời gian tiêu thụ
        Sleep(200 + (consumerId * 30));

        // Check if we've consumed everything / Kiểm tra đã tiêu thụ hết chưa
        if (consumed >= TOTAL_ITEMS) {
            printf("[Consumer #%d] All items consumed. Exiting.\n", consumerId);
            break;
        }
    }

    return 0;
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 05: Producer-Consumer with Semaphore\n");
    printf("  Bai tap 05: Nha san xuat - Nha tieu thu voi Semaphore\n");
    printf("============================================================\n\n");

    printf("Configuration:\n");
    printf("  Buffer size: %d\n", BUFFER_SIZE);
    printf("  Producers: %d (each produces %d items)\n",
           NUM_PRODUCERS, ITEMS_PER_PROD);
    printf("  Consumers: %d\n", NUM_CONSUMERS);
    printf("  Total items: %d\n\n", TOTAL_ITEMS);

    // Initialize buffer / Khởi tạo bộ đệm
    g_buffer.in    = 0;
    g_buffer.out   = 0;
    g_buffer.count = 0;

    // ========================================================================
    // CreateSemaphore() - Create a semaphore object
    // CreateSemaphore() - Tạo đối tượng semaphore
    //
    // HANDLE CreateSemaphore(
    //   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  // Security
    //   LONG                  lInitialCount,           // Initial count value
    //                         // Giá trị count ban đầu
    //   LONG                  lMaximumCount,           // Maximum count value
    //                         // Giá trị count tối đa
    //   LPCSTR                lpName                   // Name (cross-process)
    //                         // Tên (dùng liên tiến trình)
    // );
    //
    // Rules / Quy tắc:
    //   0 <= lInitialCount <= lMaximumCount
    //   WaitForSingleObject decrements count (blocks if count == 0)
    //   ReleaseSemaphore increments count (up to lMaximumCount)
    // ========================================================================

    // emptySlots: starts at BUFFER_SIZE (all slots are empty initially)
    // emptySlots: bắt đầu = BUFFER_SIZE (tất cả slot đều trống ban đầu)
    g_hEmptySlots = CreateSemaphore(
        NULL,           // Default security
        BUFFER_SIZE,    // Initial count = buffer size (all empty)
                        // Count ban đầu = kích thước bộ đệm (tất cả trống)
        BUFFER_SIZE,    // Max count = buffer size
        NULL            // Unnamed
    );

    if (g_hEmptySlots == NULL) {
        printf("[ERROR] CreateSemaphore (emptySlots) failed! Error: %lu\n",
               GetLastError());
        return 1;
    }

    // fullSlots: starts at 0 (no items in buffer initially)
    // fullSlots: bắt đầu = 0 (không có mục trong bộ đệm ban đầu)
    g_hFullSlots = CreateSemaphore(
        NULL,           // Default security
        0,              // Initial count = 0 (no items yet)
                        // Count ban đầu = 0 (chưa có mục nào)
        BUFFER_SIZE,    // Max count = buffer size
        NULL            // Unnamed
    );

    if (g_hFullSlots == NULL) {
        printf("[ERROR] CreateSemaphore (fullSlots) failed! Error: %lu\n",
               GetLastError());
        CloseHandle(g_hEmptySlots);
        return 1;
    }

    // Mutex for buffer access / Mutex để truy cập bộ đệm
    g_hMutex = CreateMutex(NULL, FALSE, NULL);
    if (g_hMutex == NULL) {
        printf("[ERROR] CreateMutex failed! Error: %lu\n", GetLastError());
        CloseHandle(g_hEmptySlots);
        CloseHandle(g_hFullSlots);
        return 1;
    }

    printf("Semaphores and Mutex created.\n");
    printf("  emptySlots initial count: %d\n", BUFFER_SIZE);
    printf("  fullSlots  initial count: 0\n\n");

    printf("--- Starting Producer-Consumer ---\n\n");

    // Create all threads / Tạo tất cả luồng
    HANDLE hThreads[NUM_PRODUCERS + NUM_CONSUMERS];
    int    producerIds[NUM_PRODUCERS];
    int    consumerIds[NUM_CONSUMERS];
    int    threadCount = 0;

    // Create consumers first (they'll wait for items)
    // Tạo nhà tiêu thụ trước (họ sẽ chờ mục)
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumerIds[i] = i;
        hThreads[threadCount] = CreateThread(NULL, 0, ConsumerThread,
                                             &consumerIds[i], 0, NULL);
        if (hThreads[threadCount] == NULL) {
            printf("[ERROR] Failed to create consumer %d\n", i);
            return 1;
        }
        threadCount++;
    }

    // Create producers / Tạo nhà sản xuất
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producerIds[i] = i;
        hThreads[threadCount] = CreateThread(NULL, 0, ProducerThread,
                                             &producerIds[i], 0, NULL);
        if (hThreads[threadCount] == NULL) {
            printf("[ERROR] Failed to create producer %d\n", i);
            return 1;
        }
        threadCount++;
    }

    // Wait for all threads / Chờ tất cả luồng
    WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);

    // Cleanup / Dọn dẹp
    for (int i = 0; i < threadCount; i++)
        CloseHandle(hThreads[i]);

    CloseHandle(g_hEmptySlots);
    CloseHandle(g_hFullSlots);
    CloseHandle(g_hMutex);

    // ========================================================================
    // Results / Kết quả
    // ========================================================================
    printf("\n============================================================\n");
    printf("  Results / Ket qua:\n");
    printf("------------------------------------------------------------\n");
    printf("  Total produced: %ld\n", g_totalProduced);
    printf("  Total consumed: %ld\n", g_totalConsumed);
    printf("  Buffer remaining: %d\n", g_buffer.count);

    if (g_totalProduced == g_totalConsumed &&
        g_totalConsumed == TOTAL_ITEMS) {
        printf("\n  SUCCESS! All items produced and consumed correctly.\n");
        printf("  THANH CONG! Tat ca muc san xuat va tieu thu dung.\n");
    }

    printf("------------------------------------------------------------\n");
    printf("\n  Semaphore key points / Diem chinh Semaphore:\n");
    printf("  - Counting semaphore allows N concurrent accesses\n");
    printf("    Semaphore dem cho phep N truy cap dong thoi\n");
    printf("  - emptySlots prevents buffer overflow\n");
    printf("    emptySlots ngan tran bo dem\n");
    printf("  - fullSlots prevents buffer underflow\n");
    printf("    fullSlots ngan thieu bo dem\n");
    printf("  - Mutex still needed for mutual exclusion on buffer\n");
    printf("    Van can Mutex de loai tru lan nhau tren bo dem\n");
    printf("============================================================\n");

    return 0;
}
