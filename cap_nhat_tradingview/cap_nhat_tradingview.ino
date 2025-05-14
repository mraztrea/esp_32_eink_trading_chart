#include <GxEPD2_3C.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Wi-Fi & API
const char *ssid = "thanhhome";
const char *password = "quynhmiu";
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

const int maxCandles = 40;
Candle candles[maxCandles];
int candleCount = 0;
char symbol[10];
char interval[5];
float priceMax = 0;
float priceMin = 999999;
float lastPrice = 0;

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

  // Nếu wake từ nút → thay đổi interval
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
  {
    intervalIndex = (intervalIndex + 1) % intervalCount;
  }

  prefs.putInt("symbolIndex", symbolIndex);
  prefs.putInt("intervalIndex", intervalIndex);
  prefs.end();

  strcpy(symbol, symbols[symbolIndex]);
  strcpy(interval, intervals[intervalIndex]);
  Serial.printf("🖼 Hiển thị chart: %s (%s)\n", symbol, interval);

  // Hiển thị thông báo đang kết nối WiFi
  showLoadingMessage(symbol, interval, "Dang ket noi WiFi...", "", "");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi OK");

  // Hiển thị thông báo đang tải dữ liệu với thông tin WiFi
  String status = "Da ket noi WiFi";
  String ssidInfo = "SSID: " + String(ssid);
  String ipInfo = "IP: " + WiFi.localIP().toString();
  showLoadingMessage(symbol, interval, "Dang tai du lieu moi...", status, ssidInfo, ipInfo);

  fetchChartData();
  drawChart(symbol, interval, lastPrice, priceMin, priceMax, candles, candleCount);

  Serial.println("💤 Vào deep sleep...");
  delay(100);
  esp_deep_sleep_start();
}

void loop() {}

void fetchChartData()
{
  String url = String(baseUrl) + symbol + "&interval=" + interval;
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200)
  {
    String payload = http.getString();
    StaticJsonDocument<8192> doc;
    if (deserializeJson(doc, payload))
      return;

    lastPrice = doc["lp"];
    priceMax = doc["hi"];
    priceMin = doc["lo"];
    updateTime = doc["time"].as<String>(); // Đọc thời gian cập nhật

    JsonArray arr = doc["c"];
    candleCount = min((int)arr.size(), maxCandles);

    for (int i = 0; i < candleCount; i++)
    {
      JsonArray row = arr[i];
      candles[i].open = row[0];
      candles[i].high = row[1];
      candles[i].low = row[2];
      candles[i].close = row[3];
    }
  }

  http.end();
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
    int chartX = 10, chartY = 30, chartW = 320, chartH = 230;
    float priceRange = priceMax - priceMin;
    int candleWidth = 10;

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
