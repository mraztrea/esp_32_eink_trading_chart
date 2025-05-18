<?php
date_default_timezone_set('Asia/Ho_Chi_Minh');

header('Content-Type: application/json');

$symbol = isset($_GET['symbol']) ? $_GET['symbol'] : 'BTCUSDT';
$interval = isset($_GET['interval']) ? $_GET['interval'] : '1d';
$limit = isset($_GET['limit']) ? intval($_GET['limit']) : 31;

// Binance API endpoint cho dữ liệu candlestick
$url = "https://api.binance.com/api/v3/klines?symbol=" . urlencode($symbol) . "&interval=$interval&limit=$limit";

$response = @file_get_contents($url);
if ($response === false) {
    echo json_encode(array("error" => "Failed to fetch data from Binance API"));
    exit;
}

$data = json_decode($response, true);
if (json_last_error() !== JSON_ERROR_NONE) {
    echo json_encode(array("error" => "Failed to parse JSON response"));
    exit;
}

// Kiểm tra lỗi
if ($data === null || isset($data['code'])) {
    echo json_encode(array("error" => "API error", "msg" => isset($data['msg']) ? $data['msg'] : 'unknown'));
    exit;
}

$prices = array();
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

    $prices[] = array($o, $h, $l, $c);
}

echo json_encode(array(
    "s" => $symbol,
    "lp" => $lastClose,
    "hi" => $hi,
    "lo" => $lo,
    "c" => $prices,
    "time" => date("Y-m-d H:i")
), JSON_PRETTY_PRINT);
