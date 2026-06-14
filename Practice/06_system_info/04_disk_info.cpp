/*
 * ============================================================================
 * Exercise 06-04: Disk Information / Thông tin ổ đĩa
 * ============================================================================
 * Description (EN): Enumerates all logical drives, identifies drive types,
 *                   retrieves disk space information, and volume details.
 *                   Demonstrates a complete disk inventory tool.
 *
 * Mô tả (VI):      Liệt kê tất cả ổ đĩa logic, xác định loại ổ, lấy thông
 *                   tin dung lượng đĩa, và chi tiết volume. Minh họa công cụ
 *                   kiểm kê ổ đĩa hoàn chỉnh.
 *
 * Key APIs:  GetLogicalDrives, GetLogicalDriveStrings, GetDriveType,
 *            GetDiskFreeSpaceEx, GetVolumeInformation
 *
 * Compile:   g++ 04_disk_info.cpp -o 04_disk_info.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Chuyển đổi byte sang đơn vị phù hợp / Convert bytes to human-readable
// ============================================================================
void FormatBytes(ULONGLONG bytes, char* buffer, int bufSize)
{
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;
    const double TB = GB * 1024.0;

    if (bytes >= (ULONGLONG)(TB)) {
        snprintf(buffer, bufSize, "%8.2f TB", (double)bytes / TB);
    } else if (bytes >= (ULONGLONG)(GB)) {
        snprintf(buffer, bufSize, "%8.2f GB", (double)bytes / GB);
    } else if (bytes >= (ULONGLONG)(MB)) {
        snprintf(buffer, bufSize, "%8.2f MB", (double)bytes / MB);
    } else if (bytes >= (ULONGLONG)(KB)) {
        snprintf(buffer, bufSize, "%8.2f KB", (double)bytes / KB);
    } else {
        snprintf(buffer, bufSize, "%8llu  B", (unsigned long long)bytes);
    }
}

// ============================================================================
// Lấy tên loại ổ đĩa / Get drive type name
// ============================================================================
const char* GetDriveTypeName(UINT driveType)
{
    // GetDriveType trả về các giá trị sau:
    // GetDriveType returns the following values:
    switch (driveType) {
        case DRIVE_UNKNOWN:     return "Unknown / Không xác định";
        case DRIVE_NO_ROOT_DIR: return "No Root Dir / Không có thư mục gốc";
        case DRIVE_REMOVABLE:   return "Removable / Ổ di động (USB, SD)";
        case DRIVE_FIXED:       return "Fixed / Ổ cứng (HDD/SSD)";
        case DRIVE_REMOTE:      return "Network / Ổ mạng";
        case DRIVE_CDROM:       return "CD-ROM / Ổ đĩa quang";
        case DRIVE_RAMDISK:     return "RAM Disk / Ổ đĩa RAM";
        default:                return "Other / Khác";
    }
}

// ============================================================================
// Demo 1: GetLogicalDrives - Bitmask các ổ đĩa
// Demo 1: GetLogicalDrives - Bitmask of available drives
// ============================================================================
void DemoGetLogicalDrives()
{
    printf("=== 1. GetLogicalDrives - Drive Bitmask / Bitmask ổ đĩa ===\n\n");

    // GetLogicalDrives: Trả về bitmask 32-bit, mỗi bit tương ứng 1 ổ
    // GetLogicalDrives: Returns a 32-bit bitmask, each bit = one drive
    // Bit 0 = A:, Bit 1 = B:, Bit 2 = C:, ...
    DWORD driveMask = GetLogicalDrives();

    if (driveMask == 0) {
        printf("  [ERROR] GetLogicalDrives failed / Lỗi: %lu\n\n", GetLastError());
        return;
    }

    printf("  Bitmask: 0x%08lX (binary: ", driveMask);
    for (int i = 25; i >= 0; i--) {
        printf("%d", (driveMask >> i) & 1);
        if (i % 4 == 0 && i > 0) printf(" ");
    }
    printf(")\n");

    printf("  Available drives / Ổ đĩa có sẵn: ");
    for (int i = 0; i < 26; i++) {
        if (driveMask & (1 << i)) {
            printf("%c: ", 'A' + i);
        }
    }
    printf("\n\n");
}

// ============================================================================
// Demo 2: Chi tiết từng ổ đĩa
// Demo 2: Detailed info for each drive
// ============================================================================
void PrintDriveDetails()
{
    printf("=== 2. Drive Details / Chi tiết từng ổ đĩa ===\n\n");

    // GetLogicalDriveStrings: Lấy danh sách ổ đĩa dạng chuỗi
    // GetLogicalDriveStrings: Gets drive list as strings
    // Kết quả: "A:\\\0C:\\\0D:\\\0\0" (double-null terminated)
    char driveStrings[256];
    DWORD len = GetLogicalDriveStringsA(sizeof(driveStrings) - 1, driveStrings);

    if (len == 0) {
        printf("  [ERROR] GetLogicalDriveStrings failed / Lỗi: %lu\n\n",
               GetLastError());
        return;
    }

    // Duyệt qua từng chuỗi ổ đĩa (cách nhau bởi null character)
    // Iterate through each drive string (separated by null characters)
    char* drive = driveStrings;
    int driveNum = 1;

    while (*drive) {
        printf("  --- Drive #%d: %s ---\n", driveNum++, drive);

        // ---- Loại ổ đĩa / Drive type ----
        // GetDriveType: Xác định loại ổ đĩa
        // GetDriveType: Determines the type of drive
        UINT driveType = GetDriveTypeA(drive);
        printf("    Type / Loại:         %s\n", GetDriveTypeName(driveType));

        // ---- Thông tin Volume / Volume information ----
        // GetVolumeInformation: Lấy tên volume, serial number, file system
        // GetVolumeInformation: Gets volume name, serial number, file system
        char volumeName[MAX_PATH + 1]  = {0};
        char fileSystem[MAX_PATH + 1]  = {0};
        DWORD serialNumber    = 0;
        DWORD maxComponentLen = 0;
        DWORD fsFlags         = 0;

        if (GetVolumeInformationA(drive,
                                   volumeName, sizeof(volumeName),
                                   &serialNumber,
                                   &maxComponentLen,
                                   &fsFlags,
                                   fileSystem, sizeof(fileSystem)))
        {
            printf("    Volume Name / Tên:   %s\n",
                   volumeName[0] ? volumeName : "(no label / không tên)");
            printf("    Serial Number / Số:  %04lX-%04lX\n",
                   (serialNumber >> 16) & 0xFFFF, serialNumber & 0xFFFF);
            printf("    File System / Hệ thống: %s\n", fileSystem);
            printf("    Max Path Component / Tên tối đa: %lu chars\n", maxComponentLen);

            // Cờ đặc tính file system / File system feature flags
            printf("    Features / Đặc tính:\n");
            if (fsFlags & FILE_CASE_SENSITIVE_SEARCH)
                printf("      - Case-sensitive search / Tìm phân biệt hoa thường\n");
            if (fsFlags & FILE_FILE_COMPRESSION)
                printf("      - File compression / Nén file\n");
            if (fsFlags & FILE_SUPPORTS_ENCRYPTION)
                printf("      - Encryption / Mã hóa\n");
            if (fsFlags & FILE_READ_ONLY_VOLUME)
                printf("      - Read-only / Chỉ đọc\n");
            if (fsFlags & FILE_VOLUME_QUOTAS)
                printf("      - Quotas / Hạn mức\n");
        } else {
            printf("    [Volume info unavailable / Không lấy được thông tin volume]\n");
        }

        // ---- Dung lượng đĩa / Disk space ----
        // GetDiskFreeSpaceEx: Lấy thông tin dung lượng ổ đĩa
        // GetDiskFreeSpaceEx: Retrieves disk space information
        // Hỗ trợ ổ đĩa lớn hơn 2GB (khác với GetDiskFreeSpace)
        // Supports disks larger than 2GB (unlike GetDiskFreeSpace)
        ULARGE_INTEGER freeBytesAvail;    // Byte trống cho user / Free bytes for user
        ULARGE_INTEGER totalBytes;        // Tổng dung lượng / Total capacity
        ULARGE_INTEGER totalFreeBytes;    // Tổng byte trống / Total free bytes

        if (GetDiskFreeSpaceExA(drive,
                                 &freeBytesAvail,
                                 &totalBytes,
                                 &totalFreeBytes))
        {
            char bufTotal[64], bufFree[64], bufAvail[64], bufUsed[64];
            ULONGLONG usedBytes = totalBytes.QuadPart - totalFreeBytes.QuadPart;

            FormatBytes(totalBytes.QuadPart,     bufTotal, sizeof(bufTotal));
            FormatBytes(totalFreeBytes.QuadPart,  bufFree,  sizeof(bufFree));
            FormatBytes(freeBytesAvail.QuadPart,  bufAvail, sizeof(bufAvail));
            FormatBytes(usedBytes,                bufUsed,  sizeof(bufUsed));

            printf("    Capacity / Dung lượng:\n");
            printf("      Total / Tổng:                %s\n", bufTotal);
            printf("      Used  / Đã dùng:             %s\n", bufUsed);
            printf("      Free (total) / Trống (tổng): %s\n", bufFree);
            printf("      Free (user)  / Trống (user): %s\n", bufAvail);

            // freeBytesAvail vs totalFreeBytes:
            // freeBytesAvail: Không gian trống mà user hiện tại có thể dùng
            //                 (có thể bị giới hạn bởi disk quota)
            // totalFreeBytes: Tổng không gian trống trên ổ đĩa
            // freeBytesAvail: Free space available to current user
            //                 (may be limited by disk quotas)
            // totalFreeBytes: Total free space on the drive

            // Vẽ thanh sử dụng / Draw usage bar
            double usedPercent = (totalBytes.QuadPart > 0)
                ? (double)usedBytes / (double)totalBytes.QuadPart * 100.0
                : 0.0;
            printf("      Usage / Sử dụng: [");
            int barWidth = 30;
            int filled = (int)(usedPercent / 100.0 * barWidth);
            for (int i = 0; i < barWidth; i++) {
                printf("%c", (i < filled) ? '#' : '-');
            }
            printf("] %.1f%%\n", usedPercent);
        } else {
            printf("    [Disk space unavailable / Không lấy được dung lượng]\n");
            printf("    (Drive may not be ready / Ổ có thể chưa sẵn sàng)\n");
        }

        printf("\n");

        // Di chuyển đến chuỗi ổ đĩa tiếp theo
        // Move to the next drive string
        drive += strlen(drive) + 1;
    }
}

// ============================================================================
// Demo 3: Bảng tổng hợp tất cả ổ đĩa
// Demo 3: Summary table of all drives
// ============================================================================
void PrintSummaryTable()
{
    printf("=== 3. Summary Table / Bảng tổng hợp ===\n\n");

    printf("  %-6s %-14s %-10s %12s %12s %12s %8s\n",
           "Drive", "Type/Loại", "FS", "Total", "Used", "Free", "Usage");
    printf("  %-6s %-14s %-10s %12s %12s %12s %8s\n",
           "------", "--------------", "----------",
           "------------", "------------", "------------", "--------");

    char driveStrings[256];
    GetLogicalDriveStringsA(sizeof(driveStrings) - 1, driveStrings);
    char* drive = driveStrings;

    while (*drive) {
        UINT driveType = GetDriveTypeA(drive);

        // Tên ngắn cho loại ổ / Short type name
        const char* typeShort;
        switch (driveType) {
            case DRIVE_REMOVABLE: typeShort = "Removable"; break;
            case DRIVE_FIXED:     typeShort = "Fixed"; break;
            case DRIVE_REMOTE:    typeShort = "Network"; break;
            case DRIVE_CDROM:     typeShort = "CD-ROM"; break;
            case DRIVE_RAMDISK:   typeShort = "RAMDisk"; break;
            default:              typeShort = "Unknown"; break;
        }

        char fileSystem[32] = "N/A";
        GetVolumeInformationA(drive, NULL, 0, NULL, NULL, NULL,
                               fileSystem, sizeof(fileSystem));

        ULARGE_INTEGER freeBytesAvail, totalBytes, totalFreeBytes;
        if (GetDiskFreeSpaceExA(drive, &freeBytesAvail, &totalBytes, &totalFreeBytes)) {
            char bufTotal[64], bufUsed[64], bufFree[64];
            ULONGLONG used = totalBytes.QuadPart - totalFreeBytes.QuadPart;
            FormatBytes(totalBytes.QuadPart, bufTotal, sizeof(bufTotal));
            FormatBytes(used, bufUsed, sizeof(bufUsed));
            FormatBytes(totalFreeBytes.QuadPart, bufFree, sizeof(bufFree));
            double percent = (totalBytes.QuadPart > 0)
                ? (double)used / (double)totalBytes.QuadPart * 100.0 : 0.0;

            printf("  %-6s %-14s %-10s %12s %12s %12s %6.1f%%\n",
                   drive, typeShort, fileSystem,
                   bufTotal, bufUsed, bufFree, percent);
        } else {
            printf("  %-6s %-14s %-10s %12s %12s %12s %8s\n",
                   drive, typeShort, fileSystem,
                   "N/A", "N/A", "N/A", "N/A");
        }

        drive += strlen(drive) + 1;
    }
    printf("\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    printf("************************************************************\n");
    printf("*  Exercise 06-04: Disk Info / Bài tập: Thông tin ổ đĩa   *\n");
    printf("************************************************************\n\n");

    DemoGetLogicalDrives();
    PrintDriveDetails();
    PrintSummaryTable();

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
