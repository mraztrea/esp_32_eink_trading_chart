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
