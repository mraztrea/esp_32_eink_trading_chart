#include <GxEPD2_3C.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// Wi-Fi & API
const char* ssid = "Huy va Nhi";
const char* password = "okokok";
const char* baseUrl = "https://huykhong.com/IOT/chart.php?symbol=";

// Chart symbols
const char* symbols[] = {"AAPL", "TSLA", "MSFT", "AMZN", "META"};
const int symbolCount = sizeof(symbols) / sizeof(symbols[0]);

Preferences prefs;

struct Candle {
  float open, high, low, close;
};

#define CS   15
#define DC   27
#define RST  26
#define BUSY 25

GxEPD2_3C<GxEPD2_420_Z98c, GxEPD2_420_Z98c::HEIGHT> display(
    GxEPD2_420_Z98c(CS, DC, RST, BUSY)
);

const int maxCandles = 40;
Candle candles[maxCandles];
int candleCount = 0;
char symbol[10];
float priceMax = 0;
float priceMin = 999999;
float lastPrice = 0;

void setup() {
  Serial.begin(115200);
  delay(300);
  SPI.begin(13, -1, 14, 15);
  display.init();

  // Wakeup by timer or BOOT
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // BOOT button
  esp_sleep_enable_timer_wakeup(15 * 60 * 1000000ULL); // 15 phút

  // Đọc index từ bộ nhớ
  prefs.begin("chart", false);
  int index = prefs.getInt("index", 0);

  // Nếu wake từ nút → +1 chart
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    index = (index + 1) % symbolCount;
  }

  prefs.putInt("index", index);
  prefs.end();

  strcpy(symbol, symbols[index]);
  Serial.printf("🖼 Hiển thị chart: %s\n", symbol);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi OK");

  fetchChartData();
  drawChart(symbol, lastPrice, priceMin, priceMax, candles, candleCount);

  Serial.println("💤 Vào deep sleep...");
  delay(100);
  esp_deep_sleep_start();
}

void loop() {}

void fetchChartData() {
  String url = String(baseUrl) + symbol;
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<8192> doc;
    if (deserializeJson(doc, payload)) return;

    lastPrice = doc["lp"];
    priceMax = doc["hi"];
    priceMin = doc["lo"];

    JsonArray arr = doc["c"];
    candleCount = min((int)arr.size(), maxCandles);

    for (int i = 0; i < candleCount; i++) {
      JsonArray row = arr[i];
      candles[i].open  = row[0];
      candles[i].high  = row[1];
      candles[i].low   = row[2];
      candles[i].close = row[3];
    }
  }

  http.end();
}

void drawChart(const char* symbol, float lastPrice, float priceMin, float priceMax, Candle candles[], int count) {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSans12pt7b);

    // Header
    display.setCursor(10, 20);
    display.print(symbol);
    display.setCursor(250, 20);
    display.printf("Last: %.1f", lastPrice);

    // Chart area
    int chartX = 10, chartY = 30, chartW = 320, chartH = 230;
    float priceRange = priceMax - priceMin;
    int candleWidth = 10;

    for (int i = 0; i < count; i++) {
      int x = chartX + i * (candleWidth + 4);
      int yHigh = chartY + chartH - ((candles[i].high - priceMin) / priceRange * chartH);
      int yLow  = chartY + chartH - ((candles[i].low - priceMin) / priceRange * chartH);
      int yOpen = chartY + chartH - ((candles[i].open - priceMin) / priceRange * chartH);
      int yClose= chartY + chartH - ((candles[i].close - priceMin) / priceRange * chartH);

      display.drawLine(x + candleWidth / 2, yHigh, x + candleWidth / 2, yLow, GxEPD_BLACK);
      int yTop = min(yOpen, yClose);
      int yBot = max(yOpen, yClose);
      if (candles[i].close >= candles[i].open) {
        display.fillRect(x, yTop, candleWidth, yBot - yTop, GxEPD_WHITE);
        display.drawRect(x, yTop, candleWidth, yBot - yTop, GxEPD_BLACK);
      } else {
        display.fillRect(x, yTop, candleWidth, yBot - yTop, GxEPD_RED);
      }
    }

    // Cột giá bên phải
    int labelX = 360;
    display.setFont(&FreeSans9pt7b);
    for (int i = 0; i <= 5; i++) {
      float val = priceMax - i * (priceRange / 5);
      int y = chartY + i * (chartH / 5) + 5;
      display.setCursor(labelX, y);
      display.print((int)val);
    }

    // Đường gióng + ô giá hiện tại
    int yLast = chartY + chartH - ((lastPrice - priceMin) / priceRange * chartH);
    for (int x = chartX; x < chartX + chartW; x += 6) {
      display.drawLine(x, yLast, x + 3, yLast, GxEPD_BLACK);
    }
    display.fillRect(labelX - 5, yLast - 10, 45, 20, GxEPD_BLACK);
    display.setCursor(labelX, yLast + 5);
    display.setTextColor(GxEPD_WHITE);
    display.print((int)lastPrice);
    display.setTextColor(GxEPD_BLACK); // khôi phục

  } while (display.nextPage());
}
