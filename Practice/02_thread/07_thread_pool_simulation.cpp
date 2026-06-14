/*
 * ============================================================================
 * Exercise 07: Thread Pool Simulation
 * Bai tap 07: Mo phong Thread Pool
 * ============================================================================
 * Description (Mô tả):
 *   Simulates a thread pool: N worker threads are created upfront and wait
 *   for tasks from a shared queue. The main thread submits tasks, and worker
 *   threads pick them up. Uses Mutex for queue protection and Event for
 *   signaling task availability.
 *
 *   Mô phỏng thread pool: N luồng công nhân được tạo sẵn và chờ công việc
 *   từ hàng đợi chia sẻ. Luồng chính gửi công việc, và luồng công nhân
 *   nhận chúng. Dùng Mutex bảo vệ hàng đợi và Event báo hiệu có công việc.
 *
 * Key APIs (Các API chính):
 *   - CreateEvent()           : Create an event object / Tạo đối tượng event
 *   - SetEvent()              : Signal the event / Báo hiệu event
 *   - ResetEvent()            : Reset the event / Đặt lại event
 *   - WaitForSingleObject()   : Wait for event/mutex / Chờ event/mutex
 *   - CreateMutex()           : Protect shared queue / Bảo vệ hàng đợi
 *
 * Compile (Biên dịch):
 *   g++ 07_thread_pool_simulation.cpp -o 07_thread_pool_simulation.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Configuration / Cấu hình
// ============================================================================
#define NUM_WORKERS     3       // Number of worker threads / Số luồng công nhân
#define MAX_QUEUE_SIZE  20      // Maximum tasks in queue / Số công việc tối đa trong hàng đợi
#define TOTAL_TASKS     12      // Total tasks to submit / Tổng công việc cần gửi

// ============================================================================
// EVENT CONCEPT / KHÁI NIỆM EVENT
// ============================================================================
//
// An Event is a synchronization object with two states:
// Event là đối tượng đồng bộ với hai trạng thái:
//   - Signaled (SET):     threads waiting on it are released
//     Báo hiệu (SET):    các luồng chờ nó được giải phóng
//   - Non-signaled (RESET): threads waiting on it are blocked
//     Không báo hiệu (RESET): các luồng chờ nó bị chặn
//
// Two types / Hai loại:
//
//   Manual-Reset Event (bManualReset = TRUE):
//     - Stays signaled until explicitly reset with ResetEvent()
//       Giữ trạng thái báo hiệu cho đến khi đặt lại bằng ResetEvent()
//     - ALL waiting threads are released when signaled
//       TẤT CẢ luồng đang chờ được giải phóng khi báo hiệu
//     - Like opening a gate: everyone can pass through
//       Giống mở cổng: mọi người đều có thể đi qua
//
//   Auto-Reset Event (bManualReset = FALSE):
//     - Automatically resets after releasing ONE waiting thread
//       Tự động đặt lại sau khi giải phóng MỘT luồng đang chờ
//     - Only ONE thread is released per SetEvent() call
//       Chỉ MỘT luồng được giải phóng mỗi lần gọi SetEvent()
//     - Like a turnstile: one person at a time
//       Giống cửa xoay: mỗi lần một người
// ============================================================================

// ============================================================================
// Task structure / Cấu trúc công việc
// ============================================================================
struct Task {
    int   taskId;
    char  description[128];
    int   workDurationMs;    // Simulated work time / Thời gian công việc giả lập
    int   priority;          // For display purposes / Để hiển thị
};

// ============================================================================
// Thread-safe task queue / Hàng đợi công việc an toàn luồng
// ============================================================================
struct TaskQueue {
    Task    tasks[MAX_QUEUE_SIZE];
    int     head;           // Dequeue position / Vị trí lấy ra
    int     tail;           // Enqueue position / Vị trí thêm vào
    int     count;          // Current task count / Số công việc hiện tại
    HANDLE  hMutex;         // Protects queue access / Bảo vệ truy cập hàng đợi
    HANDLE  hTaskAvailable; // Event: signaled when tasks are available
                            // Event: báo hiệu khi có công việc
    HANDLE  hShutdown;      // Event: signaled to stop all workers
                            // Event: báo hiệu để dừng tất cả công nhân
};

TaskQueue g_queue;

// Statistics / Thống kê
volatile long g_tasksCompleted = 0;
volatile long g_tasksByWorker[NUM_WORKERS] = {0};

// ============================================================================
// Initialize the task queue / Khởi tạo hàng đợi công việc
// ============================================================================
BOOL InitTaskQueue(TaskQueue* q) {
    q->head  = 0;
    q->tail  = 0;
    q->count = 0;

    // Create mutex for queue protection
    // Tạo mutex để bảo vệ hàng đợi
    q->hMutex = CreateMutex(NULL, FALSE, NULL);
    if (q->hMutex == NULL) {
        printf("[ERROR] CreateMutex failed!\n");
        return FALSE;
    }

    // ========================================================================
    // CreateEvent() - Create an event synchronization object
    // CreateEvent() - Tạo đối tượng đồng bộ event
    //
    // HANDLE CreateEvent(
    //   LPSECURITY_ATTRIBUTES lpEventAttributes,  // Security / Bảo mật
    //   BOOL                  bManualReset,        // Manual or Auto reset
    //                         // TRUE = manual: stays signaled until ResetEvent()
    //                         //   Giữ báo hiệu cho đến khi ResetEvent()
    //                         // FALSE = auto: resets after releasing one thread
    //                         //   Tự đặt lại sau khi giải phóng một luồng
    //   BOOL                  bInitialState,       // TRUE = start signaled
    //                         // TRUE = bắt đầu ở trạng thái báo hiệu
    //   LPCSTR                lpName               // Name (cross-process)
    //                         // Tên (dùng liên tiến trình)
    // );
    // ========================================================================

    // Auto-reset event for task availability
    // Event tự đặt lại cho tính khả dụng của công việc
    //
    // WHY AUTO-RESET? / TẠI SAO TỰ ĐẶT LẠI?
    // We want exactly ONE worker to wake up per task. If we used manual-reset,
    // ALL workers would wake up and compete for the same task.
    //
    // Chúng ta muốn chính xác MỘT công nhân thức dậy cho mỗi công việc.
    // Nếu dùng manual-reset, TẤT CẢ công nhân sẽ thức và tranh giành
    // cùng một công việc.
    q->hTaskAvailable = CreateEvent(
        NULL,       // Default security
        FALSE,      // AUTO-RESET: resets after one thread is released
                    // TỰ ĐẶT LẠI: đặt lại sau khi một luồng được giải phóng
        FALSE,      // Initial state: non-signaled (no tasks yet)
                    // Trạng thái ban đầu: không báo hiệu (chưa có công việc)
        NULL        // Unnamed
    );

    if (q->hTaskAvailable == NULL) {
        printf("[ERROR] CreateEvent (taskAvailable) failed!\n");
        CloseHandle(q->hMutex);
        return FALSE;
    }

    // Manual-reset event for shutdown signal
    // Event đặt lại thủ công cho tín hiệu tắt
    //
    // WHY MANUAL-RESET? / TẠI SAO ĐẶT LẠI THỦ CÔNG?
    // When shutting down, ALL workers need to see the signal simultaneously.
    // Manual-reset ensures it stays signaled so every worker can detect it.
    //
    // Khi tắt, TẤT CẢ công nhân cần thấy tín hiệu đồng thời.
    // Manual-reset đảm bảo nó giữ báo hiệu để mọi công nhân phát hiện.
    q->hShutdown = CreateEvent(
        NULL,       // Default security
        TRUE,       // MANUAL-RESET: stays signaled until ResetEvent()
                    // ĐẶT LẠI THỦ CÔNG: giữ báo hiệu cho đến khi ResetEvent()
        FALSE,      // Initial state: non-signaled (not shutting down)
        NULL        // Unnamed
    );

    if (q->hShutdown == NULL) {
        printf("[ERROR] CreateEvent (shutdown) failed!\n");
        CloseHandle(q->hMutex);
        CloseHandle(q->hTaskAvailable);
        return FALSE;
    }

    return TRUE;
}

// ============================================================================
// Enqueue a task (called by main thread) / Thêm công việc (gọi bởi luồng chính)
// ============================================================================
BOOL EnqueueTask(TaskQueue* q, Task* task) {
    WaitForSingleObject(q->hMutex, INFINITE);

    if (q->count >= MAX_QUEUE_SIZE) {
        ReleaseMutex(q->hMutex);
        printf("[Queue] FULL! Cannot enqueue task %d\n", task->taskId);
        return FALSE;
    }

    // Copy task into queue / Sao chép công việc vào hàng đợi
    q->tasks[q->tail] = *task;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    q->count++;

    int currentCount = q->count;
    ReleaseMutex(q->hMutex);

    printf("[Queue] Enqueued task #%d: \"%s\" (queue size: %d)\n",
           task->taskId, task->description, currentCount);

    // ========================================================================
    // SetEvent() - Signal the event to wake up a waiting worker
    // SetEvent() - Báo hiệu event để đánh thức một công nhân đang chờ
    //
    // For auto-reset event: wakes up exactly ONE waiting thread.
    // Với event tự đặt lại: đánh thức chính xác MỘT luồng đang chờ.
    // ========================================================================
    SetEvent(q->hTaskAvailable);

    return TRUE;
}

// ============================================================================
// Dequeue a task (called by worker threads) / Lấy công việc (gọi bởi luồng CN)
// ============================================================================
BOOL DequeueTask(TaskQueue* q, Task* outTask) {
    WaitForSingleObject(q->hMutex, INFINITE);

    if (q->count == 0) {
        ReleaseMutex(q->hMutex);
        return FALSE;
    }

    // Copy task out of queue / Sao chép công việc ra khỏi hàng đợi
    *outTask = q->tasks[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->count--;

    ReleaseMutex(q->hMutex);
    return TRUE;
}

// ============================================================================
// Worker thread function / Hàm luồng công nhân
// ============================================================================
DWORD WINAPI WorkerThreadFunc(LPVOID lpParameter) {
    int workerId = *(int*)lpParameter;

    printf("[Worker #%d | TID=%lu] Ready and waiting for tasks...\n",
           workerId, GetCurrentThreadId());

    while (1) {
        // ====================================================================
        // Wait for either: a task is available OR shutdown signal
        // Chờ một trong hai: có công việc hoặc tín hiệu tắt
        //
        // We use WaitForMultipleObjects to wait on TWO events:
        //   [0] = hTaskAvailable (auto-reset)
        //   [1] = hShutdown      (manual-reset)
        //
        // bWaitAll=FALSE means: wake up when EITHER event is signaled
        // bWaitAll=FALSE nghĩa là: thức dậy khi BẤT KỲ event nào được báo hiệu
        // ====================================================================
        HANDLE waitHandles[2] = {
            g_queue.hTaskAvailable,
            g_queue.hShutdown
        };

        DWORD waitResult = WaitForMultipleObjects(
            2,              // Two handles
            waitHandles,    // Array of handles
            FALSE,          // Wait for ANY (not ALL)
            INFINITE        // No timeout
        );

        // Check which event was signaled / Kiểm tra event nào được báo hiệu
        if (waitResult == WAIT_OBJECT_0 + 1) {
            // Shutdown event signaled / Tín hiệu tắt
            // Check if there are remaining tasks before exiting
            // Kiểm tra có công việc còn lại trước khi thoát
            Task task;
            if (DequeueTask(&g_queue, &task)) {
                // Process remaining task / Xử lý công việc còn lại
                printf("[Worker #%d] Processing remaining task #%d: \"%s\"\n",
                       workerId, task.taskId, task.description);
                Sleep(task.workDurationMs);
                InterlockedIncrement(&g_tasksCompleted);
                InterlockedIncrement(&g_tasksByWorker[workerId]);
            }
            printf("[Worker #%d] Shutdown signal received. Exiting.\n",
                   workerId);
            break;
        }

        if (waitResult == WAIT_OBJECT_0) {
            // Task available event signaled / Có công việc
            Task task;
            if (DequeueTask(&g_queue, &task)) {
                printf("[Worker #%d] Picked up task #%d: \"%s\" "
                       "(duration: %dms)\n",
                       workerId, task.taskId, task.description,
                       task.workDurationMs);

                // Simulate work / Giả lập công việc
                Sleep(task.workDurationMs);

                // Track completion / Theo dõi hoàn thành
                long completed = InterlockedIncrement(&g_tasksCompleted);
                InterlockedIncrement(&g_tasksByWorker[workerId]);

                printf("[Worker #%d] Completed task #%d (%ld/%d total)\n",
                       workerId, task.taskId, completed, TOTAL_TASKS);
            }
            // If DequeueTask returned FALSE, another worker got it first
            // Nếu DequeueTask trả về FALSE, công nhân khác đã lấy trước
        }
    }

    return 0;
}

// ============================================================================
// Cleanup the task queue / Dọn dẹp hàng đợi công việc
// ============================================================================
void DestroyTaskQueue(TaskQueue* q) {
    if (q->hMutex)         CloseHandle(q->hMutex);
    if (q->hTaskAvailable) CloseHandle(q->hTaskAvailable);
    if (q->hShutdown)      CloseHandle(q->hShutdown);
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 07: Thread Pool Simulation\n");
    printf("  Bai tap 07: Mo phong Thread Pool\n");
    printf("============================================================\n\n");

    printf("Configuration:\n");
    printf("  Workers: %d\n", NUM_WORKERS);
    printf("  Total tasks: %d\n", TOTAL_TASKS);
    printf("  Queue capacity: %d\n\n", MAX_QUEUE_SIZE);

    // Initialize queue / Khởi tạo hàng đợi
    if (!InitTaskQueue(&g_queue)) {
        printf("[ERROR] Failed to initialize task queue!\n");
        return 1;
    }
    printf("[Main] Task queue initialized.\n\n");

    // ========================================================================
    // Create worker threads / Tạo luồng công nhân
    // ========================================================================
    HANDLE hWorkers[NUM_WORKERS];
    int    workerIds[NUM_WORKERS];

    printf("--- Creating Worker Threads ---\n\n");

    for (int i = 0; i < NUM_WORKERS; i++) {
        workerIds[i] = i;
        hWorkers[i] = CreateThread(NULL, 0, WorkerThreadFunc,
                                   &workerIds[i], 0, NULL);
        if (hWorkers[i] == NULL) {
            printf("[ERROR] Failed to create worker %d! Error: %lu\n",
                   i, GetLastError());
            // Signal shutdown for already-created workers
            SetEvent(g_queue.hShutdown);
            for (int j = 0; j < i; j++) {
                WaitForSingleObject(hWorkers[j], INFINITE);
                CloseHandle(hWorkers[j]);
            }
            DestroyTaskQueue(&g_queue);
            return 1;
        }
    }

    // Give workers time to start / Cho công nhân thời gian khởi động
    Sleep(200);

    // ========================================================================
    // Submit tasks / Gửi công việc
    // ========================================================================
    printf("\n--- Submitting Tasks ---\n\n");

    const char* taskDescs[] = {
        "Parse config file",
        "Load user data",
        "Validate input",
        "Compress archive",
        "Send notification",
        "Generate thumbnail",
        "Update database",
        "Sync cloud data",
        "Run diagnostics",
        "Build report",
        "Clean temp files",
        "Backup logs"
    };

    int durations[] = {
        300, 500, 200, 800, 400, 600, 350, 700, 250, 450, 150, 550
    };

    for (int i = 0; i < TOTAL_TASKS; i++) {
        Task task;
        task.taskId = i + 1;
        snprintf(task.description, sizeof(task.description),
                 "%s", taskDescs[i % 12]);
        task.workDurationMs = durations[i % 12];
        task.priority = (i % 3) + 1;

        EnqueueTask(&g_queue, &task);

        // Simulate tasks arriving over time / Giả lập công việc đến theo thời gian
        Sleep(150);
    }

    printf("\n[Main] All %d tasks submitted. Waiting for completion...\n\n",
           TOTAL_TASKS);

    // ========================================================================
    // Wait for all tasks to be processed / Chờ tất cả công việc được xử lý
    // ========================================================================
    // Simple polling with sleep (in production, use a completion event)
    // Kiểm tra đơn giản với sleep (trong sản phẩm, dùng event hoàn thành)
    while (g_tasksCompleted < TOTAL_TASKS) {
        Sleep(100);
    }

    printf("[Main] All tasks completed! Sending shutdown signal...\n\n");

    // ========================================================================
    // Shutdown: signal all workers to stop
    // Tắt: báo hiệu tất cả công nhân dừng lại
    //
    // SetEvent on a MANUAL-RESET event: stays signaled, waking ALL waiters
    // SetEvent trên event ĐẶT LẠI THỦ CÔNG: giữ báo hiệu, đánh thức TẤT CẢ
    // ========================================================================
    SetEvent(g_queue.hShutdown);

    // Also signal taskAvailable to wake up any sleeping workers
    // Cũng báo hiệu taskAvailable để đánh thức công nhân đang ngủ
    for (int i = 0; i < NUM_WORKERS; i++) {
        SetEvent(g_queue.hTaskAvailable);
    }

    // Wait for all workers to exit / Chờ tất cả công nhân thoát
    WaitForMultipleObjects(NUM_WORKERS, hWorkers, TRUE, 5000);

    // Cleanup / Dọn dẹp
    for (int i = 0; i < NUM_WORKERS; i++) {
        CloseHandle(hWorkers[i]);
    }
    DestroyTaskQueue(&g_queue);

    // ========================================================================
    // Results / Kết quả
    // ========================================================================
    printf("\n============================================================\n");
    printf("  Thread Pool Results / Ket qua Thread Pool:\n");
    printf("------------------------------------------------------------\n");
    printf("  Total tasks completed: %ld / %d\n",
           g_tasksCompleted, TOTAL_TASKS);
    printf("\n  Tasks per worker / Cong viec moi cong nhan:\n");
    for (int i = 0; i < NUM_WORKERS; i++) {
        printf("    Worker #%d: %ld tasks", i, g_tasksByWorker[i]);

        // Visual bar / Thanh trực quan
        printf("  [");
        for (long j = 0; j < g_tasksByWorker[i]; j++) printf("##");
        printf("]\n");
    }

    printf("------------------------------------------------------------\n");
    printf("\n  Key concepts / Khai niem chinh:\n\n");
    printf("  Auto-Reset Event (hTaskAvailable):\n");
    printf("    - Wakes ONE worker per SetEvent() call\n");
    printf("      Danh thuc MOT cong nhan moi lan goi SetEvent()\n");
    printf("    - Prevents \"thundering herd\" problem\n");
    printf("      Ngan van de \"bay dan san\"\n\n");
    printf("  Manual-Reset Event (hShutdown):\n");
    printf("    - Stays signaled - wakes ALL workers\n");
    printf("      Giu bao hieu - danh thuc TAT CA cong nhan\n");
    printf("    - Perfect for broadcast shutdown signals\n");
    printf("      Hoan hao cho tin hieu tat phat song\n\n");
    printf("  Thread Pool advantages / Uu diem Thread Pool:\n");
    printf("    - Reuse threads (avoid create/destroy overhead)\n");
    printf("      Tai su dung luong (tranh chi phi tao/huy)\n");
    printf("    - Control concurrency level\n");
    printf("      Kiem soat muc do dong thoi\n");
    printf("    - Queue tasks when all workers are busy\n");
    printf("      Xep hang cong viec khi tat ca cong nhan ban\n");
    printf("============================================================\n");

    return 0;
}
