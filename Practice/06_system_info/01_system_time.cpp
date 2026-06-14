/*
 * ============================================================================
 * Exercise 06-01: System Time / Thời gian hệ thống
 * ============================================================================
 * Description (EN): Demonstrates Windows time APIs to retrieve and format
 *                   system time, local time, timezone information, and
 *                   convert between time representations.
 *
 * Mô tả (VI):      Minh họa các API thời gian của Windows để lấy và định dạng
 *                   thời gian hệ thống, giờ địa phương, thông tin múi giờ,
 *                   và chuyển đổi giữa các biểu diễn thời gian.
 *
 * Key APIs:  GetSystemTime, GetLocalTime, GetTimeZoneInformation,
 *            SystemTimeToFileTime, FileTimeToSystemTime,
 *            GetDateFormatEx, GetTimeFormatEx
 *
 * Compile:   g++ 01_system_time.cpp -o 01_system_time.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// Tên các ngày trong tuần / Day-of-week names
const char* DAY_NAMES[] = {
    "Sunday / Chủ nhật",
    "Monday / Thứ Hai",
    "Tuesday / Thứ Ba",
    "Wednesday / Thứ Tư",
    "Thursday / Thứ Năm",
    "Friday / Thứ Sáu",
    "Saturday / Thứ Bảy"
};

// Tên các tháng / Month names
const char* MONTH_NAMES[] = {
    "", // index 0 unused
    "January / Tháng 1",   "February / Tháng 2",  "March / Tháng 3",
    "April / Tháng 4",     "May / Tháng 5",       "June / Tháng 6",
    "July / Tháng 7",      "August / Tháng 8",    "September / Tháng 9",
    "October / Tháng 10",  "November / Tháng 11", "December / Tháng 12"
};

// ============================================================================
// In thời gian UTC (giờ quốc tế)
// Print UTC time (Coordinated Universal Time)
// ============================================================================
void PrintSystemTime()
{
    printf("=== 1. GetSystemTime - UTC Time / Giờ UTC ===\n\n");

    // GetSystemTime: Lấy thời gian UTC hiện tại
    // GetSystemTime: Retrieves the current UTC date and time
    // Kết quả trả về qua cấu trúc SYSTEMTIME
    // Result returned via SYSTEMTIME structure
    SYSTEMTIME st;
    GetSystemTime(&st);

    printf("  Raw SYSTEMTIME fields / Các trường SYSTEMTIME:\n");
    printf("    wYear         = %d\n", st.wYear);
    printf("    wMonth        = %d (%s)\n", st.wMonth, MONTH_NAMES[st.wMonth]);
    printf("    wDay          = %d\n", st.wDay);
    printf("    wDayOfWeek    = %d (%s)\n", st.wDayOfWeek, DAY_NAMES[st.wDayOfWeek]);
    printf("    wHour         = %d\n", st.wHour);
    printf("    wMinute       = %d\n", st.wMinute);
    printf("    wSecond       = %d\n", st.wSecond);
    printf("    wMilliseconds = %d\n", st.wMilliseconds);

    // Định dạng đẹp / Nicely formatted
    printf("\n  Formatted UTC / Định dạng UTC:\n");
    printf("    %04d-%02d-%02d %02d:%02d:%02d.%03d UTC\n",
           st.wYear, st.wMonth, st.wDay,
           st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    printf("    %s, %s %d, %d\n\n",
           DAY_NAMES[st.wDayOfWeek], MONTH_NAMES[st.wMonth],
           st.wDay, st.wYear);
}

// ============================================================================
// In giờ địa phương
// Print local time
// ============================================================================
void PrintLocalTime()
{
    printf("=== 2. GetLocalTime - Local Time / Giờ địa phương ===\n\n");

    // GetLocalTime: Lấy thời gian theo múi giờ địa phương
    // GetLocalTime: Retrieves the current local date and time
    // Khác với GetSystemTime, kết quả đã tính theo timezone của máy
    // Unlike GetSystemTime, the result already accounts for the machine's timezone
    SYSTEMTIME lt;
    GetLocalTime(&lt);

    printf("  Formatted Local Time / Giờ địa phương:\n");
    printf("    %04d-%02d-%02d %02d:%02d:%02d.%03d\n",
           lt.wYear, lt.wMonth, lt.wDay,
           lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
    printf("    %s, %s %d, %d\n\n",
           DAY_NAMES[lt.wDayOfWeek], MONTH_NAMES[lt.wMonth],
           lt.wDay, lt.wYear);
}

// ============================================================================
// Hiển thị thông tin múi giờ
// Display timezone information
// ============================================================================
void PrintTimezoneInfo()
{
    printf("=== 3. GetTimeZoneInformation - Timezone / Múi giờ ===\n\n");

    // GetTimeZoneInformation: Lấy thông tin múi giờ hiện tại
    // GetTimeZoneInformation: Retrieves the current timezone settings
    // Trả về TIME_ZONE_ID_STANDARD hoặc TIME_ZONE_ID_DAYLIGHT
    // Returns TIME_ZONE_ID_STANDARD or TIME_ZONE_ID_DAYLIGHT
    TIME_ZONE_INFORMATION tzi;
    DWORD result = GetTimeZoneInformation(&tzi);

    // Bias: số phút chênh lệch giữa UTC và giờ địa phương
    // Bias: difference in minutes between UTC and local time
    // Ví dụ: Việt Nam = -420 phút (UTC+7), nghĩa là UTC - (-420) = UTC + 420 phút = UTC + 7 giờ
    // Example: Vietnam = -420 minutes (UTC+7), meaning UTC - (-420) = UTC + 420 min = UTC + 7 hours
    printf("  Bias (minutes from UTC) / Độ lệch: %ld minutes\n", tzi.Bias);
    printf("  UTC offset / Múi giờ UTC:           UTC%+.1f\n", -(double)tzi.Bias / 60.0);

    // Tên múi giờ chuẩn (wchar_t) / Standard timezone name (wchar_t)
    printf("  Standard Name / Tên chuẩn:          %ls\n", tzi.StandardName);
    printf("  Standard Bias / Lệch chuẩn:         %ld minutes\n", tzi.StandardBias);

    // Tên múi giờ mùa hè / Daylight saving timezone name
    printf("  Daylight Name / Tên DST:            %ls\n", tzi.DaylightName);
    printf("  Daylight Bias / Lệch DST:           %ld minutes\n", tzi.DaylightBias);

    // Trạng thái hiện tại / Current state
    const char* state;
    switch (result) {
        case TIME_ZONE_ID_STANDARD: state = "Standard Time / Giờ chuẩn"; break;
        case TIME_ZONE_ID_DAYLIGHT: state = "Daylight Saving Time / Giờ mùa hè"; break;
        case TIME_ZONE_ID_UNKNOWN:  state = "Unknown / Không xác định"; break;
        default:                    state = "Error / Lỗi"; break;
    }
    printf("  Current State / Trạng thái:         %s\n\n", state);
}

// ============================================================================
// Chuyển đổi giữa SYSTEMTIME và FILETIME
// Convert between SYSTEMTIME and FILETIME
// ============================================================================
void DemoTimeConversion()
{
    printf("=== 4. Time Conversion / Chuyển đổi thời gian ===\n\n");

    SYSTEMTIME st;
    GetSystemTime(&st);

    // SystemTimeToFileTime: Chuyển SYSTEMTIME → FILETIME
    // SystemTimeToFileTime: Convert SYSTEMTIME → FILETIME
    // FILETIME: Số khoảng 100-nanosecond từ 1/1/1601
    // FILETIME: Number of 100-nanosecond intervals since January 1, 1601
    FILETIME ft;
    if (SystemTimeToFileTime(&st, &ft)) {
        printf("  SYSTEMTIME -> FILETIME:\n");
        printf("    dwHighDateTime = 0x%08lX\n", ft.dwHighDateTime);
        printf("    dwLowDateTime  = 0x%08lX\n", ft.dwLowDateTime);

        // Ghép 2 DWORD thành số 64-bit
        // Combine two DWORDs into a 64-bit number
        ULARGE_INTEGER uli;
        uli.HighPart = ft.dwHighDateTime;
        uli.LowPart  = ft.dwLowDateTime;
        printf("    As 64-bit value / Giá trị 64-bit: %llu\n", uli.QuadPart);
        printf("    (100-nanosecond intervals since Jan 1, 1601)\n");
        printf("    (Số khoảng 100 nano giây từ 01/01/1601)\n\n");

        // FileTimeToSystemTime: Chuyển ngược lại FILETIME → SYSTEMTIME
        // FileTimeToSystemTime: Convert back FILETIME → SYSTEMTIME
        SYSTEMTIME st2;
        if (FileTimeToSystemTime(&ft, &st2)) {
            printf("  FILETIME -> SYSTEMTIME (round-trip / khứ hồi):\n");
            printf("    %04d-%02d-%02d %02d:%02d:%02d.%03d UTC\n",
                   st2.wYear, st2.wMonth, st2.wDay,
                   st2.wHour, st2.wMinute, st2.wSecond, st2.wMilliseconds);
            printf("    [Match / Khớp: %s]\n\n",
                   (st.wYear == st2.wYear && st.wMonth == st2.wMonth &&
                    st.wDay == st2.wDay && st.wHour == st2.wHour &&
                    st.wMinute == st2.wMinute && st.wSecond == st2.wSecond)
                   ? "YES / Có" : "NO / Không");
        }
    } else {
        printf("  [ERROR] SystemTimeToFileTime failed / Lỗi chuyển đổi: %lu\n\n",
               GetLastError());
    }
}

// ============================================================================
// Tính khoảng cách giữa 2 thời điểm bằng FILETIME
// Calculate elapsed time between two points using FILETIME
// ============================================================================
void DemoElapsedTime()
{
    printf("=== 5. Elapsed Time Calculation / Tính thời gian trôi qua ===\n\n");

    // Lấy thời điểm bắt đầu / Get start time
    SYSTEMTIME stStart;
    GetSystemTime(&stStart);
    FILETIME ftStart;
    SystemTimeToFileTime(&stStart, &ftStart);

    printf("  Sleeping for 1.5 seconds... / Đang ngủ 1.5 giây...\n");
    Sleep(1500);

    // Lấy thời điểm kết thúc / Get end time
    SYSTEMTIME stEnd;
    GetSystemTime(&stEnd);
    FILETIME ftEnd;
    SystemTimeToFileTime(&stEnd, &ftEnd);

    // Tính khoảng cách / Calculate difference
    ULARGE_INTEGER uliStart, uliEnd;
    uliStart.HighPart = ftStart.dwHighDateTime;
    uliStart.LowPart  = ftStart.dwLowDateTime;
    uliEnd.HighPart   = ftEnd.dwHighDateTime;
    uliEnd.LowPart    = ftEnd.dwLowDateTime;

    // Kết quả tính bằng 100-nanosecond intervals
    // Result in 100-nanosecond intervals
    ULONGLONG elapsed = uliEnd.QuadPart - uliStart.QuadPart;
    double elapsedMs = (double)elapsed / 10000.0;  // Convert to milliseconds

    printf("  Start / Bắt đầu: %02d:%02d:%02d.%03d\n",
           stStart.wHour, stStart.wMinute, stStart.wSecond, stStart.wMilliseconds);
    printf("  End   / Kết thúc: %02d:%02d:%02d.%03d\n",
           stEnd.wHour, stEnd.wMinute, stEnd.wSecond, stEnd.wMilliseconds);
    printf("  Elapsed / Đã trôi qua: %.2f ms (expected ~1500 ms)\n", elapsedMs);
    printf("  Raw 100ns intervals / Khoảng 100ns: %llu\n\n", elapsed);
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    printf("************************************************************\n");
    printf("*  Exercise 06-01: System Time / Bài tập: Thời gian hệ thống  *\n");
    printf("************************************************************\n\n");

    PrintSystemTime();
    PrintLocalTime();
    PrintTimezoneInfo();
    DemoTimeConversion();
    DemoElapsedTime();

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
