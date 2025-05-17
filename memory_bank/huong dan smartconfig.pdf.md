<!-- 5/17/25, 12:10 PM Lâp trinh ESP32 Smartconfig thiêt lâp wifi qua app- Khuê Nguyên Creator -->

LÂP TRÌNH ESP32

# Lập trình ESP32 Smartconfig thiết lập wifi qua app

ÐÃ ÐANG TRÊN 05/11/2021 BOI NGUYENKHUE2608

<!-- K -->

**Bài 7 WIFI:Lâp trình ESP32Smartconfig**

Trong bài này chúng ta sẽ học cách sử dụng ESP32 Smartconfig để thiết lâp wifi từ xa cho ESP32 qua app. Đây là một phân rất quan trọng trong các thiết bị IOT sử dụng wifi. Cùng tìm hiểu nhé

Bài 7 Networking trong serie Hoc ESP32 tù A tói Z

# Table of Contents

1. Tai sao chúng ta cân su dung smartconfig dể thiết lâp wifi

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 1/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiét lâp wifi qua app- Khuê Nguyên Creator -->

2. Smartconfig hoat dông nhu thế nào?

2.1.Smartconfig là gì?

2.2.Smartconfig hoat dông nhu thế nào?

2.3. Smartconfig SDK cho thiết bi và app

3. Lâp trình Wifi Smartconfig trên ESP32

3.1.Full code

3.2.Ket qua

4.Két

# Tai sao chúng ta cân sử dụng smartconfig để thiết lâp wifi

Trong các bài huóng dan truóc chúng ta thuòng lâp trình WiFi.begin(ssid,password) để cài đǎt tên và password cho mach.

Một khá bất tiên đó là, mỗi khi ban sử dung wifi khác, ban phải sửa lai wifi trên code vànap lai chương trình. Ðiêu này thực sự là không thể khi làm các sản phẩm bán cho khách hàng.

Vì vậy người ta phải tiến hành 1 bước trước khi thiết bị hoat đông đó là thiết lâp wifi cho san phẩm dó, có 2 cách dể làm diêu này:

·Dùng AP mode: Nhu bài ESP32 AP Mode, chung ta có thê phát ra wifi sau dó dùng web browser truy cập vào ip của ESP32 rôi gửi wifi và mật khẩu lên. Thế nhung khábất tiện khi người dùng phải thao tác nhiêu bước, không thể tự dông hóa khi dùng app.

·Dùng Smartconfig: Cách này rất tiên dung, có các gói SDK dể các ban có thê truyên wifi và pass từ app xuống ESP32 mà không cân phai thao tác gì khác.

Trong bài này, chúng ta sẽ học cách sử dụng smartconfig trên ESP32.

# Smartconfig hoat dông như thế nào?

# Smartconfig là gì?

Smartconfig là một kĩ thuật cấu hình wifi của thiết bị mà không cân nap lai code, sử dung kết nối giữa wifi của thiết bị với điện thoai để truyên thông tin ssid và pass. Kĩ thuât này được Tl tao ra và đang được áp dụng cho hâu hết tất cả các dòng chip có sử dụng wifi.Vói ESP32, ESP8266 và các dòng khác cua Espressif họ cũng sử dung kĩ thuât này vàdóng gói thành ESP Touch.

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 2/12 -->

<!-- 5/17/25, 12:10 PM Lâp trinh ESP32 Smartconfig thiêt lâp wifi qua app-Khuê Nguyên Creator -->

<!-- ExpTouch SmartConfig for ES. SSD:TechTOnions 855I0 02:00 00:0000:00 Password Device count1 ® Ow ESP TOUCH No Access Point Required SSID And Password APP SSID And Password CONFRM 出县法课委规公新乐3法非子 江BEBGu 、 -->

Vây uu diêm cua SmartConfig là gì?

·Dễ dàng cau hình wifi cho ESP8266,ESP32 thông qua smartphone.

·Không cân phai nap lai code dể câu hình

·Có thể dùng Smartconfig để câu hình nhiêu thiết bị một lúc

# Smartconfig hoat động như thế nào?

Có 2 đặc diểm của ESP để có thể sử dung đuợc kĩ thuât này.

·ESP có kha nǎng lǎng nghe tât ca các gói tin không dây WiFi xung quanh nó, bao gôm cả các gói tindã duoc mã hóa.

**·** Các gói tin gửi trong mang WiFi được mã hóa và không thể đọc duợc nội dung, tuy nhiên đô dài gói tin là một hǎng so.

# Tiến trình Smartconfig nhu sau:

·ESP ở chế dô lang nghe gói tin UDP

·Smartphone app gui các gói tin UDP duoc mã hóa

·ESP nhân ban tin, kiểm tra độ dài của mỗi dữ liệu từ đó suy ra từng kí tự, ghép nôi chúng lai sẽ tạo ra 1 chuỗi SSID và PASS

·ESP chuyên sang STA mode đǎng nhập vào mạng wifi với thông tin nhu trên

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 3/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiét lâp wifi qua app - Khuê Nguyen Creator -->

·App nhân được thông tin ESP đǎng nhập vào, luu địa chỉ IP của thiết bi dó lai dể giao tiếp

Các tiến trình này là tự động khi các bạn sử dung gói SDK của ESP. Ðể tìm hiểu kĩ hon các ban vào link bên duới nhé

Link tham khao:http://depletionregion.blogspot.com/2013/10/cc3000-smart-config-transmitting-ssid.html

# Smartconfig SDK cho thiết bi và app

Espressif cung câp gói SDK trong link sau:

https://www.espressif.com/en/products/software/esp-touch/overview

Ở đây các ban có thể download các gói thư viên cho Thiết bị và App dùng IOS hoǎc Android

| Title                    | Platform   | Version | Release Date | Download |
| ------------------------ | ---------- | ------- | ------------ | -------- |
| ESP8266 NONOS SDK V1.5.3 | NON-OS SDK | V1.5.3  | 2016.04.18   |          |

| Title                 | Platform | Version | Release Date | Download |
| --------------------- | -------- | ------- | ------------ | -------- |
| ESP-TOUCH for Android | Android  | V2.0.0  | 2021.02.08   |          |
| ESP-TOUCH for iOs     | IOS      | V2.0.0  | 2020.04.20   |          |

Trên Arduino giao thức này được viết thành 1 hàm dó là smartconfig rôi nên các ban không cân download gì ca.

Trong bài này, chúng ta ko viết app nên cũng sẽ dùng các app có sǎn trên CH Play và App Store:

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 4/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiét lâp wifi qua app- Khuê Nguyen Creator -->

- Trên Android ứng dụng tên là **"IOT_Espressif"** hoǎc **“ESP8266** **SmartConfig"** (vân có thểdùng cho ESP32):

https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch

| EspTouch: SmartConfig for<br>ESP8266,ESP32<br>KhoaZero123 Education ★\*572<br>Everyone<br>This app is available for your device<br>Add to Wishlist Install<br>08:59 보 0440 08.50로 40<br>EspTouch:SmartConfig for ESP826.. EspTouch:SmanCanfig for ESP826<br>SSID FOC Techition<br>SSID:FGC Techlution<br>BSSID:10663110:553e<br>BSSID:10:6f:3f:10:65:3e<br>Password: f gct2016 <br>Password: <br>Device count:1<br>Device count:1 Boaccalt Modae<br>Broadcest Muticast<br>Esetouch is noefgurng plesce wat<br>for amomet<br>THOAT<br><img src="https://web-api.textin.com/ocr_image/external/98987bff9a5a6bec.jpg"> |
| --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 5/12 -->

<!-- 5/17/25,12:10 PM Lâp trình ESP32 Smartconfig thiét lâp wifi qua app - Khuê Nguyên Creator -->

https://play.google.com/store/apps/details?id=com.espressif.iot

- Trên IOS App tên là **"Espressif Esptouch":**

https://itunes.apple.com/us/app/espressif-esptouch/id1071176700?mt=8

# Lâp trình Wifi Smartconfig trên ESP32

Một số hàm cân luu ý khi su dung Smartconfig

·WiFi.mode(WIFI_AP_STA): chuyên ESP32 sang Station mode (Ðê chay SmartConfig cân chuyên ESP32 sang Station mode)

·WiFi.beginSmartConfig(): start SmartConfig

·WiFi.smartConfigDone(): kiêm tra smartconfig xong hay chua

# Full code

1 #include &lt;Arduino.h&gt;

2 #include "WiFi.h"

3 **void** setup() {

4 **Serial.begin(115200);**

5 **WiFi.mode(WIFI_AP_STA);**

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 6/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiêt lâp wifi qua app - Khuê Nguyên Creator -->

6 /_ start SmartConfig _/

7 WiFi.beginSmartConfig();

8

9 /_ Wait for SmartConfig packet from mobile _/

10 Serial.println("Waiting for SmartConfig.");

11 while (!WiFi.smartConfigDone()){

12 delay(500);

13 Serial.print(".");

14 }

15 Serial.println("");

16 Serial.println("SmartConfig done.");

17

18 /_ Wait for WiFi to connect to AP _/

19 Serial.println("Waiting for WiFi");

20 while (WiFi.status() != WL_CONNECTED) {

21 delay(500);

22 Serial.print(".");

23 }

24 Serial.println("WiFi Connected.");

25 Serial.print("IP Address: ");

26 Serial.println(WiFi.localIP());

27 }

28 void loop() {

29

30 }

# Kết quá

Sau khi nap chuong trình, ESP32 sẽ vào chế đô smart config. Trên app, nhâp thông tin mang và ân Confirm. ESP32sẽ nhân được Wifi và kết nối vào.

--- Available filters and text transformations: colorize, debug, default, direct,esp32_exception **time**

**---More details at http://bit.ly/pio-monitor-filters**

**---Miniterm on COM7 115200,8,N,1---**

**---Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H---**

**Waiting for SmartConfig.**

**SmartConfig done.**

**Waiting for WiFi**

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 7/12 -->

<!-- 5/17/25,12:10 PM Lâp trinh ESP32 Smartconfig thiêt lâp wifi qua app - Khuê Nguyên Creator -->

<!-- ☐ 11:21 PM ESP8266 SmartConfig SSID:Tuan Passord 12345678 Esptouch success,bssid= S 30aea400ab24,InetAddress= 172.20.10.5 CONFIRM CONFIRM -->

![](https://web-api.textin.com/ocr_image/external/9d159d38300dcae0.jpg)

# Kêt

ESP32 Smartconfig sử dụng cũng tương đối đơn giản, công viêc của các ban là thêm phân này vào app điêu khiển cho ứng dụng cua các ban.

Cám on ban dã dón đọc, cùng vào hội Anh Em Nghiên Lập Trình để cùng trao dôi nhé

5/5 - (2 bình chon)

![](https://web-api.textin.com/ocr_image/external/56ea52af3e389552.jpg)

<!-- f -->

![](https://web-api.textin.com/ocr_image/external/4033d2f7f3adb4cc.jpg)

![](https://web-api.textin.com/ocr_image/external/b83e35cab59c5611.jpg)

![](https://web-api.textin.com/ocr_image/external/00ae2cddd9393303.jpg)

![](https://web-api.textin.com/ocr_image/external/ca56970b7dc98889.jpg)

<!-- in -->

![](https://web-api.textin.com/ocr_image/external/8f8f2b0429cd954f.jpg)

**NGUYENKHUE2608**

![](https://web-api.textin.com/ocr_image/external/e049c0fa3b688502.jpg)

**Ðể lai một bình luân**

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 8/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiết lập wifi qua app - Khuê Nguyen Creator -->

Email của ban sẽ không được hiển thị công khai. Các trường bắt buộc được đánh dau \*

Bình luân \*

| <img src="https://web-api.textin.com/ocr_image/external/0f40ec88d062c2d3.jpg"> |
| ------------------------------------------------------------------------------ |

Tên \*

![](https://web-api.textin.com/ocr_image/external/c6879db670385909.jpg)

Email \*

|     |
| --- |

Trang web

☐ Luu tên của tôi, email, và trang web trong trình duyệt này cho lân bình luân kế tiếp của tôi.

| GUI BÌNH LUÂN |
| ------------- |

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 9/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiêt lâp wifi qua app - Khuê Nguyễn Creator -->

| Khuê Nguyên Creator-Ho...<br>4.447 nguòi theo doi<br><img src="https://web-api.textin.com/ocr_image/external/3f12f923f772c871.jpg"> |
| ----------------------------------------------------------------------------------------------------------------------------------- |
| Theo döi Trang Chia sè                                                                                                              |
|                                                                                                                                     |

# LATEST POSTS

**21** Xuât file Gerber Alltium và dǎt hàng gia công PCB tai JLCPCB Th4

<u>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</u>

**15** Cơ ban vê GitFlow, tai sao nó được sử dung rông rãi trong phát triên phân mêm 2 Nhân xét

Th10

14 Học Git cơ ban cho nguòi mói bǎt dâu

Th10

[STM32- RTOS] Các trang thái cua Task và Task API

| 24  |
| --- |

28 [STM32 - RTOS] Bài 1: Tao Task và Scheduleing task su dung STM32 HAL Th7

# TAG CLOUD

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 10/12 -->

<!-- 5/17/25, 12:10 PM Lâp trinh ESP32 Smartconfig thiét lâp wifi qua app - Khuê Nguyên Creator -->

8051 Altium biến Blynk bài tâp C Cai Dat Cube Cai dat Keil c CubeMX Công tac WIFI dac eSp32 esp32http esp32 webserver firebase GPIO Hercules TerminalHoc lap trinh huong dan phan mem Huóng dan Học Altium Học lâp trình STM32 học stm32 IOT iot platform lâp trìnhLâp **trình C lâp trình**

STM32 ngôn ngữ **C** ngǎt nuvoton phím tät altium rtos**STM32**stm32 lcd1602 STM32

Timer stm32 usb stm32 usb hid thiết kế pcb thư viện altium tông hop vi diêu khiên vs code Vẽ mach in diên tửdiên tu co ban

# DANH MUC

| Chia Sé                 | (18) |
| ----------------------- | ---- |
| Chua duoc phân loai (1) |      |
| Ðiên tu và Phân cúng    | (25) |
| Hê diêu hành            | (2)✓ |
| Hoc lâp trình MCU       | (89) |
| Lâp trìnhIOT            | (6)✓ |
| Ngôn Ngu Lâp trình      | (59) |

# QUANG CÁO

# KHUÊ NGUYÊN CREATOR

K **Chia sé dam mê**

![](https://web-api.textin.com/ocr_image/external/57a264659992af5c.jpg)

Blog này làm ra để luu trữ tât ca những kiến thức, những câu chuyên của mình. Ðôi khi là những

ý tưởng nhất thời, đôi khi là các dự án tự mình làm. Chia sẻ cho người khác cũng là niêm vui cua

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 11/12 -->

<!-- 5/17/25, 12:10 PM Lâp trình ESP32 Smartconfig thiét lâp wifi qua app- Khuê Nguyen Creator -->

mình, kiến thức mỗi người là khác nhau, không hằn quá cao siêu nhung sẽ có lúc hữu dụng.

**DMCAPROTECTED**

# Liên Kêt

Nhóm: Nghiên Lâp Trình

Fanpage:Khuê Nguyên Creator

My Shop

# Thông Tin

Tác Gia

Chính Sách Bao Mât

![](https://web-api.textin.com/ocr_image/external/6de63614dc45dded.jpg)

![](https://web-api.textin.com/ocr_image/external/53ee1ad60ded43b5.jpg)

![](https://web-api.textin.com/ocr_image/external/ac56279622ade140.jpg)

<!-- in -->

![](https://web-api.textin.com/ocr_image/external/e3e4827ade33757f.jpg)

**f**

Copyright 2025©Khuê Nguyen

<!-- https://khuenguyencreator.com/lap-trinh-esp32-smartconfig-thiet-lap-wifi/ 12/12 -->
