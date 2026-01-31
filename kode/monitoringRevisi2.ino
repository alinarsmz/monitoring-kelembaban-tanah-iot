#include <WiFi.h>
#include <HTTPClient.h>

/* =================== WIFI =================== */
const char* ssid     = "o";
const char* password = "satuduatiga";

/* =================== SERVER =================== */
String SERVER_URL = "https://alinarusmiza.ftiunwaha.my.id/kelembaban/insert_data.php";

/* =================== PIN =================== */
#define PIN_TEGANGAN 33

/* =================== RESISTOR =================== */
float R1 = 30000.0;
float R2 = 7500.0;

/* =================== VAR GLOBAL =================== */
int bateraiTampil = -1;   // smoothing battery %

/* =================== BATAS KELEMBABAN (%) =================== */
#define BATAS_KERING 30
#define BATAS_BASAH  70

/* =================== SETUP =================== */
void setup() {
  Serial.begin(115200);
  pinMode(PIN_TEGANGAN, INPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terhubung!");
}

/* =================== BACA TEGANGAN =================== */
float readVoltage() {
  const int samples = 20;
  long total = 0;

  for (int i = 0; i < samples; i++) {
    total += analogRead(PIN_TEGANGAN);
    delay(2);
  }

  float adc = total / (float)samples;
  float Vsensor = (adc * 4.2) / 4095.0;
  float Vbat = Vsensor / (R2 / (R1 + R2));

  return Vbat - 0.2; // kalibrasi
}

/* =================== TEGANGAN → PERSEN =================== */
int voltageToPercent(float v) {
  if (v <= 3.0) return 0;
  if (v >= 4.2) return 100;
  return (int)((v - 3.0) / (4.2 - 3.0) * 100);
}

/* =================== STATUS TANAH =================== */
String statusTanah(int persen) {
  if (persen <= BATAS_KERING) return "KERING";
  if (persen >= BATAS_BASAH)  return "BASAH";
  return "NORMAL";
}

/* =================== PESAN PERINGATAN =================== */
String pesanTanah(String status) {
  if (status == "KERING") return "Siramilah air!";
  if (status == "BASAH")  return "Kurangi air!";
  return "Normal";
}

/* =================== LOOP =================== */
void loop() {

  /* -------- SIMULASI SENSOR KELEMBABAN -------- */
  int k1 = random(0, 100);
  int k2 = random(0, 100);
  int k3 = random(0, 100);

  String status1 = statusTanah(k1);
  String status2 = statusTanah(k2);
  String status3 = statusTanah(k3);

  /* -------- BATERAI -------- */
  float tegangan = readVoltage();
  int bateraiBaru = voltageToPercent(tegangan);

  if (bateraiTampil < 0) {
    bateraiTampil = bateraiBaru;
  } else if (abs(bateraiBaru - bateraiTampil) >= 3) {
    bateraiTampil = bateraiBaru;
  }

  /* -------- LOG SERIAL -------- */
  Serial.println("===== DATA SENSOR =====");

  Serial.printf("K1: %d %% | %s | %s\n",
                k1, status1.c_str(), pesanTanah(status1).c_str());

  Serial.printf("K2: %d %% | %s | %s\n",
                k2, status2.c_str(), pesanTanah(status2).c_str());

  Serial.printf("K3: %d %% | %s | %s\n",
                k3, status3.c_str(), pesanTanah(status3).c_str());

  Serial.printf("Tegangan: %.2f V\n", tegangan);
  Serial.printf("Baterai : %d %%\n", bateraiTampil);

  /* -------- KIRIM KE SERVER -------- */
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = SERVER_URL;
    url += "?k1=" + String(k1);
    url += "&k2=" + String(k2);
    url += "&k3=" + String(k3);
    url += "&baterai=" + String(bateraiTampil);
    url += "&tegangan=" + String(tegangan, 2);
    url += "&s1=" + status1;
    url += "&s2=" + status2;
    url += "&s3=" + status3;

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.print("Server Response: ");
      Serial.println(http.getString());
    } else {
      Serial.println("Gagal kirim data");
    }

    http.end();
  }

  Serial.println("========================\n");
  delay(15000);
}
