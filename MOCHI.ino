#include "DHT.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_BMP085.h>
#define DHTTYPE DHT11

//Inisiasi Pin Sensor
uint8_t DHTpin = D3;
int pin_rainSensor = A0;

const char* ssid = "Marek";
const char* password = "masokpak";
const char* host = "malikmal98.my.id";
//String url = "/public/insert"; 
int adcvalue=0;

Adafruit_BMP085 bmp;
DHT dht(DHTpin, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);
  pinMode(DHTpin, INPUT);
  dht.begin();
  if (!bmp.begin()) 
  {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1) {
      }
  }
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default, would try to act as both a client and an access-point and could cause network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("connecting to ");
  Serial.print(host); // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, 80 /*httpPort*/)) {
    Serial.println("connection failed");
    return;
  }

 float nilai = analogRead(pin_rainSensor);
 nilai = nilai /1024 * 100;
 nilai = 100 - nilai;
  
  String url = "/public/insert/"
+ String(9) + "/" // ID Device. yang sudah terdaftar ada ID 4
+ String(dht.readTemperature()) + "/" // Data sensor Suhu
+ String(dht.readHumidity()) + "/" // Data sensor Kelembapan
+ String(bmp.readPressure()*0.001) + "/" // Data sensor Tekanan Udara dalam satuan mbar
+ String((int)nilai) ; // Random Data sensor hujan antara 0 & 1

  Serial.print(" \nRequesting URL: ");
  Serial.println(url); // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println(">>> Client Timeout !");
      client.stop(); 
      return;
    }
  }
  delay(1000);
}
