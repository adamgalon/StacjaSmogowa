#include "Arduino.h"
#include "PMS.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#include "WiFi.h"
#include "FirebaseESP32.h"

#define FIREBASE_HOST "https://aplikacjaiotinzynier-39c1d.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyALLMnhKy2GEaJySmnA-_dGOZSezZfh7rk"

#define WIFI_SSID "VM3509513"
#define WIFI_PASSWORD "ftMtsngh3phw"

#define RXD2 25
#define TXD2 26
#define SEA_LEVEL_PRESSURE_HPA (1013.25)

double dbPms1;
double dbPms2;
double dbPms10;
float dbTemperature;
float dbHumidity;
float dbPressure;
float dbAapprox;

FirebaseData firebaseData;
String path = "/ESP32_Device";
FirebaseJson json;
void printResult(FirebaseData &data);

HardwareSerial SerialPMS(1);
PMS::DATA data;
PMS pms(SerialPMS);
Adafruit_BME280 bme;

void setup()
{
  Serial.begin(9600);
  SerialPMS.begin(9600, SERIAL_8N1, RXD2, TXD2);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(400);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("jestem tu");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("jestem po 1111");

  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop()
{
  if (pms.read(data) && bme.begin(0x76))
  {
    Serial.println();

    Serial.print("PM 1.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_1_0);
    if (dbPms1 != data.PM_AE_UG_1_0)
    {
      dbPms1 = data.PM_AE_UG_1_0;
    }

    Serial.print("PM 2.5 (ug/m3): ");
    Serial.println(data.PM_AE_UG_2_5);
    if (dbPms2 != data.PM_AE_UG_2_5)
    {
      dbPms2 = data.PM_AE_UG_2_5;
    }

    Serial.print("PM 10.0 (ug/m3): ");
    Serial.println(data.PM_AE_UG_10_0);
    if (dbPms10 != data.PM_SP_UG_10_0)
    {
      dbPms10 = data.PM_AE_UG_10_0;
    }

    Serial.println();
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");
    if (dbTemperature != bme.readTemperature())
    {
      dbTemperature = bme.readTemperature();
    }

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
    if (dbPressure != bme.readPressure())
    {
      dbPressure = bme.readPressure();
    }

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    if (dbHumidity != bme.readHumidity())
    {
      dbHumidity = bme.readHumidity();
    }

    Serial.println();
    Firebase.setDouble(firebaseData, path + "/pmsData1", dbPms1);
    Firebase.setDouble(firebaseData, path + "/pmsData2", dbPms2);
    Firebase.setDouble(firebaseData, path + "/pmsData10", dbPms10);

    Firebase.setFloat(firebaseData, path + "/humidity", dbHumidity);
    Firebase.setFloat(firebaseData, path + "/pressure", dbPressure / 100.0F);
    Firebase.setFloat(firebaseData, path + "/temperature", dbTemperature);
  }
}
