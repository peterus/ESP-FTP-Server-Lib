#if defined(ESP32)
#include <SD.h>
#include <SPIFFS.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FS.h>
#endif

#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"

#define WIFI_SSID     "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define FTP_USER     "ftp"
#define FTP_PASSWORD "ftp"

#ifndef UNIT_TEST
FTPServer ftp;

void setup() {
  Serial.begin(115200);
#if defined(ESP32)
  SPIFFS.begin(true);
  SPI.begin(14, 2, 15);
  if (!SD.begin(13)) {
    Serial.println("SD Card Mount Failed");
  }
#elif defined(ESP8266)
  SPIFFS.begin();
#endif

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ftp.addUser(FTP_USER, FTP_PASSWORD);
#if defined(ESP32)
  ftp.addFilesystem("SD", &SD);
#endif
  ftp.addFilesystem("SPIFFS", &SPIFFS);

  ftp.begin();

  Serial.println("...---'''---...---'''---...---'''---...");
}

void loop() {
  ftp.handle();
}
#endif
