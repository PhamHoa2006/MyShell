/*
 * ============================================================================
 * Exercise 06-05: System Uptime / Thời gian hoạt động hệ thống
 * ============================================================================
 * Description (EN): Calculates and displays system uptime using GetTickCount64.
 *                   Also demonstrates QueryPerformanceCounter for high-res
 *                   timing and comparison between the two methods.
 *
 * Mô tả (VI):      Tính và hiển thị thời gian hệ thống đã hoạt động bằng
 *                   GetTickCount64. Cũng minh họa QueryPerformanceCounter
 *                   cho đồng hồ độ phân giải cao và so sánh hai phương pháp.
 *
 * Key APIs:  GetTickCount64, GetTickCount, QueryPerformanceCounter,
 *            QueryPerformanceFrequency
 *
 * Compile:   g++ 05_uptime.cpp -o 05_uptime.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Định dạng thời gian uptime thành chuỗi dễ đọc
// Format uptime duration into a human-readable string
// ============================================================================
void FormatUptime(ULONGLONG totalMs, char* buffer, int bufSize)
{
    // Chuyển đổi milliseconds sang các đơn vị lớn hơn
    // Convert milliseconds to larger units
    ULONGLONG totalSeconds = totalMs / 1000;
    ULONGLONG totalMinutes = totalSeconds / 60;
    ULONGLONG totalHours   = totalMinutes / 60;
    ULONGLONG days         = totalHours / 24;

    ULONGLONG hours   = totalHours   % 24;
    ULONGLONG minutes = totalMinutes % 60;
    ULONGLONG seconds = totalSeconds % 60;
    ULONGLONG ms      = totalMs      % 1000;

    if (days > 0) {
        snprintf(buffer, bufSize,
                 "%llu day(s) %llu hour(s) %llu min %llu sec %llu ms",
                 (unsigned long long)days,
                 (unsigned long long)hours,
                 (unsigned long long)minutes,
                 (unsigned long long)seconds,
                 (unsigned long long)ms);
    } else if (hours > 0) {
        snprintf(buffer, bufSize,
                 "%llu hour(s) %llu min %llu sec %llu ms",
                 (unsigned long long)hours,
                 (unsigned long long)minutes,
                 (unsigned long long)seconds,
                 (unsigned long long)ms);
    } else {
        snprintf(buffer, bufSize,
                 "%llu min %llu sec %llu ms",
                 (unsigned long long)minutes,
                 (unsigned long long)seconds,
                 (unsigned long long)ms);
    }
}

// ============================================================================
// Hiển thị uptime bằng nhiều cách biểu diễn
// Display uptime in multiple representations
// ============================================================================
void DisplayUptime(ULONGLONG uptimeMs)
{
    ULONGLONG totalSec = uptimeMs / 1000;
    ULONGLONG days     = totalSec / 86400;
    ULONGLONG hours    = (totalSec % 86400) / 3600;
    ULONGLONG minutes  = (totalSec % 3600) / 60;
    ULONGLONG seconds  = totalSec % 60;
    ULONGLONG ms       = uptimeMs % 1000;

    printf("  Raw milliseconds / Mili giây thô:       %llu ms\n",
           (unsigned long long)uptimeMs);
    printf("  Total seconds / Tổng giây:               %.3f s\n",
           (double)uptimeMs / 1000.0);
    printf("  Total minutes / Tổng phút:               %.2f min\n",
           (double)uptimeMs / 60000.0);
    printf("  Total hours / Tổng giờ:                  %.2f h\n",
           (double)uptimeMs / 3600000.0);
    printf("  Total days / Tổng ngày:                  %.4f days\n\n",
           (double)uptimeMs / 86400000.0);

    // Hiển thị dạng DD:HH:MM:SS.mmm
    printf("  Formatted / Định dạng:\n");
    printf("    %llu days, %02llu:%02llu:%02llu.%03llu\n",
           (unsigned long long)days,
           (unsigned long long)hours,
           (unsigned long long)minutes,
           (unsigned long long)seconds,
           (unsigned long long)ms);

    // Hiển thị dạng văn bản / Text format
    char buf[256];
    FormatUptime(uptimeMs, buf, sizeof(buf));
    printf("    %s\n\n", buf);

    // Hiển thị dạng tiếng Việt / Vietnamese format
    printf("  Tiếng Việt:\n");
    printf("    %llu ngày, %llu giờ, %llu phút, %llu giây\n\n",
           (unsigned long long)days,
           (unsigned long long)hours,
           (unsigned long long)minutes,
           (unsigned long long)seconds);
}

// ============================================================================
// Demo 1: GetTickCount64 (khuyến nghị dùng)
// Demo 1: GetTickCount64 (recommended)
// ============================================================================
void DemoGetTickCount64()
{
    printf("=== 1. GetTickCount64 - System Uptime / Thời gian hoạt động ===\n\n");

    // GetTickCount64: Trả về số millisecond kể từ khi hệ thống khởi động
    // GetTickCount64: Returns milliseconds since system boot
    // Trả về giá trị 64-bit → không bao giờ bị tràn (overflow)
    // Returns 64-bit value → never overflows
    // Độ phân giải: khoảng 10-16 ms (phụ thuộc phần cứng)
    // Resolution: approximately 10-16 ms (depends on hardware)
    ULONGLONG uptimeMs = GetTickCount64();

    printf("  GetTickCount64() = %llu\n\n", (unsigned long long)uptimeMs);
    DisplayUptime(uptimeMs);
}

// ============================================================================
// Demo 2: GetTickCount (32-bit, có thể tràn)
// Demo 2: GetTickCount (32-bit, can overflow)
// ============================================================================
void DemoGetTickCount()
{
    printf("=== 2. GetTickCount (32-bit) - Cảnh báo tràn số ===\n\n");

    // GetTickCount: Phiên bản 32-bit cũ hơn
    // GetTickCount: Older 32-bit version
    // CHÚ Ý: Giá trị DWORD (32-bit) sẽ tràn sau ~49.7 ngày!
    // WARNING: DWORD (32-bit) value overflows after ~49.7 days!
    // 2^32 ms = 4,294,967,296 ms ≈ 49 ngày 17 giờ 2 phút 47 giây
    // 2^32 ms = 4,294,967,296 ms ≈ 49 days 17 hours 2 min 47 sec
    DWORD tickCount = GetTickCount();

    printf("  GetTickCount() = %lu\n", tickCount);
    printf("  (32-bit, wraps after ~49.7 days / tràn sau ~49.7 ngày)\n\n");

    // So sánh với GetTickCount64 / Compare with GetTickCount64
    ULONGLONG tickCount64 = GetTickCount64();
    printf("  Comparison / So sánh:\n");
    printf("    GetTickCount   = %lu\n", tickCount);
    printf("    GetTickCount64 = %llu\n", (unsigned long long)tickCount64);

    if (tickCount64 > 0xFFFFFFFFULL) {
        printf("    [!] System has been up > 49.7 days! GetTickCount has wrapped.\n");
        printf("    [!] Hệ thống đã hoạt động > 49.7 ngày! GetTickCount đã tràn.\n");
        printf("    [!] Number of wraps / Số lần tràn: %llu\n",
               (unsigned long long)(tickCount64 / 0x100000000ULL));
    } else {
        printf("    [OK] Values match (uptime < 49.7 days) / Giá trị khớp\n");
    }
    printf("\n");
}

// ============================================================================
// Demo 3: QueryPerformanceCounter - Đồng hồ độ phân giải cao
// Demo 3: QueryPerformanceCounter - High-resolution timer
// ============================================================================
void DemoPerformanceCounter()
{
    printf("=== 3. QueryPerformanceCounter - High-Res Timer / Bộ đếm chính xác ===\n\n");

    // QueryPerformanceFrequency: Lấy tần số bộ đếm (counts/second)
    // QueryPerformanceFrequency: Gets counter frequency (counts/second)
    // Tần số thường là 10 MHz (10,000,000 Hz) trên Windows hiện đại
    // Frequency is typically 10 MHz (10,000,000 Hz) on modern Windows
    LARGE_INTEGER freq;
    if (!QueryPerformanceFrequency(&freq)) {
        printf("  [ERROR] QueryPerformanceFrequency failed / Lỗi: %lu\n\n",
               GetLastError());
        return;
    }

    printf("  Counter Frequency / Tần số bộ đếm: %lld Hz\n", freq.QuadPart);
    printf("  Resolution / Độ phân giải: %.9f seconds (%.3f ns)\n\n",
           1.0 / (double)freq.QuadPart,
           1e9 / (double)freq.QuadPart);

    // QueryPerformanceCounter: Lấy giá trị bộ đếm hiện tại
    // QueryPerformanceCounter: Gets current counter value
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    printf("  Current Counter Value / Giá trị hiện tại: %lld\n", counter.QuadPart);

    // Tính uptime bằng performance counter
    // Calculate uptime using performance counter
    double uptimeSec = (double)counter.QuadPart / (double)freq.QuadPart;
    ULONGLONG uptimeMs = (ULONGLONG)(uptimeSec * 1000.0);

    printf("  Calculated Uptime / Thời gian tính toán: %.6f seconds\n", uptimeSec);
    printf("  As milliseconds / Mili giây:             %llu ms\n\n",
           (unsigned long long)uptimeMs);

    // So sánh với GetTickCount64 / Compare with GetTickCount64
    ULONGLONG tickMs = GetTickCount64();
    double diff = (double)uptimeMs - (double)tickMs;
    printf("  Comparison / So sánh:\n");
    printf("    Performance Counter: %llu ms\n", (unsigned long long)uptimeMs);
    printf("    GetTickCount64:      %llu ms\n", (unsigned long long)tickMs);
    printf("    Difference / Chênh lệch: %.0f ms (%.3f sec)\n\n", diff, diff / 1000.0);
}

// ============================================================================
// Demo 4: Đo thời gian thực thi code
// Demo 4: Measure code execution time
// ============================================================================
void DemoPrecisionTiming()
{
    printf("=== 4. Precision Timing Demo / Demo đo thời gian chính xác ===\n\n");

    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    // Đo thời gian Sleep / Measure Sleep duration
    printf("  Measuring Sleep(100) accuracy...\n");
    printf("  Đang đo độ chính xác của Sleep(100)...\n\n");

    // Chạy 5 lần để thấy sự biến thiên / Run 5 times to see variation
    for (int i = 1; i <= 5; i++) {
        QueryPerformanceCounter(&start);
        Sleep(100);  // Yêu cầu ngủ 100ms / Request 100ms sleep
        QueryPerformanceCounter(&end);

        double elapsed = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
        printf("  Trial %d / Lần %d: Sleep(100) actual = %.6f ms (expected 100.000 ms)\n",
               i, i, elapsed * 1000.0);
    }

    printf("\n  Note / Ghi chú: Sleep() is NOT precise!\n");
    printf("  Sleep() không chính xác! Thường dài hơn yêu cầu do timer resolution.\n\n");
}

// ============================================================================
// Tính thời điểm hệ thống khởi động
// Calculate system boot time
// ============================================================================
void PrintBootTime()
{
    printf("=== 5. Estimated Boot Time / Thời điểm khởi động ước tính ===\n\n");

    // Lấy giờ hiện tại và uptime để tính giờ khởi động
    // Get current time and uptime to calculate boot time
    ULONGLONG uptimeMs = GetTickCount64();
    SYSTEMTIME now;
    GetLocalTime(&now);

    FILETIME ftNow;
    SystemTimeToFileTime(&now, &ftNow);

    ULARGE_INTEGER uliNow;
    uliNow.HighPart = ftNow.dwHighDateTime;
    uliNow.LowPart  = ftNow.dwLowDateTime;

    // Trừ uptime (đổi sang 100ns intervals) / Subtract uptime (in 100ns intervals)
    ULARGE_INTEGER uliBoot;
    uliBoot.QuadPart = uliNow.QuadPart - (uptimeMs * 10000ULL);

    FILETIME ftBoot;
    ftBoot.dwHighDateTime = uliBoot.HighPart;
    ftBoot.dwLowDateTime  = uliBoot.LowPart;

    SYSTEMTIME bootTime;
    FileTimeToSystemTime(&ftBoot, &bootTime);

    printf("  Current Time / Giờ hiện tại:     %04d-%02d-%02d %02d:%02d:%02d\n",
           now.wYear, now.wMonth, now.wDay,
           now.wHour, now.wMinute, now.wSecond);
    printf("  Uptime / Thời gian hoạt động:    ");
    char buf[256];
    FormatUptime(uptimeMs, buf, sizeof(buf));
    printf("%s\n", buf);
    printf("  Boot Time / Giờ khởi động:       %04d-%02d-%02d %02d:%02d:%02d (estimated)\n\n",
           bootTime.wYear, bootTime.wMonth, bootTime.wDay,
           bootTime.wHour, bootTime.wMinute, bootTime.wSecond);
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    printf("************************************************************\n");
    printf("*  Exercise 06-05: System Uptime / Bài tập: Uptime        *\n");
    printf("************************************************************\n\n");

    DemoGetTickCount64();
    DemoGetTickCount();
    DemoPerformanceCounter();
    DemoPrecisionTiming();
    PrintBootTime();

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
