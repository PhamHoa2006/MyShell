# Role 1: Core Architect Specifications

## 1. Mục tiêu
- **Mục đích:** Xây dựng phần lõi của Small Shell bao gồm vòng lặp REPL, hệ thống phân tích cú pháp (Tokenizer), bộ điều phối lệnh (Command Dispatcher), cùng với 2 tính năng cơ bản là `history` và `help` theo yêu cầu trong `Role1_Core_Specs.md`.
- **Trạng thái:** [x] Hoàn thành

## 2. Đầu vào và Đầu ra (Input & Output)
- **Input:** Mã nguồn hiện tại (`main.cpp` mới chỉ có thông tin khởi động và hàm `split_input`).
- **Output:** Shell có thể hiển thị prompt với đường dẫn hiện tại, không bị crash khi ấn Ctrl+C, nhận lệnh, tách chuỗi chính xác và điều phối gọi đúng các lệnh `help`, `history`, `exit`.

## 3. Danh sách công việc (Checklist)
- [x] Bước 1: Xây dựng vòng lặp REPL trong `main.cpp` (Hiện Prompt đường dẫn, nhận lệnh vô tận, lệnh `exit` để thoát).
- [x] Bước 2: Bắt sự kiện ngắt tín hiệu `Ctrl+C` (SIGINT) để không thoát shell mà chỉ reset dòng nhập.
- [x] Bước 3: Tích hợp hàm `split_input` đã sửa làm Tokenizer chuẩn. (*Chú ý: khi tách xong cần bỏ cả dấu ngoặc kép bọc ngoài chuỗi tham số*).
- [x] Bước 4: Xây dựng **Command Dispatcher** trong `main.cpp` (Nhận token đầu tiên làm lệnh, so sánh và gọi các file tính năng).
- [x] Bước 5: Tạo `Feature/history.h` (Lưu lịch sử các lệnh vào `history.txt` và hiện N lệnh cuối cùng).
- [x] Bước 6: Tạo `Feature/help.h` (In danh sách các lệnh được hỗ trợ).

## 4. Chi tiết kỹ thuật / Thiết kế
- **Các file cần thay đổi:**
  - `main.cpp`: Chỉnh sửa logic `main()`, `execute_command()`.
  - `Feature/history.h` (Tạo mới).
  - `Feature/help.h` (Tạo mới).
- **Cấu trúc dữ liệu dự kiến:**
  - `signal(SIGINT, handler)` cho Ctrl+C.
  - Đọc/Ghi file text cho `history`.

## 5. Ghi chú / Vấn đề gặp phải
- [16/06]: Khởi tạo Task.

## 6. Tham khảo
- [Role1_Core_Specs.md](file:///Users/phamkhanh/Documents/2025.2/Nguyen_ly_he_dieu_hanh/Project/Specs/Role1_Core_Specs.md)
