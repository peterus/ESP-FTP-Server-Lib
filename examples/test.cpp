#include <SPIFFS.h>
#include <SD.h>
#include <WiFi.h>

#include "ESP-FTP-Server-Lib.h"

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define FTP_USER "ftp"
#define FTP_PASSWORD "ftp"

FTPServer ftp;

void setup()
{
	Serial.begin(115200);
	SPIFFS.begin(true);
	SPI.begin(14, 2, 15);
	if(!SD.begin(13))
	{
		Serial.println("SD Card Mount Failed");
	}


	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(WIFI_SSID);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	ftp.addUser(FTP_USER, FTP_PASSWORD);
	//ftp.setFilesystem(&SPIFFS);
	ftp.setFilesystem(&SD);

	ftp.begin();

	Serial.println("...---'''---...---'''---...---'''---...");
}

void loop()
{
	ftp.handle();
}
