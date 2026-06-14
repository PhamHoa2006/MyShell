/*
 * Case Study 02: Producer-Consumer Problem
 * Description: A classic multi-threading synchronization problem solved using 
 *              Semaphores and Mutexes. Multiple producers generate items into a 
 *              bounded buffer, and multiple consumers process them.
 *
 * Key APIs covered:
 * - CreateSemaphore, ReleaseSemaphore: To track empty and full slots in the buffer.
 * - CreateMutex, ReleaseMutex: To protect access to the shared buffer.
 * - CreateThread, WaitForMultipleObjects: Thread management.
 *
 * Tiếng Việt:
 * Case Study 02: Bài toán Nhà sản xuất - Người tiêu dùng
 * Mô tả: Sử dụng Semaphore và Mutex để đồng bộ hoá nhiều luồng sản xuất và tiêu thụ
 *        trên cùng một hàng đợi có kích thước giới hạn.
 */

#include <windows.h>
#include <iostream>
#include <queue>
#include <vector>

const int BUFFER_SIZE = 5;
const int NUM_PRODUCERS = 2;
const int NUM_CONSUMERS = 2;
const int ITEMS_PER_PRODUCER = 10;

// Shared Resources
std::queue<int> buffer;
HANDLE hMutex;           // Bảo vệ truy cập buffer / Protects buffer access
HANDLE hSemEmpty;        // Đếm số khe trống / Counts empty slots
HANDLE hSemFull;         // Đếm số sản phẩm có sẵn / Counts filled slots

// Luồng Nhà sản xuất / Producer Thread
DWORD WINAPI Producer(LPVOID lpParam) {
    int producerId = (int)(intptr_t)lpParam;
    
    for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        int item = producerId * 100 + i; // Tạo ra một món đồ / Produce an item

        // Đợi có khe trống / Wait for an empty slot
        WaitForSingleObject(hSemEmpty, INFINITE);

        // Khóa buffer / Lock the buffer
        WaitForSingleObject(hMutex, INFINITE);
        
        // Thêm vào buffer / Add to buffer
        buffer.push(item);
        std::cout << "Producer " << producerId << " produced item: " << item 
                  << " (Buffer size: " << buffer.size() << ")" << std::endl;

        // Mở khóa buffer / Unlock buffer
        ReleaseMutex(hMutex);

        // Tăng số slot đầy (báo hiệu cho Consumer) / Increment full slots (signal consumer)
        ReleaseSemaphore(hSemFull, 1, NULL);

        Sleep(100); // Giả lập thời gian sản xuất / Simulate work
    }
    return 0;
}

// Luồng Người tiêu dùng / Consumer Thread
DWORD WINAPI Consumer(LPVOID lpParam) {
    int consumerId = (int)(intptr_t)lpParam;
    int totalItemsToConsume = (NUM_PRODUCERS * ITEMS_PER_PRODUCER) / NUM_CONSUMERS;

    for (int i = 0; i < totalItemsToConsume; ++i) {
        // Đợi có sản phẩm / Wait for a filled slot
        WaitForSingleObject(hSemFull, INFINITE);

        // Khóa buffer / Lock the buffer
        WaitForSingleObject(hMutex, INFINITE);
        
        // Lấy từ buffer / Remove from buffer
        int item = buffer.front();
        buffer.pop();
        std::cout << "\t\tConsumer " << consumerId << " consumed item: " << item 
                  << " (Buffer size: " << buffer.size() << ")" << std::endl;

        // Mở khóa buffer / Unlock buffer
        ReleaseMutex(hMutex);

        // Tăng số slot trống (báo hiệu cho Producer) / Increment empty slots (signal producer)
        ReleaseSemaphore(hSemEmpty, 1, NULL);

        Sleep(150); // Giả lập thời gian tiêu thụ / Simulate work
    }
    return 0;
}

int main() {
    std::cout << "Starting Producer-Consumer Demo..." << std::endl;

    // Khởi tạo các đối tượng đồng bộ / Initialize synchronization objects
    hMutex = CreateMutex(NULL, FALSE, NULL);
    hSemEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL); // Ban đầu trống tất cả
    hSemFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);            // Ban đầu không có gì

    std::vector<HANDLE> threads;

    // Tạo luồng Producer / Create Producer threads
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        HANDLE hThread = CreateThread(NULL, 0, Producer, (LPVOID)(intptr_t)i, 0, NULL);
        threads.push_back(hThread);
    }

    // Tạo luồng Consumer / Create Consumer threads
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        HANDLE hThread = CreateThread(NULL, 0, Consumer, (LPVOID)(intptr_t)i, 0, NULL);
        threads.push_back(hThread);
    }

    // Đợi tất cả luồng hoàn thành / Wait for all threads to finish
    WaitForMultipleObjects((DWORD)threads.size(), threads.data(), TRUE, INFINITE);

    // Dọn dẹp / Cleanup
    for (HANDLE hThread : threads) {
        CloseHandle(hThread);
    }
    CloseHandle(hMutex);
    CloseHandle(hSemEmpty);
    CloseHandle(hSemFull);

    std::cout << "All threads completed successfully." << std::endl;
    return 0;
}
