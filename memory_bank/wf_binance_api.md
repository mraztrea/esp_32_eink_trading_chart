# Workflow: Chuyển đổi từ Twelvedata API sang Binance API

## Mô tả

Điều chỉnh file chart.php để sử dụng API của Binance thay vì Twelvedata API để lấy dữ liệu giá cryptocurrency.

## Các thay đổi chính

1. **Thay đổi tham số mặc định:**

   - Thay đổi tham số mặc định từ 'XAU/USD' sang 'BTCUSDT'
   - Thêm tham số interval và limit với giá trị mặc định '5m' và 20

2. **Thay đổi API endpoint:**

   - Từ: `https://api.twelvedata.com/time_series?symbol=...&interval=5min&outputsize=20&apikey=$apiKey&format=JSON`
   - Sang: `https://api.binance.com/api/v3/klines?symbol=...&interval=$interval&limit=$limit`

3. **Điều chỉnh xử lý lỗi:**

   - Kiểm tra lỗi dựa trên cấu trúc trả về của Binance API

4. **Điều chỉnh cấu trúc dữ liệu:**
   - Dữ liệu trả về từ Binance có cấu trúc dạng mảng thay vì JSON object
   - Mỗi phần tử trong mảng có cấu trúc:
     - [0]: Open time
     - [1]: Open price
     - [2]: High price
     - [3]: Low price
     - [4]: Close price
     - [5]: Volume
     - ... etc

## Code trước khi thay đổi

```php
<?php
header('Content-Type: application/json');

$symbol = isset($_GET['symbol']) ? $_GET['symbol'] : 'XAU/USD';
$apiKey = 'api_key_tu_dang_ky_di_nhe';

$url = "https://api.twelvedata.com/time_series?symbol=" . urlencode($symbol) . "&interval=5min&outputsize=20&apikey=$apiKey&format=JSON";

$response = file_get_contents($url);
$data = json_decode($response, true);

// Check if error
if (!isset($data['values'])) {
    echo json_encode(["error" => "API error", "msg" => $data['message'] ?? 'unknown']);
    exit;
}

$prices = [];
$hi = 0;
$lo = 999999;
$lastClose = 0;

$values = array_reverse($data['values']); // oldest first

foreach ($values as $row) {
    $o = floatval($row['open']);
    $h = floatval($row['high']);
    $l = floatval($row['low']);
    $c = floatval($row['close']);

    $hi = max($hi, $h);
    $lo = min($lo, $l);
    $lastClose = $c;

    $prices[] = [$o, $h, $l, $c];
}

echo json_encode([
    "s" => $symbol,
    "lp" => $lastClose,
    "hi" => $hi,
    "lo" => $lo,
    "c" => $prices
], JSON_PRETTY_PRINT);
```

## Code sau khi thay đổi

```php
<?php
header('Content-Type: application/json');

$symbol = isset($_GET['symbol']) ? $_GET['symbol'] : 'BTCUSDT';
$interval = isset($_GET['interval']) ? $_GET['interval'] : '5m';
$limit = isset($_GET['limit']) ? intval($_GET['limit']) : 20;

// Binance API endpoint cho dữ liệu candlestick
$url = "https://api.binance.com/api/v3/klines?symbol=" . urlencode($symbol) . "&interval=$interval&limit=$limit";

$response = file_get_contents($url);
$data = json_decode($response, true);

// Kiểm tra lỗi
if ($data === null || isset($data['code'])) {
    echo json_encode(["error" => "API error", "msg" => $data['msg'] ?? 'unknown']);
    exit;
}

$prices = [];
$hi = 0;
$lo = 999999;
$lastClose = 0;

// Dữ liệu Binance đã sắp xếp theo thời gian (cũ đến mới), nên không cần đảo ngược
foreach ($data as $candle) {
    // Cấu trúc dữ liệu Binance klines:
    // [0] Open time, [1] Open, [2] High, [3] Low, [4] Close, [5] Volume, etc.
    $o = floatval($candle[1]);
    $h = floatval($candle[2]);
    $l = floatval($candle[3]);
    $c = floatval($candle[4]);

    $hi = max($hi, $h);
    $lo = min($lo, $l);
    $lastClose = $c;

    $prices[] = [$o, $h, $l, $c];
}

echo json_encode([
    "s" => $symbol,
    "lp" => $lastClose,
    "hi" => $hi,
    "lo" => $lo,
    "c" => $prices
], JSON_PRETTY_PRINT);
```
