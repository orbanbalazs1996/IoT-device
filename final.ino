#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include <DateTime.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"

//Define wifi parameters
const char* ssid = "Kerekes";
const char* password =  "Bundi123";

//Define senzor parameters
#define DHTTYPE DHT11   // DHT 11
#define DHTPIN 21  //ESP WROOM 32 SDA pin
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BME280 bme; // I2C
Adafruit_BMP280 bmp; // I2C

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);





void setup() {

  wifi_setup();
  time_setup();
  bme_setup();
  bmp_setup();
  dht_setup();

}

void loop() {

  float bme_homerseklet [5];
  float bme_paratartalom [5];
  float dht_paratartalom [5];
  float bmp_homerseklet [5];
  String formattedDate[5];

  for (int i = 0; i < 5; i++) {

    updateTime();
    bme_homerseklet [i] = read_bme_homerseklet();
    bmp_homerseklet [i] = read_bmp_homerseklet();
    bme_paratartalom [i] = read_bme_paratartalom();
    dht_paratartalom [i] = read_dht_paratartalom();
    formattedDate [i] = timeClient.getFormattedDate();;
    delay(60000);
  }


  for (int i = 0; i < 5; i++) {


    addTemperature(bme_homerseklet[i], bmp_homerseklet[i], formattedDate [i]);
    addHumanity(bme_paratartalom[i], dht_paratartalom[i], formattedDate [i]);
  }

}


void addTemperature (float temp1, float temp2, String formattedDate) {
  DynamicJsonDocument doc2(2048);
  doc2["meropont_id"] = "1";
  doc2["date"] = formattedDate;
  doc2["ho_1"] = temp1;
  doc2["ho_2"] = temp2;
  // Serialize JSON document
  String json2;
  serializeJson(doc2, json2);
  HTTPClient http2;
  // Send request
  http2.begin("https://beta.dev.itk.ppke.hu/webprog/~orbba2/homerseklet_beillesztes");
  http2.POST(json2);
  // Read response
  //Serial.print(http2.getString());
  // Disconnect
  http2.end();
}

void addHumanity(float hum1, float hum2, String formattedDate) {
  // Prepare JSON document
  DynamicJsonDocument doc(2048);
  doc["meropont_id"] = "1";
  doc["date"] = formattedDate;
  doc["paratartalom_1"] = hum1;
  doc["paratartalom_2"] = hum2;
  // Serialize JSON document
  String json;
  serializeJson(doc, json);
  HTTPClient http;
  // Send request
  http.begin("https://beta.dev.itk.ppke.hu/webprog/~orbba2/paratartalom_beillesztes");
  http.POST(json);
  // Read response
  //Serial.print(http.getString());
  // Disconnect
  http.end();


}

void updateTime() {

  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  // Serial.println(formattedDate);

}


float read_bme_homerseklet() {
  //return BME Temperature();
  return bme.readTemperature();
}

float read_bme_paratartalom() {
  //return DHT humidity;
  return bme.readHumidity();
}

float read_bmp_homerseklet() {
  //return BMP Temperature();
  return bmp.readTemperature();
}

float read_dht_paratartalom() {
  //return DHT humidity;
  return dht.readHumidity();
}
void bme_setup() {
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1) delay(10);
  }

}
void bmp_setup() {
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }


  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}
void dht_setup() {
  dht.begin();
  //if (!dht.begin()) {
  //  Serial.println(F("Could not find a valid DHT11 sensor, check wiring!"));
  //  while (1);
  //}
}
void wifi_setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}
void time_setup() {
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT + 2 = 7200
  timeClient.setTimeOffset(7200);
}
