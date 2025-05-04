# Workflow: Chuyển đổi interval khi nhấn nút BOOT

## Mô tả

File workflow này ghi lại các thay đổi được thực hiện để điều chỉnh hành vi khi nhấn nút BOOT trên ESP32. Thay vì thay đổi symbol (ví dụ: BTCUSDT), thiết bị sẽ thay đổi interval (1h, 1d, 1w) của biểu đồ.

## Các thay đổi chính

1. Thêm biến `intervalCount` để đếm số lượng interval có sẵn

   ```c
   const int intervalCount = sizeof(intervals) / sizeof(intervals[0]);
   ```

2. Thêm biến `interval` để lưu interval hiện tại

   ```c
   char interval[5];
   ```

3. Đổi phương thức lưu trữ trong Preferences

   ```c
   // Cũ
   int index = prefs.getInt("index", 0);
   // Mới
   int symbolIndex = prefs.getInt("symbolIndex", 0);
   int intervalIndex = prefs.getInt("intervalIndex", 0);
   ```

4. Thay đổi logic khi wake từ nút BOOT

   ```c
   // Cũ
   if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
   {
     index = (index + 1) % symbolCount;
   }
   // Mới
   if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
   {
     intervalIndex = (intervalIndex + 1) % intervalCount;
   }
   ```

5. Lưu cả hai chỉ số vào Preferences

   ```c
   prefs.putInt("symbolIndex", symbolIndex);
   prefs.putInt("intervalIndex", intervalIndex);
   ```

6. Cập nhật URL API để bao gồm thông số interval

   ```c
   String url = String(baseUrl) + symbol + "&interval=" + interval;
   ```

7. Hiển thị interval trên màn hình E-Ink
   ```c
   display.setCursor(100, 20);
   display.print(interval);
   ```

## Kết quả

Sau khi thực hiện các thay đổi này, mỗi khi thiết bị wake từ nút BOOT, nó sẽ hiển thị biểu đồ với interval tiếp theo trong danh sách (luân phiên giữa 1h, 1d, 1w) và giữ nguyên symbol.
