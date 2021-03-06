//#include <U8g2lib.h>
//#include <U8x8lib.h>

#include <ESP8266WiFi.h>
#include "wifi.h"
#include "temppref.h"

// IO
const int green = 0;
const int red = 8;
const int yellow = 6;
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 5, /* data=*/ 4);

// Server, file, port
const char hostname[] = "query.yahooapis.com";
const String url = "https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22Scherpenzeel%2C%20nl%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
const int port = 80;

// Timeout
unsigned long timeout = 10000;  // ms

// WiFi Client
WiFiClient client;

void setup() {
  
  pinMode(red,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(green,OUTPUT);
  // Init Serial
  Serial.begin(9600);
  delay(100);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Show we are connected
  Serial.println("Connected!");
}

void loop() {

  unsigned long timestamp;
  int temp;
  int tempCelsius;
  // Establish TCP connection
  Serial.print("Connecting to ");
  Serial.println(hostname);
  if ( !client.connect(hostname, port) ) {
    Serial.println("Connection failed");
  }

  // Send GET request
  String req = "GET " + url + " HTTP/1.1\r\n" + 
                "Host: " + hostname + "\r\n" +
                "Connection: close\r\n" +
                "\r\n";
  client.print(req);

  // Wait for response from server
  delay(500);
  timestamp = millis();
  while ( !client.available() && (millis() < timestamp + timeout) ) {
    delay(1);
  }

  // Parse temperature
  if ( client.find("temp\":") ) {
    temp = client.parseInt();
    tempCelsius = (temp - 32) * 5/9;
    Serial.print("Verwachte Temperatuur: ");
    Serial.print(tempCelsius);
    Serial.println(" C");
    if (tempCelsius < badWeather) {
      digitalWrite(green,LOW);
      digitalWrite(yellow,LOW);
      digitalWrite(red,HIGH);
    } else if (tempCelsius < regularWeather) {
      digitalWrite(green,LOW);
      digitalWrite(yellow,HIGH);
      digitalWrite(red,LOW);
    } else if (tempCelsius > awesomeWeather) {
      digitalWrite(green,HIGH);
      digitalWrite(yellow,LOW);
      digitalWrite(red,LOW);
    } 
  }

  // Flush receive buffer
  while ( client.available() ) {
    client.readStringUntil('\r');
  }

  // Close TCP connection
  client.stop();
  Serial.println();
  Serial.println("Connection closed");

  delay(30000);
}
