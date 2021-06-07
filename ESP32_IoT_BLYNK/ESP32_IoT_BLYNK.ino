#include <DHTesp.h>
#include <Blynk.h>
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e323d38d82534423a1ed7f636033054f";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MINION";
char pass[] = "bismillah";

// Terminal comunication
#define BLYNK_PRINT Serial
WidgetTerminal terminal(V0);
BLYNK_WRITE(V0)
{
  terminal.clear();
  terminal.println("BLYNK Version " BLYNK_VERSION " : SELAMAT DATANG");
  terminal.println("-------------------------");
  if (String("hai") == param.asStr())
  {
    terminal.println("Hallo, saya IoT bisa tolong sebutkan nama anda ?");
  }
  else if (String("tidak") == param.asStr())
  {
    terminal.println("Oke, Terima Kasih");
  }
  else if (String("ya") == param.asStr())
  {
    terminal.println("Berikut yang dapat saya bantu:");
    terminal.println("(tulis dengan sesuai)");
    terminal.println("server");
    terminal.println("wifi");
  }
  else if (String("wifi") == param.asStr())
  {
    terminal.print("Nama Wifi: ");
    terminal.println(ssid);
    terminal.print("Pasword Wifi: ");
    terminal.println(pass);
  }
  else if (String("server") == param.asStr())
  {
    terminal.print("server blynk: ");
    terminal.print(auth);  
  }
  else{
    terminal.print("Hallo ");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.print(", ada yang bisa saya bantu (ya / tidak)");
    terminal.println();
  }
  terminal.flush();
}

BlynkTimer timer;               // Timer BLYNK

DHTesp dht;                     // DHT
#define dhtPin 13               // DHT pin 13
#define dhtType DHT22           // TYPE DHT22

#define redPin 16               // PIN led Red
#define greenPin 4              // PIN led Green
#define bluePin 2               // PIN led Blue

#define lampuPin 12             // PIN lampu AC

int red, green, blue;           // NILAI INTEGER untuk VARIABEL red,green,blue
int mred, mgreen, mblue;

BLYNK_WRITE(V1)                 // BLYNK menulis ke alamat V1
{
  red = param.asInt();          // PARAMETER INTEGER untuk VARIABEL Red
}
BLYNK_WRITE(V2)                 // BLYNK menulis ke alamat V2
{
  green = param.asInt();        // PARAMETER INTEGER untuk VARIABEL Green
}
BLYNK_WRITE(V3)                 // BLYNK menulis ke alamat V3
{
  blue = param.asInt();         // PARAMETER INTEGER untuk VARIABEL Blue
}
int freq = 20000;               // FREQUENSI PWM untuk LED
int redChannel = 0;             // Red CHANNEL urutan ke 0
int greenChannel = 1;           // Green CHANNEL urutan ke 1
int blueChannel = 2;            // Blue CHANNEL urutan ke 2
int resolution = 10;            // RESOLUSI PWM 10-BIT/1024

void setup()
{
  // Debug console
  Serial.begin(9600);                             // KECEPATAN komunikasi SERIAL

  Blynk.begin(auth, ssid, pass);                  // BLYNK MENGHUBUNGKAN

  // Menghubungkan ke Wifi sesuai dengan SSID dan Pasword
  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  ledcSetup(redChannel, freq, resolution);        /////////////////////////////////////////////////////
  ledcSetup(greenChannel, freq, resolution);      // PERSIAPAN LED (URUTAN LED, FREQUENSI, RESOLUSI) //
  ledcSetup(blueChannel, freq, resolution);       /////////////////////////////////////////////////////
  
  ledcAttachPin(redPin, redChannel);              ///////////////////////////////////
  ledcAttachPin(greenPin, greenChannel);          // MELAMPIRKAN PIN ke URUTAN LED //
  ledcAttachPin(bluePin, blueChannel);            ///////////////////////////////////  

  dht.setup(dhtPin, DHTesp::dhtType);             // PERSIAPAN DHT (PIN, TYPE)
  timer.setInterval(1000L, sendSensor);           // SELANG WAKTU untuk timer menjalankan fungsi SENDSENSOR

  pinMode(lampuPin, OUTPUT);                      // PIN lampu sebagai output
}

void sendSensor()               // MEMBUAT fungsi untuk sensor
{  
  float suhu = dht.getTemperature();              // MENGAMBIL DATA Temperature
  float kelembaban = dht.getHumidity();           // MENGAMBIL DATA Humidity

  if(isnan(suhu) || isnan(kelembaban))            // ISNAN (IS Not A Number), jika suhu/kelembaban bukan nomor
  {
    Serial.println("Gagal membaca sensor...!!!"); // DATA GAGAL TERBACA
    return;
  }

  Blynk.virtualWrite(V4, suhu);                   // PIN VIRTUAL 4 DITULIS ke BLYNK
  Blynk.virtualWrite(V5, kelembaban);             // PIN VIRTUAL 5 DITULIS ke BLYNK
}

void tulisLed ()                                  // MEMBUAT FUNGSI untuk menulis nilai LED
{
  mred = map(red,0,100,0,1024);                   // MENSKALA nilai 100 menjadi 1024
  mgreen = map(green,0,100,0,1024);               // MENSKALA nilai 100 menjadi 1024
  mblue = map(blue,0,100,0,1024);                 // MENSKALA nilai 100 menjadi 1024
  
  ledcWrite(redChannel, mred);                     // TULIS LED dari (URUTAN LED, ALAMAT BLYNK)
  ledcWrite(greenChannel, mgreen);                 // TULIS LED dari (URUTAN LED, ALAMAT BLYNK)
  ledcWrite(blueChannel, mblue);                   // TULIS LED dari (URUTAN LED, ALAMAT BLYNK)
}

void loop()
{
  Blynk.run();
  tulisLed();
  timer.run();
}
