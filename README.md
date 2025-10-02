# DSA Cinema Project

## Cách build và chạy chương trình

### Trên Windows (Git Bash, MSYS2, MinGW)
Mở terminal trong thư mục dự án, sau đó chạy:

```bash
Build: g++ src/main.cpp src/UIManager.cpp src/MovieManager.cpp src/BookingManager.cpp src/utils.cpp src/IndexManager.cpp  -o main.exe

Note: Stack.h is header-only, no separate compilation needed.

Run: ./main.exe

## Tính năng mới: Undo (Hoàn tác)
- Sau khi đặt hoặc hủy ghế, có thể hoàn tác thao tác cuối cùng
- Chọn "3. Hoan tac thao tac cuoi" trong menu chính (chỉ hiện khi có lịch sử)
