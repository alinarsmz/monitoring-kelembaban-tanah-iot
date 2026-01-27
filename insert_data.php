<?php
$host = "localhost";
$user = "alinarus";
$pass = "#alina5x#";
$db   = "alinarus_kelembaban_cabai";

$conn = new mysqli($host, $user, $pass, $db);
if ($conn->connect_error) {
    die("Koneksi gagal: " . $conn->connect_error);
}

$k1       = $_GET['k1'] ?? null;
$k2       = $_GET['k2'] ?? null;
$k3       = $_GET['k3'] ?? null;
$baterai  = $_GET['baterai'] ?? null;
$tegangan = $_GET['tegangan'] ?? null;

$sql = "INSERT INTO data_kelembaban (waktu, k1, k2, k3, baterai, tegangan)
        VALUES (NOW(), '$k1', '$k2', '$k3', '$baterai', '$tegangan')";

echo ($conn->query($sql)) ? "Data tersimpan" : "Error: " . $conn->error;
$conn->close();
?>
