/*
 * Case Study 03: File System Watcher
 * Description: Monitors a specific directory for changes using the FindFirstChangeNotification API.
 *              It loops infinitely, printing notifications whenever a file is created, deleted,
 *              or modified within the watched directory.
 *
 * Key APIs covered:
 * - FindFirstChangeNotification: Sets up a directory to be watched.
 * - FindNextChangeNotification: Resets the watch after an event occurs.
 * - WaitForSingleObject: Waits for the directory change event to be signaled.
 *
 * Tiếng Việt:
 * Case Study 03: Theo dõi thay đổi thư mục (File Watcher)
 * Mô tả: Giám sát một thư mục, phát hiện các thay đổi (tạo mới, xoá, chỉnh sửa file)
 *        sử dụng hàm FindFirstChangeNotification.
 */

#include <windows.h>
#include <iostream>
#include <string>

int main() {
    // Thư mục cần theo dõi (mặc định là thư mục hiện tại) 
    // Directory to watch (current directory by default)
    const char* watchPath = ".\\";
    
    std::cout << "Watching directory: " << watchPath << " for changes..." << std::endl;
    std::cout << "Press Ctrl+C to exit." << std::endl;

    // Thiết lập đối tượng thông báo / Set up notification handle
    // Chúng ta theo dõi thay đổi tên file và thư mục, thay đổi thuộc tính, thay đổi kích thước và thời gian ghi.
    HANDLE hChangeNotification = FindFirstChangeNotificationA(
        watchPath,
        FALSE, // Không theo dõi thư mục con / Do not watch subtrees
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | 
        FILE_NOTIFY_CHANGE_LAST_WRITE
    );

    if (hChangeNotification == INVALID_HANDLE_VALUE) {
        std::cerr << "FindFirstChangeNotification failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    while (true) {
        // Đợi sự kiện thay đổi xảy ra / Wait for the notification event
        std::cout << "\nWaiting for changes..." << std::endl;
        DWORD dwWaitStatus = WaitForSingleObject(hChangeNotification, INFINITE);

        switch (dwWaitStatus) {
            case WAIT_OBJECT_0:
                // Một thay đổi đã xảy ra trong thư mục
                // A change occurred in the directory
                std::cout << "[!] A change was detected in the directory!" << std::endl;
                
                // Mẹo: Để biết chính xác file nào thay đổi, cần dùng API phức tạp hơn là ReadDirectoryChangesW.
                // Hàm FindFirstChangeNotification chỉ báo có thay đổi chung chung.
                // Tip: ReadDirectoryChangesW is needed to know exactly WHICH file changed.

                // Reset thông báo để tiếp tục theo dõi lần sau
                // Reset notification to continue watching
                if (FindNextChangeNotification(hChangeNotification) == FALSE) {
                    std::cerr << "FindNextChangeNotification failed. Error: " << GetLastError() << std::endl;
                    FindCloseChangeNotification(hChangeNotification);
                    return 1;
                }
                break;

            case WAIT_TIMEOUT:
                // Sẽ không bao giờ xảy ra vì ta dùng INFINITE
                std::cout << "Timeout." << std::endl;
                break;

            default:
                std::cerr << "Unhandled wait status." << std::endl;
                break;
        }
    }

    // Không bao giờ đạt tới đây do vòng lặp vô hạn, nhưng code dọn dẹp tiêu chuẩn
    // Unreachable due to infinite loop, but standard cleanup code
    FindCloseChangeNotification(hChangeNotification);
    return 0;
}
