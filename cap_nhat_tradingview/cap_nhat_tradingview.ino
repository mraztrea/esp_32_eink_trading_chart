#include <GxEPD2_3C.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Wi-Fi & API
struct WiFiNetwork {
    const char* ssid;
    const char* password;
};

const WiFiNetwork networks[] = {
    // {"Z117_3", "chiquynhbo"},
    {"thanhhome", "quynhmiu"},
    {"HRI", "hri@1008"}
};
const int networkCount = sizeof(networks) / sizeof(networks[0]);
const char *baseUrl = "https://xehoi.pro/chart.php?symbol=";

// Chart symbols
const char *symbols[] = {"BTCUSDT"};
const char *intervals[] = {"1h", "1d", "1w"};
const int symbolCount = sizeof(symbols) / sizeof(symbols[0]);
const int intervalCount = sizeof(intervals) / sizeof(intervals[0]);

Preferences prefs;
String updateTime = ""; // Thêm biến lưu thời gian cập nhật

struct Candle
{
  float open, high, low, close;
};

#define CS 15
#define DC 27
#define RST 26
#define BUSY 25

GxEPD2_3C<GxEPD2_420_Z98c, GxEPD2_420_Z98c::HEIGHT> display(
    GxEPD2_420_Z98c(CS, DC, RST, BUSY));

const int maxCandles = 50;
Candle candles[maxCandles];
int candleCount = 0;
char symbol[10];
char interval[5];
float priceMax = 0;
float priceMin = 999999;
float lastPrice = 0;
bool fetchFailed = false; // Biến đánh dấu thất bại khi fetchChartData

bool connectToAvailableWiFi() {
  int networksFound = WiFi.scanNetworks();
  Serial.println("Quét tìm mạng WiFi...");
  
  if (networksFound == 0) {
    Serial.println("Không tìm thấy mạng WiFi nào");
    return false;
  }

  // Kiểm tra từng mạng đã quét được
  for (int i = 0; i < networksFound; i++) {
    String scannedSSID = WiFi.SSID(i);
    
    // So sánh với danh sách mạng đã cấu hình
    for (int j = 0; j < networkCount; j++) {
      if (scannedSSID.equals(networks[j].ssid)) {
        Serial.printf("Thử kết nối đến %s\n", networks[j].ssid);
        
        WiFi.begin(networks[j].ssid, networks[j].password);
        
        // Chờ kết nối trong 10 giây
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
          delay(500);
          Serial.print(".");
          attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
          Serial.printf("\nĐã kết nối thành công đến %s\n", networks[j].ssid);
          return true;
        }
        
        Serial.println("\nKết nối thất bại, thử mạng tiếp theo");
        WiFi.disconnect();
      }
    }
  }
  
  Serial.println("Không tìm thấy mạng WiFi khả dụng nào trong danh sách");
  return false;
}

void setup()
{
  Serial.begin(115200);
  delay(300);
  SPI.begin(13, -1, 14, 15);
  display.init();

  // Wakeup by timer or BOOT
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);         // BOOT button
  esp_sleep_enable_timer_wakeup(15 * 60 * 1000000ULL); // 15 phút

  // Đọc index từ bộ nhớ
  prefs.begin("chart", false);
  int symbolIndex = prefs.getInt("symbolIndex", 0);
  int intervalIndex = prefs.getInt("intervalIndex", 0);
  fetchFailed = prefs.getBool("fetchFailed", false); // Đọc trạng thái thất bại từ lần trước

  // Nếu wake từ nút → thay đổi interval nếu lần trước không thất bại
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
  {
    if (!fetchFailed) {
      // Chỉ thay đổi interval nếu lần trước không thất bại
      intervalIndex = (intervalIndex + 1) % intervalCount;
    } else {
      Serial.println("Lần trước fetch thất bại, không thay đổi interval");
    }
  }

  prefs.putInt("symbolIndex", symbolIndex);
  prefs.putInt("intervalIndex", intervalIndex);
  prefs.putBool("fetchFailed", false); // Reset trạng thái thất bại trước khi fetch mới
  prefs.end();

  strcpy(symbol, symbols[symbolIndex]);
  strcpy(interval, intervals[intervalIndex]);
  Serial.printf("🖼 Hiển thị chart: %s (%s)\n", symbol, interval);

  // Hiển thị thông báo đang kết nối WiFi
  showLoadingMessage(symbol, interval, "Dang quet WiFi...", "", "");

  if (!connectToAvailableWiFi()) {
    showLoadingMessage(symbol, interval, "Khong tim thay WiFi", "Vui long kiem tra lai", "");
    delay(5000);
    esp_deep_sleep_start();
    return;
  }

  // Hiển thị thông báo đang tải dữ liệu với thông tin WiFi
  String status = "Da ket noi WiFi";
  String ssidInfo = "SSID: " + WiFi.SSID();
  String ipInfo = "IP: " + WiFi.localIP().toString();
  showLoadingMessage(symbol, interval, "Dang tai du lieu moi...", status, ssidInfo, ipInfo);

  fetchChartData();
  drawChart(symbol, interval, lastPrice, priceMin, priceMax, candles, candleCount);

  Serial.println("💤 Vào deep sleep...");
  delay(100);
  esp_deep_sleep_start();
}

void loop() {}

bool tryFetchChartData(int retryCount) {
  String url = String(baseUrl) + symbol + "&interval=" + interval + "&limit=31";
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  
  Serial.printf("Lần thử %d: HTTP code %d\n", retryCount + 1, httpCode);
  
  if (httpCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<8192> doc;
    if (deserializeJson(doc, payload)) {
      Serial.println("Lỗi parse JSON");
      http.end();
      return false;
    }

    lastPrice = doc["lp"];
    priceMax = doc["hi"];
    priceMin = doc["lo"];
    updateTime = doc["time"].as<String>(); // Đọc thời gian cập nhật

    JsonArray arr = doc["c"];
    candleCount = min((int)arr.size(), maxCandles);

    for (int i = 0; i < candleCount; i++) {
      JsonArray row = arr[i];
      candles[i].open = row[0];
      candles[i].high = row[1];
      candles[i].low = row[2];
      candles[i].close = row[3];
    }
    
    http.end();
    return true;
  }
  
  http.end();
  return false;
}

void fetchChartData()
{
  fetchFailed = true; // Mặc định là thất bại, chỉ đổi khi thành công
  
  // Thử tối đa 3 lần
  const int maxRetries = 3;
  bool success = false;
  
  for (int i = 0; i < maxRetries; i++) {
    success = tryFetchChartData(i);
    if (success) {
      // Nếu thành công, thoát khỏi vòng lặp
      break;
    }
    
    if (i < maxRetries - 1) {
      // Chỉ delay giữa các lần thử, không phải lần cuối
      Serial.printf("Thử lại lần %d sau 1 giây...\n", i + 2);
      delay(1000); // Delay 1 giây trước khi thử lại
    }
  }
  
  // Cập nhật trạng thái và lưu vào bộ nhớ
  fetchFailed = !success;
  prefs.begin("chart", false);
  prefs.putBool("fetchFailed", fetchFailed);
  prefs.end();
  
  if (success) {
    Serial.println("Fetch dữ liệu thành công!");
  } else {
    Serial.println("Fetch dữ liệu thất bại sau 3 lần thử!");
  }
}

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
    if (wifiStatus != "")
    {
      display.setCursor(10, 160);
      display.print(wifiStatus);
    }

    if (ssidInfo != "")
    {
      display.setCursor(10, 190);
      display.print(ssidInfo);
    }

    if (ipInfo != "")
    {
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

void drawChart(const char *symbol, const char *interval, float lastPrice, float priceMin, float priceMax, Candle candles[], int count)
{
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSans9pt7b);

    // Header
    display.setCursor(10, 20);
    display.print(symbol);
    display.setCursor(120, 20);
    display.print(interval);
    display.setCursor(250, 20);
    display.printf("Last: %.1f", lastPrice);

    // Chart area
    int chartX = 30, chartY = 30, chartW = 320, chartH = 230;
    float priceRange = priceMax - priceMin;
    int candleWidth = 6;

    for (int i = 0; i < count; i++)
    {
      int x = chartX + i * (candleWidth + 4);
      int yHigh = chartY + chartH - ((candles[i].high - priceMin) / priceRange * chartH);
      int yLow = chartY + chartH - ((candles[i].low - priceMin) / priceRange * chartH);
      int yOpen = chartY + chartH - ((candles[i].open - priceMin) / priceRange * chartH);
      int yClose = chartY + chartH - ((candles[i].close - priceMin) / priceRange * chartH);

      display.drawLine(x + candleWidth / 2, yHigh, x + candleWidth / 2, yLow, GxEPD_BLACK);
      int yTop = min(yOpen, yClose);
      int yBot = max(yOpen, yClose);
      if (candles[i].close >= candles[i].open)
      {
        display.fillRect(x, yTop, candleWidth, yBot - yTop, GxEPD_WHITE);
        display.drawRect(x, yTop, candleWidth, yBot - yTop, GxEPD_BLACK);
      }
      else
      {
        display.fillRect(x, yTop, candleWidth, yBot - yTop, GxEPD_BLACK);
      }
    }

    // Cột giá bên phải
    int labelX = 360;
    display.setFont(&FreeSans9pt7b);
    for (int i = 0; i <= 5; i++)
    {
      float val = priceMax - i * (priceRange / 5);
      int y = chartY + i * (chartH / 5) + 5;
      display.setCursor(labelX, y);
      display.print((int)val);
    }

    // Đường gióng + ô giá hiện tại
    int yLast = chartY + chartH - ((lastPrice - priceMin) / priceRange * chartH);
    for (int x = chartX; x < chartX + chartW; x += 6)
    {
      display.drawLine(x, yLast, x + 3, yLast, GxEPD_BLACK);
    }
    display.fillRect(labelX - 5, yLast - 10, 45, 20, GxEPD_BLACK);
    display.setCursor(labelX, yLast + 5);
    display.setTextColor(GxEPD_WHITE);
    display.print((int)lastPrice);
    display.setTextColor(GxEPD_BLACK); // khôi phục

    // Hiển thị thời gian cập nhật
    display.setFont(&FreeSans9pt7b);
    display.setCursor(240, 280);
    display.print(updateTime);

  } while (display.nextPage());
}
