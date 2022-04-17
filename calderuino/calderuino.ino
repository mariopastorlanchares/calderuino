#include <NTPClient.h>
#include <WiFiUdp.h>
// #include <ESP8266WiFi.h>
#include "secrets.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

// Sounds library
#include <max6675.h>

// FIREBASE
#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Define the Firebase Data object
FirebaseData fbdo;
// Define the FirebaseAuth data for authentication data
FirebaseAuth auth;
// Define the FirebaseConfig data for config data
FirebaseConfig config;

// Json
FirebaseJson json;       // or constructor with contents e.g. FirebaseJson json("{\"a\":true}");


// NTP Clock
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// DEVICES

// High temp sounds
// Common pins
int ktcCS = 13;  //D7
int ktcCLK = 14; //D5
// Sound 1
int ktc1SO = 12; //D6
MAX6675 ktc1(ktcCLK, ktcCS, ktc1SO);
// Sound 2
int ktc2S0 = 15; //D8
MAX6675 ktc2(ktcCLK, ktcCS, ktc2S0);

// Ambient sound
int LM35 = A0;
//RELAY 1
int relay1 = 5; //D1
//RELAY 2
int relay2 = 4; //D2

// VARIABLES
float ambient = 0;
// Charcoal heater
float heater_char = 0;
// Diesel Heater
float heater_gas = 0;

int lastMin = -1;



void setup() {
  Serial.begin(9600);
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
  Firebase.RTDB.setMaxRetry(&fbdo, 3);

  /* Assign the required callback functions */
  fbdo.setExternalClientCallbacks(tcpConnectionRequestCallback, networkConnection, networkStatusRequestCallback);

  //Optional, set number of error resumable queues
  // Firebase.RTDB.setMaxErrorQueue(&fbdo, 30);

  //Optional, use classic HTTP GET and POST requests.
  //This option allows get and delete functions (PUT and DELETE HTTP requests) works for
  //device connected behind the Firewall that allows only GET and POST requests.
  // Firebase.RTDB.enableClassicRequest(&fbdo, true);

  // Relays
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
}

void loop() {
  timeClient.update();

  // SOUND
  // Ambient sound
  float ambientReading = analogRead(LM35); //Analog pin reading output voltage by Lm35
  ambient = ambientReading * 0.259; //Finding the true centigrate/celsius temperature
  Serial.print("AMBIENT TEMP = ");
  Serial.println(ambient); //Print centigrade temperature on Serial Monitor

  // High Temp Sounds
  heater_char = ktc1.readCelsius();
  heater_gas = ktc2.readCelsius();


  // Time
  Serial.print(timeClient.getEpochTime());
  Serial.print(" ");
  Serial.println(timeClient.getMinutes());

  // RELAYS
  boolean relay1_val = digitalRead(relay1);
  if (Firebase.RTDB.getInt(&fbdo, "current/relay1_signal")) {
    if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.print( "Relay 1 signal :");
      Serial.println(fbdo.to<int>());
      digitalWrite(relay1, fbdo.to<int>());
    }
  } else {
    Serial.println(fbdo.errorReason());
    if (fbdo.errorReason() == "path not exist") {
      Serial.printf("Set relay signal... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/relay1_signal"), relay1_val) ? "ok" : fbdo.errorReason().c_str());
    }
  }

  boolean relay2_val = digitalRead(relay2);
  if (Firebase.RTDB.getInt(&fbdo, "current/relay2_signal")) {
    if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
      Serial.print( "Relay 2 signal :");
      Serial.println(fbdo.to<int>());
      digitalWrite(relay2, fbdo.to<int>());
    }
  } else {
    Serial.println(fbdo.errorReason());
    if (fbdo.errorReason() == "path not exist") {
      Serial.printf("Set relay signal... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/relay2_signal"), relay2_val) ? "ok" : fbdo.errorReason().c_str());
    }
  }


  // Set values in firebase
  if (Firebase.ready()) {
    Serial.printf("Set time... %s\n", Firebase.RTDB.setInt(&fbdo, F("current/time"), timeClient.getEpochTime()) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set ambient... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/ambient"), ambient) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set charcoal heater... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/charcoal"), heater_char) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set diesel heater... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/diesel"), heater_gas) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set relay 1 current... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/relay1"),  digitalRead(relay1)) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set relay 2 current... %s\n", Firebase.RTDB.setFloat(&fbdo, F("current/relay2"),  digitalRead(relay2)) ? "ok" : fbdo.errorReason().c_str());

    // HISTORY every 10 minutes
    long timestamp = timeClient.getEpochTime();
    int currentMin = timeClient.getMinutes();
    // Chante 10 for the min frequency you want your history to update
    if (currentMin % 10 == 0 && currentMin != lastMin) {
      lastMin = currentMin;
      json.add("time", timestamp);
      json.add("ambient", ambient);
      json.add("charcoal", heater_char);
      json.add("diesel", heater_gas);
      Serial.printf("Update json... %s\n\n", Firebase.RTDB.updateNode(&fbdo, "/history/" + (String) timestamp, &json) ? "ok" : fbdo.errorReason().c_str());
    }
  }

  delay(10000);
}


void networkConnection()
{
  // Reset the network connection
  WiFi.disconnect();

  WiFi.begin( SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
    if (millis() - ms >= 5000)
    {
      Serial.println(" failed!");
      return;
    }
  }
  Serial.println();
  Serial_Printf("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Define the callback function to handle server status acknowledgement
void networkStatusRequestCallback()
{
  // Set the network status
  fbdo.setNetworkStatus(WiFi.status() == WL_CONNECTED);
}

// Define the callback function to handle server connection
void tcpConnectionRequestCallback(const char *host, int port)
{

  // You may need to set the system timestamp to use for
  // auth token expiration checking.

  // Firebase.setSystemTime(WiFi.getTime());

  Serial.print("Connecting to server via external Client... ");
  //if (!client.connect(host, port))
  //{
    //Serial.println("failed.");
    //return;
  //}
  //Serial.println("success.");
}
