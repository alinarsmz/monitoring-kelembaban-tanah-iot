<?php
$conn = new mysqli("localhost", "alinarus", "#alina5x#", "alinarus_kelembaban_cabai");
if ($conn->connect_error) die("Koneksi gagal");

$sql = "
SELECT waktu, k1, k2, k3, baterai, tegangan
FROM (
    SELECT * FROM data_kelembaban
    ORDER BY id DESC
    LIMIT 15
) AS sub
ORDER BY id ASC
";

$result = $conn->query($sql);
$data = [];

while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

header("Content-Type: application/json");
echo json_encode($data);
$conn->close();
?>
