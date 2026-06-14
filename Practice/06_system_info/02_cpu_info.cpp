/*
 * ============================================================================
 * Exercise 06-02: CPU Information / Thông tin CPU
 * ============================================================================
 * Description (EN): Uses GetSystemInfo and IsProcessorFeaturePresent to
 *                   retrieve detailed CPU/processor information including
 *                   architecture, core count, page size, and feature flags.
 *
 * Mô tả (VI):      Sử dụng GetSystemInfo và IsProcessorFeaturePresent để lấy
 *                   thông tin chi tiết về CPU/bộ xử lý bao gồm kiến trúc,
 *                   số lõi, kích thước trang, và các tính năng hỗ trợ.
 *
 * Key APIs:  GetSystemInfo, GetNativeSystemInfo, IsProcessorFeaturePresent
 *
 * Compile:   g++ 02_cpu_info.cpp -o 02_cpu_info.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Chuyển mã kiến trúc CPU thành chuỗi mô tả
// Convert CPU architecture code to descriptive string
// ============================================================================
const char* GetArchitectureName(WORD arch)
{
    // wProcessorArchitecture: Kiến trúc bộ xử lý
    // wProcessorArchitecture: Processor architecture identifier
    switch (arch) {
        case PROCESSOR_ARCHITECTURE_AMD64:   return "x64 (AMD64 / Intel 64)";
        case PROCESSOR_ARCHITECTURE_ARM:     return "ARM";
        case PROCESSOR_ARCHITECTURE_ARM64:   return "ARM64";
        case PROCESSOR_ARCHITECTURE_INTEL:   return "x86 (Intel 32-bit)";
        // Một số giá trị ít phổ biến hơn / Less common values
        case PROCESSOR_ARCHITECTURE_IA64:    return "IA-64 (Itanium)";
        case PROCESSOR_ARCHITECTURE_UNKNOWN: return "Unknown / Không xác định";
        default:                             return "Other / Khác";
    }
}

// ============================================================================
// In thông tin hệ thống cơ bản từ GetSystemInfo
// Print basic system info from GetSystemInfo
// ============================================================================
void PrintSystemInfo()
{
    printf("=== 1. GetSystemInfo - Basic System Info / Thông tin cơ bản ===\n\n");

    // GetSystemInfo: Lấy thông tin về hệ thống phần cứng
    // GetSystemInfo: Retrieves information about the current system hardware
    // Trên hệ thống WoW64, trả về thông tin "emulated" cho tiến trình 32-bit
    // On WoW64 systems, returns "emulated" info for 32-bit processes
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // ---- Kiến trúc CPU / CPU Architecture ----
    printf("  Processor Architecture / Kiến trúc CPU:\n");
    printf("    Code / Mã:          %d\n", si.wProcessorArchitecture);
    printf("    Name / Tên:         %s\n\n", GetArchitectureName(si.wProcessorArchitecture));

    // ---- Số bộ xử lý / Number of processors ----
    // dwNumberOfProcessors: Số lõi logic (kể cả Hyper-Threading)
    // dwNumberOfProcessors: Number of logical processors (including HT)
    printf("  Number of Processors / Số bộ xử lý:\n");
    printf("    Logical CPUs / CPU logic:  %lu\n\n", si.dwNumberOfProcessors);

    // ---- Active Processor Mask ----
    // dwActiveProcessorMask: Bitmask chỉ ra CPU nào đang hoạt động
    // dwActiveProcessorMask: Bitmask indicating which CPUs are active
    printf("  Active Processor Mask / Mặt nạ CPU hoạt động:\n");
    printf("    Mask value / Giá trị: 0x%p\n", (void*)(uintptr_t)si.dwActiveProcessorMask);
    printf("    Active CPUs / CPU hoạt động: ");
    for (int i = 0; i < 64; i++) {
        if (si.dwActiveProcessorMask & ((DWORD_PTR)1 << i)) {
            printf("%d ", i);
        }
    }
    printf("\n\n");

    // ---- Kích thước trang bộ nhớ / Memory page size ----
    // dwPageSize: Kích thước trang bộ nhớ (thường 4096 bytes = 4 KB)
    // dwPageSize: Size of a memory page (typically 4096 bytes = 4 KB)
    printf("  Memory Page Size / Kích thước trang nhớ:\n");
    printf("    %lu bytes (%lu KB)\n\n", si.dwPageSize, si.dwPageSize / 1024);

    // ---- Vùng nhớ ảo của ứng dụng / Application virtual memory range ----
    // lpMinimumApplicationAddress: Địa chỉ ảo thấp nhất mà ứng dụng có thể dùng
    // lpMaximumApplicationAddress: Địa chỉ ảo cao nhất mà ứng dụng có thể dùng
    printf("  Virtual Address Range / Phạm vi địa chỉ ảo:\n");
    printf("    Min Application Address / Địa chỉ min: 0x%p\n",
           si.lpMinimumApplicationAddress);
    printf("    Max Application Address / Địa chỉ max: 0x%p\n\n",
           si.lpMaximumApplicationAddress);

    // ---- Allocation Granularity ----
    // dwAllocationGranularity: Đơn vị nhỏ nhất để cấp phát bộ nhớ ảo
    // dwAllocationGranularity: Minimum granularity for VirtualAlloc
    // Thường là 64 KB (65536 bytes)
    // Usually 64 KB (65536 bytes)
    printf("  Allocation Granularity / Đơn vị cấp phát:\n");
    printf("    %lu bytes (%lu KB)\n\n", si.dwAllocationGranularity,
           si.dwAllocationGranularity / 1024);

    // ---- Processor level & revision ----
    // wProcessorLevel: Mức kiến trúc CPU (phụ thuộc vào architecture)
    // wProcessorRevision: Bản revision cụ thể
    printf("  Processor Level / Mức CPU:    %d\n", si.wProcessorLevel);
    printf("  Processor Revision / Bản sửa: 0x%04X\n\n", si.wProcessorRevision);
}

// ============================================================================
// In thông tin hệ thống gốc (native) - hữu ích trên WoW64
// Print native system info - useful on WoW64
// ============================================================================
void PrintNativeSystemInfo()
{
    printf("=== 2. GetNativeSystemInfo - Native Info (WoW64) ===\n\n");

    // GetNativeSystemInfo: Trả về thông tin hệ thống thực (native)
    // GetNativeSystemInfo: Returns actual (native) system info
    // Hữu ích khi chạy tiến trình 32-bit trên Windows 64-bit (WoW64)
    // Useful when running 32-bit process on 64-bit Windows (WoW64)
    SYSTEM_INFO nsi;
    GetNativeSystemInfo(&nsi);

    printf("  Native Architecture / Kiến trúc gốc: %s\n",
           GetArchitectureName(nsi.wProcessorArchitecture));
    printf("  Native Processors / CPU gốc:         %lu\n",
           nsi.dwNumberOfProcessors);
    printf("  Native Page Size / Trang nhớ gốc:    %lu bytes\n\n",
           nsi.dwPageSize);
}

// ============================================================================
// Kiểm tra các tính năng CPU
// Check CPU features
// ============================================================================
void PrintProcessorFeatures()
{
    printf("=== 3. IsProcessorFeaturePresent - CPU Features / Tính năng CPU ===\n\n");

    // IsProcessorFeaturePresent: Kiểm tra CPU có hỗ trợ tính năng cụ thể không
    // IsProcessorFeaturePresent: Checks if the CPU supports a specific feature
    // Trả về TRUE nếu hỗ trợ, FALSE nếu không
    // Returns TRUE if supported, FALSE if not

    // Cấu trúc để lưu tên và mã tính năng / Structure for feature name and code
    struct FeatureInfo {
        DWORD   feature;
        const char* name;
        const char* description;
    };

    FeatureInfo features[] = {
        { PF_MMX_INSTRUCTIONS_AVAILABLE,
          "PF_MMX_INSTRUCTIONS_AVAILABLE",
          "MMX instructions / Lệnh MMX" },
        { PF_XMMI_INSTRUCTIONS_AVAILABLE,
          "PF_XMMI_INSTRUCTIONS_AVAILABLE",
          "SSE instructions / Lệnh SSE" },
        { PF_XMMI64_INSTRUCTIONS_AVAILABLE,
          "PF_XMMI64_INSTRUCTIONS_AVAILABLE",
          "SSE2 instructions / Lệnh SSE2" },
        { PF_SSE3_INSTRUCTIONS_AVAILABLE,
          "PF_SSE3_INSTRUCTIONS_AVAILABLE",
          "SSE3 instructions / Lệnh SSE3" },
        { PF_NX_ENABLED,
          "PF_NX_ENABLED",
          "NX (No-Execute) bit / Bit NX" },
        { PF_COMPARE_EXCHANGE128,
          "PF_COMPARE_EXCHANGE128",
          "128-bit CAS / CAS 128-bit" },
        { PF_COMPARE64_EXCHANGE128,
          "PF_COMPARE64_EXCHANGE128",
          "64-bit CmpXchg16b / CmpXchg16b 64-bit" },
        { PF_RDTSC_INSTRUCTION_AVAILABLE,
          "PF_RDTSC_INSTRUCTION_AVAILABLE",
          "RDTSC instruction / Lệnh RDTSC" },
        { PF_PAE_ENABLED,
          "PF_PAE_ENABLED",
          "PAE enabled / PAE được bật" },
        { PF_SECOND_LEVEL_ADDRESS_TRANSLATION,
          "PF_SECOND_LEVEL_ADDRESS_TRANSLATION",
          "SLAT (Hyper-V) / SLAT (ảo hóa)" },
        { PF_VIRT_FIRMWARE_ENABLED,
          "PF_VIRT_FIRMWARE_ENABLED",
          "Virtualization in firmware / Ảo hóa phần cứng" },
        { PF_RDRAND_INSTRUCTION_AVAILABLE,
          "PF_RDRAND_INSTRUCTION_AVAILABLE",
          "RDRAND instruction / Lệnh RDRAND" },
#ifdef PF_SSSE3_INSTRUCTIONS_AVAILABLE
        { PF_SSSE3_INSTRUCTIONS_AVAILABLE,
          "PF_SSSE3_INSTRUCTIONS_AVAILABLE",
          "SSSE3 instructions / Lệnh SSSE3" },
#endif
#ifdef PF_SSE4_1_INSTRUCTIONS_AVAILABLE
        { PF_SSE4_1_INSTRUCTIONS_AVAILABLE,
          "PF_SSE4_1_INSTRUCTIONS_AVAILABLE",
          "SSE4.1 instructions / Lệnh SSE4.1" },
#endif
#ifdef PF_SSE4_2_INSTRUCTIONS_AVAILABLE
        { PF_SSE4_2_INSTRUCTIONS_AVAILABLE,
          "PF_SSE4_2_INSTRUCTIONS_AVAILABLE",
          "SSE4.2 instructions / Lệnh SSE4.2" },
#endif
#ifdef PF_AVX_INSTRUCTIONS_AVAILABLE
        { PF_AVX_INSTRUCTIONS_AVAILABLE,
          "PF_AVX_INSTRUCTIONS_AVAILABLE",
          "AVX instructions / Lệnh AVX" },
#endif
#ifdef PF_AVX2_INSTRUCTIONS_AVAILABLE
        { PF_AVX2_INSTRUCTIONS_AVAILABLE,
          "PF_AVX2_INSTRUCTIONS_AVAILABLE",
          "AVX2 instructions / Lệnh AVX2" },
#endif
    };

    int count = sizeof(features) / sizeof(features[0]);
    printf("  %-45s  %-8s  %s\n", "Feature / Tính năng", "Status", "Description / Mô tả");
    printf("  %-45s  %-8s  %s\n",
           "---------------------------------------------", "--------",
           "---------------------------------------");

    for (int i = 0; i < count; i++) {
        BOOL present = IsProcessorFeaturePresent(features[i].feature);
        printf("  %-45s  [%-4s]    %s\n",
               features[i].name,
               present ? "YES" : "NO",
               features[i].description);
    }
    printf("\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    printf("************************************************************\n");
    printf("*  Exercise 06-02: CPU Info / Bài tập: Thông tin CPU       *\n");
    printf("************************************************************\n\n");

    PrintSystemInfo();
    PrintNativeSystemInfo();
    PrintProcessorFeatures();

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
