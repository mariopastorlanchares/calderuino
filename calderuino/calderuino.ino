// #include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// #include <ESP8266WiFi.h>
#include "secrets.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

//Define the Firebase Data object
FirebaseData fbdo;

//Define the FirebaseAuth data for authentication data
FirebaseAuth auth;

// Define the FirebaseConfig data for config data
FirebaseConfig config;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  Serial.begin(9600);
  Serial.print("Hola ");
  // WiFi
  WiFi.begin( SECRET_SSID, SECRET_PASS);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  // Time client
  // timeClient.begin();

  // Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  //Initialize the library with the Firebase authen and config.
  Firebase.begin(&config, &auth);

  //Optional, set AP reconnection in setup()
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  //Optional, set number of error retry
  // Firebase.RTDB.setMaxRetry(&fbdo, 3);

  //Optional, set number of error resumable queues
  // Firebase.RTDB.setMaxErrorQueue(&fbdo, 30);

  //Optional, use classic HTTP GET and POST requests.
  //This option allows get and delete functions (PUT and DELETE HTTP requests) works for
  //device connected behind the Firewall that allows only GET and POST requests.
  // Firebase.RTDB.enableClassicRequest(&fbdo, true);
}

void loop() {
  timeClient.update();
  Serial.print(timeClient.getEpochTime());
  Serial.print(" ");
  Serial.println(timeClient.getMinutes());
  Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("time"), timeClient.getEpochTime()) ? "ok" : fbdo.errorReason().c_str());
  // Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("/test/int"), 1) ? "ok" : fbdo.errorReason().c_str());
  delay(10000);
}
