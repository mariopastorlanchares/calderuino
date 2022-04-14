#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

// Create a "secrets.h" header file in the project root and store the passwords there
const char *ssid     = SECRET_SSID;
const char *password = SECRET_PASS;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(timeClient.getEpochTime());
  Serial.print(" ");
  Serial.print(timeClient.getMinutes());
  Serial.println("");
  //Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
