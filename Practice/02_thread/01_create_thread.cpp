/*
 * ============================================================================
 * Exercise 01: Basic Thread Creation / Tạo luồng cơ bản
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrates the most fundamental threading operation in Windows:
 *   creating a thread with CreateThread() and waiting for it to finish
 *   with WaitForSingleObject().
 *
 *   Minh họa thao tác luồng cơ bản nhất trong Windows: tạo luồng bằng
 *   CreateThread() và chờ luồng kết thúc bằng WaitForSingleObject().
 *
 * Key APIs (Các API chính):
 *   - CreateThread()          : Create a new thread / Tạo luồng mới
 *   - WaitForSingleObject()   : Wait for thread to finish / Chờ luồng kết thúc
 *   - CloseHandle()           : Close thread handle / Đóng handle luồng
 *   - GetCurrentThreadId()    : Get current thread's ID / Lấy ID luồng hiện tại
 *   - Sleep()                 : Pause execution / Tạm dừng thực thi
 *
 * Compile (Biên dịch):
 *   g++ 01_create_thread.cpp -o 01_create_thread.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Thread function / Hàm luồng
// ============================================================================
// Every thread needs a function with this exact signature:
// Mỗi luồng cần một hàm có chữ ký chính xác như sau:
//
//   DWORD WINAPI ThreadFunction(LPVOID lpParameter);
//
// - DWORD   : return type, 32-bit unsigned integer (kiểu trả về)
// - WINAPI  : calling convention (__stdcall) required by Windows
// - LPVOID  : void pointer, allows passing any data to the thread
//             (con trỏ void, cho phép truyền bất kỳ dữ liệu nào vào luồng)
// ============================================================================

DWORD WINAPI SimpleThreadFunction(LPVOID lpParameter) {
    // Cast the parameter back to its original type
    // Ép kiểu tham số về kiểu ban đầu
    const char* message = (const char*)lpParameter;

    printf("[Thread %lu] Started! Message: \"%s\"\n",
           GetCurrentThreadId(), message);

    // Simulate some work / Giả lập công việc
    for (int i = 1; i <= 5; i++) {
        printf("[Thread %lu] Working... step %d/5\n",
               GetCurrentThreadId(), i);
        Sleep(500);  // Sleep 500ms - nhường CPU cho luồng khác
    }

    printf("[Thread %lu] Finished!\n", GetCurrentThreadId());

    // Return an exit code (can be retrieved with GetExitCodeThread)
    // Trả về mã thoát (có thể lấy bằng GetExitCodeThread)
    return 42;
}

// ============================================================================
// A thread function that receives a struct as parameter
// Hàm luồng nhận struct làm tham số
// ============================================================================
struct ThreadData {
    int     id;
    char    name[64];
    int     workItems;
};

DWORD WINAPI StructThreadFunction(LPVOID lpParameter) {
    // Cast to our struct pointer / Ép kiểu về con trỏ struct
    ThreadData* data = (ThreadData*)lpParameter;

    printf("[Thread %lu] ID=%d, Name=\"%s\", WorkItems=%d\n",
           GetCurrentThreadId(), data->id, data->name, data->workItems);

    for (int i = 0; i < data->workItems; i++) {
        printf("[Thread %lu] Processing item %d/%d\n",
               GetCurrentThreadId(), i + 1, data->workItems);
        Sleep(300);
    }

    printf("[Thread %lu] All work done!\n", GetCurrentThreadId());
    return 0;
}

int main() {
    printf("============================================================\n");
    printf("  Exercise 01: Basic Thread Creation\n");
    printf("  Bai tap 01: Tao luong co ban\n");
    printf("============================================================\n\n");

    printf("[Main Thread %lu] Starting...\n\n", GetCurrentThreadId());

    // ========================================================================
    // PART 1: Simple thread with string parameter
    // PHAN 1: Luồng đơn giản với tham số chuỗi
    // ========================================================================
    printf("--- PART 1: Simple Thread Creation ---\n\n");

    DWORD  threadId = 0;    // Will receive the new thread's ID
                            // Sẽ nhận ID của luồng mới
    HANDLE hThread = NULL;  // Handle to the new thread
                            // Handle đến luồng mới

    // ========================================================================
    // CreateThread() - The core API for thread creation
    // CreateThread() - API cốt lõi để tạo luồng
    //
    // HANDLE CreateThread(
    //   LPSECURITY_ATTRIBUTES  lpThreadAttributes,  // [1] Security
    //   SIZE_T                 dwStackSize,          // [2] Stack size
    //   LPTHREAD_START_ROUTINE lpStartAddress,       // [3] Thread function
    //   LPVOID                 lpParameter,          // [4] Parameter
    //   DWORD                  dwCreationFlags,      // [5] Creation flags
    //   LPDWORD                lpThreadId            // [6] Thread ID output
    // );
    //
    // Parameter details / Chi tiết tham số:
    //
    // [1] lpThreadAttributes (LPSECURITY_ATTRIBUTES):
    //     - NULL = default security, handle NOT inheritable
    //       (bảo mật mặc định, handle KHÔNG thể kế thừa)
    //     - Set SECURITY_ATTRIBUTES.bInheritHandle = TRUE for child processes
    //       to inherit this handle
    //
    // [2] dwStackSize (SIZE_T):
    //     - 0 = use default stack size (usually 1MB)
    //       (dùng kích thước stack mặc định, thường là 1MB)
    //     - Specify a value to set custom stack size
    //
    // [3] lpStartAddress (LPTHREAD_START_ROUTINE):
    //     - Pointer to the function the thread will execute
    //       (Con trỏ đến hàm mà luồng sẽ thực thi)
    //     - Must have signature: DWORD WINAPI Func(LPVOID)
    //
    // [4] lpParameter (LPVOID):
    //     - Data passed to the thread function
    //       (Dữ liệu truyền cho hàm luồng)
    //     - Can be NULL if no data needed
    //
    // [5] dwCreationFlags (DWORD):
    //     - 0                  = thread starts immediately
    //                            (luồng bắt đầu ngay lập tức)
    //     - CREATE_SUSPENDED   = thread starts in suspended state
    //                            (luồng bắt đầu ở trạng thái treo)
    //
    // [6] lpThreadId (LPDWORD):
    //     - Receives the unique thread identifier
    //       (Nhận mã định danh duy nhất của luồng)
    //     - Can be NULL if you don't need the ID
    // ========================================================================

    const char* msg = "Hello from the new thread!";

    hThread = CreateThread(
        NULL,                   // [1] Default security / Bảo mật mặc định
        0,                      // [2] Default stack size / Kích thước stack mặc định
        SimpleThreadFunction,   // [3] Thread function / Hàm luồng
        (LPVOID)msg,            // [4] Parameter to pass / Tham số truyền vào
        0,                      // [5] Start immediately / Bắt đầu ngay
        &threadId               // [6] Receive thread ID / Nhận ID luồng
    );

    // Always check if CreateThread succeeded!
    // Luôn kiểm tra xem CreateThread có thành công không!
    if (hThread == NULL) {
        printf("[ERROR] CreateThread failed! Error code: %lu\n",
               GetLastError());
        return 1;
    }

    printf("[Main Thread] Created thread with ID: %lu\n", threadId);
    printf("[Main Thread] Thread handle: %p\n\n", hThread);

    // ========================================================================
    // WaitForSingleObject() - Wait for the thread to finish
    // WaitForSingleObject() - Chờ luồng kết thúc
    //
    // DWORD WaitForSingleObject(
    //   HANDLE hHandle,        // Handle to wait on (thread, mutex, event, etc.)
    //                          // Handle cần chờ (luồng, mutex, event, v.v.)
    //   DWORD  dwMilliseconds  // Timeout in milliseconds
    //                          // Thời gian chờ tối đa (mili giây)
    //                          // INFINITE = wait forever / chờ mãi mãi
    // );
    //
    // Return values / Giá trị trả về:
    //   WAIT_OBJECT_0  (0x00) = Object is signaled (thread finished)
    //                           (Đối tượng được báo hiệu - luồng đã kết thúc)
    //   WAIT_TIMEOUT   (0x102)= Timeout expired / Hết thời gian chờ
    //   WAIT_FAILED    (0xFFFFFFFF) = Error / Lỗi
    // ========================================================================

    printf("[Main Thread] Waiting for thread to finish...\n");

    DWORD waitResult = WaitForSingleObject(hThread, INFINITE);

    switch (waitResult) {
        case WAIT_OBJECT_0:
            printf("[Main Thread] Thread completed successfully!\n");
            break;
        case WAIT_TIMEOUT:
            printf("[Main Thread] Wait timed out!\n");
            break;
        case WAIT_FAILED:
            printf("[Main Thread] Wait failed! Error: %lu\n", GetLastError());
            break;
    }

    // ========================================================================
    // GetExitCodeThread() - Get the thread's return value
    // GetExitCodeThread() - Lấy giá trị trả về của luồng
    // ========================================================================
    DWORD exitCode = 0;
    if (GetExitCodeThread(hThread, &exitCode)) {
        printf("[Main Thread] Thread exit code: %lu\n", exitCode);
    }

    // ========================================================================
    // CloseHandle() - Release the thread handle
    // CloseHandle() - Giải phóng handle luồng
    //
    // IMPORTANT: Closing the handle does NOT terminate the thread!
    // It only releases the handle resource. The thread continues to run
    // if it hasn't finished yet.
    //
    // QUAN TRỌNG: Đóng handle KHÔNG kết thúc luồng!
    // Nó chỉ giải phóng tài nguyên handle. Luồng vẫn tiếp tục chạy
    // nếu chưa kết thúc.
    // ========================================================================
    CloseHandle(hThread);
    printf("[Main Thread] Thread handle closed.\n\n");

    // ========================================================================
    // PART 2: Thread with struct parameter
    // PHAN 2: Luồng với tham số struct
    // ========================================================================
    printf("--- PART 2: Thread with Struct Parameter ---\n\n");

    // IMPORTANT: The struct must remain valid while the thread is running!
    // If you allocate it on the stack, you MUST wait for the thread before
    // the variable goes out of scope.
    //
    // QUAN TRỌNG: Struct phải còn hợp lệ khi luồng đang chạy!
    // Nếu cấp phát trên stack, bạn PHẢI chờ luồng trước khi biến hết phạm vi.
    ThreadData data;
    data.id = 1;
    snprintf(data.name, sizeof(data.name), "Worker-Alpha");
    data.workItems = 3;

    hThread = CreateThread(
        NULL,                    // Default security
        0,                       // Default stack size
        StructThreadFunction,    // Thread function
        &data,                   // Pass struct address / Truyền địa chỉ struct
        0,                       // Start immediately
        &threadId                // Receive thread ID
    );

    if (hThread == NULL) {
        printf("[ERROR] CreateThread failed! Error code: %lu\n",
               GetLastError());
        return 1;
    }

    printf("[Main Thread] Created struct thread with ID: %lu\n\n", threadId);

    // Wait and cleanup / Chờ và dọn dẹp
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    // ========================================================================
    // PART 3: Demonstrate timeout with WaitForSingleObject
    // PHAN 3: Minh họa timeout với WaitForSingleObject
    // ========================================================================
    printf("\n--- PART 3: Wait with Timeout ---\n\n");

    hThread = CreateThread(NULL, 0, SimpleThreadFunction,
                           (LPVOID)"Long task", 0, &threadId);
    if (hThread == NULL) {
        printf("[ERROR] CreateThread failed!\n");
        return 1;
    }

    // Try waiting with a short timeout first
    // Thử chờ với timeout ngắn trước
    printf("[Main Thread] Waiting with 1-second timeout...\n");
    waitResult = WaitForSingleObject(hThread, 1000);  // 1000ms = 1 second

    if (waitResult == WAIT_TIMEOUT) {
        printf("[Main Thread] Timeout! Thread is still running.\n");
        printf("[Main Thread] Now waiting indefinitely...\n");

        // Wait for real this time / Chờ thật lần này
        WaitForSingleObject(hThread, INFINITE);
        printf("[Main Thread] Thread finally finished!\n");
    } else {
        printf("[Main Thread] Thread finished within timeout.\n");
    }

    CloseHandle(hThread);

    // ========================================================================
    printf("\n============================================================\n");
    printf("  Summary / Tom tat:\n");
    printf("  - CreateThread() creates a new thread of execution\n");
    printf("    CreateThread() tao mot luong thuc thi moi\n");
    printf("  - WaitForSingleObject() blocks until thread finishes\n");
    printf("    WaitForSingleObject() chan cho den khi luong ket thuc\n");
    printf("  - CloseHandle() releases the handle (NOT the thread)\n");
    printf("    CloseHandle() giai phong handle (KHONG phai luong)\n");
    printf("  - Always pass valid pointers that outlive the thread!\n");
    printf("    Luon truyen con tro hop le con ton tai khi luong chay!\n");
    printf("============================================================\n");

    return 0;
}
