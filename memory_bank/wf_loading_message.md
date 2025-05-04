# Workflow: Hiển thị thông báo đang tải dữ liệu

## Mô tả

File workflow này ghi lại các thay đổi thực hiện để hiển thị thông báo đang tải dữ liệu và thông tin kết nối WiFi trên màn hình E-Ink mỗi khi hàm `fetchChartData()` được gọi.

## Các thay đổi chính

1. Nâng cấp hàm `showLoadingMessage()` để hiển thị thông tin WiFi

   ```c
   void showLoadingMessage(const char *symbol, const char *interval, const String &message,
                         const String &wifiStatus, const String &ssidInfo, const String &ipInfo)
   {
     display.setFullWindow();
     display.firstPage();
     do
     {
       display.fillScreen(GxEPD_WHITE);
       display.setTextColor(GxEPD_BLACK);
       display.setFont(&FreeSans9pt7b);

       // Tiêu đề
       display.setCursor(10, 20);
       display.print(symbol);
       display.setCursor(120, 20);
       display.print(interval);

       // Thông báo đang tải
       display.setCursor(10, 100);
       display.print(message);
       display.setCursor(10, 130);
       display.print("Vui long doi trong giay lat...");

       // Thông tin WiFi
       if (wifiStatus != "") {
         display.setCursor(10, 160);
         display.print(wifiStatus);
       }

       if (ssidInfo != "") {
         display.setCursor(10, 190);
         display.print(ssidInfo);
       }

       if (ipInfo != "") {
         display.setCursor(10, 220);
         display.print(ipInfo);
       }

     } while (display.nextPage());
   }

   // Overload để tương thích với code cũ
   void showLoadingMessage(const char *symbol, const char *interval, const String &message,
                         const String &wifiStatus, const String &ssidInfo)
   {
     showLoadingMessage(symbol, interval, message, wifiStatus, ssidInfo, "");
   }
   ```

2. Thêm thông báo đang kết nối WiFi trước khi kết nối

   ```c
   // Hiển thị thông báo đang kết nối WiFi
   showLoadingMessage(symbol, interval, "Dang ket noi WiFi...", "", "");

   WiFi.begin(ssid, password);
   ```

3. Hiển thị thông tin WiFi sau khi kết nối thành công

   ```c
   // Hiển thị thông báo đang tải dữ liệu với thông tin WiFi
   String status = "Da ket noi WiFi";
   String ssidInfo = "SSID: " + String(ssid);
   String ipInfo = "IP: " + WiFi.localIP().toString();
   showLoadingMessage(symbol, interval, "Dang tai du lieu moi...", status, ssidInfo, ipInfo);
   ```

## Kết quả

Sau khi thực hiện các thay đổi này, màn hình E-Ink sẽ hiển thị hai màn hình thông báo:

1. Màn hình đầu tiên hiển thị "Dang ket noi WiFi..." khi thiết bị đang kết nối WiFi
2. Màn hình thứ hai hiển thị:
   - "Dang tai du lieu moi..." khi bắt đầu tải dữ liệu
   - Trạng thái WiFi: "Da ket noi WiFi"
   - Tên SSID: "SSID: [tên_wifi]"
   - Địa chỉ IP: "IP: [địa_chỉ_ip]"

Các thông tin này giúp người dùng biết được trạng thái kết nối mạng và quá trình tải dữ liệu của thiết bị.

## Lưu ý

- Đã sử dụng chữ không dấu để tránh lỗi hiển thị với font của màn hình E-Ink
- Dùng overload function để tương thích với code cũ nếu cần thiết
