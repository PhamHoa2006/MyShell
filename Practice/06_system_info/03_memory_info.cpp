/*
 * ============================================================================
 * Exercise 06-03: Memory Information / Thông tin bộ nhớ
 * ============================================================================
 * Description (EN): Uses GlobalMemoryStatusEx to retrieve detailed memory
 *                   statistics including physical RAM, virtual memory, and
 *                   page file usage. Displays with human-readable formatting
 *                   and usage bar graphs.
 *
 * Mô tả (VI):      Sử dụng GlobalMemoryStatusEx để lấy thông tin chi tiết
 *                   về bộ nhớ bao gồm RAM vật lý, bộ nhớ ảo, và sử dụng
 *                   page file. Hiển thị dạng dễ đọc với biểu đồ thanh.
 *
 * Key APIs:  GlobalMemoryStatusEx
 *
 * Compile:   g++ 03_memory_info.cpp -o 03_memory_info.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Hàm chuyển đổi byte sang đơn vị phù hợp (KB, MB, GB)
// Convert bytes to appropriate unit (KB, MB, GB)
// ============================================================================
void FormatBytes(DWORDLONG bytes, char* buffer, int bufSize)
{
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;
    const double TB = GB * 1024.0;

    if (bytes >= (DWORDLONG)(TB)) {
        snprintf(buffer, bufSize, "%.2f TB", (double)bytes / TB);
    } else if (bytes >= (DWORDLONG)(GB)) {
        snprintf(buffer, bufSize, "%.2f GB", (double)bytes / GB);
    } else if (bytes >= (DWORDLONG)(MB)) {
        snprintf(buffer, bufSize, "%.2f MB", (double)bytes / MB);
    } else if (bytes >= (DWORDLONG)(KB)) {
        snprintf(buffer, bufSize, "%.2f KB", (double)bytes / KB);
    } else {
        snprintf(buffer, bufSize, "%llu B", (unsigned long long)bytes);
    }
}

// ============================================================================
// Vẽ thanh tiến trình (progress bar) cho tỷ lệ sử dụng
// Draw a progress bar for usage percentage
// ============================================================================
void DrawProgressBar(double percentage, int width)
{
    int filled = (int)(percentage / 100.0 * width);
    if (filled > width) filled = width;
    if (filled < 0) filled = 0;

    printf("[");
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            printf("#");      // Đã sử dụng / Used
        } else {
            printf("-");      // Còn trống / Free
        }
    }
    printf("] %.1f%%", percentage);
}

// ============================================================================
// Hiển thị thông tin bộ nhớ chi tiết
// Display detailed memory information
// ============================================================================
void PrintMemoryInfo()
{
    printf("=== Memory Status / Trạng thái bộ nhớ ===\n\n");

    // MEMORYSTATUSEX: Cấu trúc chứa thông tin bộ nhớ mở rộng
    // MEMORYSTATUSEX: Extended memory status structure
    // QUAN TRỌNG: Phải set dwLength trước khi gọi API
    // IMPORTANT: Must set dwLength before calling the API
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    // GlobalMemoryStatusEx: Lấy thông tin bộ nhớ vật lý và ảo
    // GlobalMemoryStatusEx: Retrieves physical and virtual memory info
    // Phiên bản mở rộng của GlobalMemoryStatus (hỗ trợ >4GB RAM)
    // Extended version of GlobalMemoryStatus (supports >4GB RAM)
    if (!GlobalMemoryStatusEx(&memInfo)) {
        printf("  [ERROR] GlobalMemoryStatusEx failed / Lỗi lấy thông tin bộ nhớ!\n");
        printf("  Error code / Mã lỗi: %lu\n", GetLastError());
        return;
    }

    char buf1[64], buf2[64], buf3[64];

    // ============================================================
    // 1. Tỷ lệ sử dụng tổng thể / Overall memory load
    // ============================================================
    printf("  --- Overall Memory Load / Tải bộ nhớ tổng thể ---\n\n");

    // dwMemoryLoad: Phần trăm bộ nhớ vật lý đang được sử dụng (0-100)
    // dwMemoryLoad: Approximate percentage of physical memory in use (0-100)
    printf("  Memory Load / Tải bộ nhớ: ");
    DrawProgressBar((double)memInfo.dwMemoryLoad, 40);
    printf("\n\n");

    // ============================================================
    // 2. Bộ nhớ vật lý (RAM) / Physical Memory (RAM)
    // ============================================================
    printf("  --- Physical Memory (RAM) / Bộ nhớ vật lý (RAM) ---\n\n");

    // ullTotalPhys: Tổng dung lượng RAM vật lý (bytes)
    // ullTotalPhys: Total physical memory in bytes
    FormatBytes(memInfo.ullTotalPhys, buf1, sizeof(buf1));
    printf("  Total Physical / Tổng RAM:      %s (%llu bytes)\n",
           buf1, (unsigned long long)memInfo.ullTotalPhys);

    // ullAvailPhys: RAM vật lý còn trống
    // ullAvailPhys: Available physical memory
    FormatBytes(memInfo.ullAvailPhys, buf1, sizeof(buf1));
    printf("  Available / Còn trống:           %s (%llu bytes)\n",
           buf1, (unsigned long long)memInfo.ullAvailPhys);

    // Tính RAM đã dùng / Calculate used RAM
    DWORDLONG usedPhys = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    FormatBytes(usedPhys, buf1, sizeof(buf1));
    double physPercent = (double)usedPhys / (double)memInfo.ullTotalPhys * 100.0;
    printf("  Used / Đã dùng:                 %s (%.1f%%)\n", buf1, physPercent);

    printf("  Usage / Sử dụng:                ");
    DrawProgressBar(physPercent, 40);
    printf("\n\n");

    // ============================================================
    // 3. Page File (Bộ nhớ trao đổi) / Page File (Swap)
    // ============================================================
    printf("  --- Page File / Bộ nhớ trao đổi (Swap) ---\n\n");

    // ullTotalPageFile: Kích thước tối đa của page file
    // ullTotalPageFile: Maximum size of the page file
    // Bao gồm cả RAM vật lý / Includes physical RAM
    FormatBytes(memInfo.ullTotalPageFile, buf1, sizeof(buf1));
    printf("  Total Page File / Tổng page file:    %s\n", buf1);

    // ullAvailPageFile: Page file còn trống
    // ullAvailPageFile: Available page file space
    FormatBytes(memInfo.ullAvailPageFile, buf1, sizeof(buf1));
    printf("  Available / Còn trống:                %s\n", buf1);

    DWORDLONG usedPage = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    FormatBytes(usedPage, buf1, sizeof(buf1));
    double pagePercent = (memInfo.ullTotalPageFile > 0)
        ? (double)usedPage / (double)memInfo.ullTotalPageFile * 100.0
        : 0.0;
    printf("  Used / Đã dùng:                      %s (%.1f%%)\n", buf1, pagePercent);

    printf("  Usage / Sử dụng:                     ");
    DrawProgressBar(pagePercent, 40);
    printf("\n\n");

    // ============================================================
    // 4. Bộ nhớ ảo của tiến trình / Process Virtual Memory
    // ============================================================
    printf("  --- Virtual Memory (This Process) / Bộ nhớ ảo (tiến trình này) ---\n\n");

    // ullTotalVirtual: Tổng bộ nhớ ảo cho tiến trình hiện tại
    // ullTotalVirtual: Total virtual memory for the current process
    // Trên 64-bit: thường 128 TB; Trên 32-bit: thường 2 GB hoặc 3 GB
    // On 64-bit: typically 128 TB; On 32-bit: typically 2 GB or 3 GB
    FormatBytes(memInfo.ullTotalVirtual, buf1, sizeof(buf1));
    printf("  Total Virtual / Tổng ảo:        %s\n", buf1);

    // ullAvailVirtual: Bộ nhớ ảo còn trống cho tiến trình
    // ullAvailVirtual: Available virtual memory for the process
    FormatBytes(memInfo.ullAvailVirtual, buf1, sizeof(buf1));
    printf("  Available / Còn trống:           %s\n", buf1);

    DWORDLONG usedVirt = memInfo.ullTotalVirtual - memInfo.ullAvailVirtual;
    FormatBytes(usedVirt, buf1, sizeof(buf1));
    double virtPercent = (memInfo.ullTotalVirtual > 0)
        ? (double)usedVirt / (double)memInfo.ullTotalVirtual * 100.0
        : 0.0;
    printf("  Used / Đã dùng:                 %s (%.1f%%)\n", buf1, virtPercent);

    printf("  Usage / Sử dụng:                ");
    DrawProgressBar(virtPercent, 40);
    printf("\n\n");

    // ============================================================
    // 5. Bảng tổng hợp / Summary Table
    // ============================================================
    printf("  --- Summary Table / Bảng tổng hợp ---\n\n");
    printf("  %-22s  %12s  %12s  %12s  %8s\n",
           "Category / Loại", "Total", "Used", "Available", "Usage");
    printf("  %-22s  %12s  %12s  %12s  %8s\n",
           "----------------------", "------------", "------------",
           "------------", "--------");

    // Physical / Vật lý
    FormatBytes(memInfo.ullTotalPhys, buf1, sizeof(buf1));
    FormatBytes(usedPhys, buf2, sizeof(buf2));
    FormatBytes(memInfo.ullAvailPhys, buf3, sizeof(buf3));
    printf("  %-22s  %12s  %12s  %12s  %6.1f%%\n",
           "Physical / Vật lý", buf1, buf2, buf3, physPercent);

    // Page File
    FormatBytes(memInfo.ullTotalPageFile, buf1, sizeof(buf1));
    FormatBytes(usedPage, buf2, sizeof(buf2));
    FormatBytes(memInfo.ullAvailPageFile, buf3, sizeof(buf3));
    printf("  %-22s  %12s  %12s  %12s  %6.1f%%\n",
           "Page File / Trao đổi", buf1, buf2, buf3, pagePercent);

    // Virtual / Ảo
    FormatBytes(memInfo.ullTotalVirtual, buf1, sizeof(buf1));
    FormatBytes(usedVirt, buf2, sizeof(buf2));
    FormatBytes(memInfo.ullAvailVirtual, buf3, sizeof(buf3));
    printf("  %-22s  %12s  %12s  %12s  %6.1f%%\n\n",
           "Virtual / Ảo", buf1, buf2, buf3, virtPercent);
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    printf("************************************************************\n");
    printf("*  Exercise 06-03: Memory Info / Bài tập: Thông tin bộ nhớ *\n");
    printf("************************************************************\n\n");

    PrintMemoryInfo();

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
