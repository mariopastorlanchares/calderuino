# CALDERUINO PROJECT

## What is Calderuino?

Calderuino is a project to monitor and manage multiple boilers via a web application and an arduino (preferably NodeMcu)
.

## Installation

Create a **secrets.h** header file for arduino project in **/calderuino/** folder (the folder where calderuino.ino is
placed) to store all your passwords for wifi and firebase connections.

Copy this contents in secrets.h:

 ```
/* 1. Define the Wifi credentials */
#define SECRET_SSID "YOUR_SSID"
#define SECRET_PASS "YOUR_PASSWORD"

/* 2. Define the API Key */
#define API_KEY"AIza*********"

/* 3. Define the RTDB URL */
#define DATABASE_URL "******.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "*********@****.com"
#define USER_PASSWORD "***********"
 ```

### Libraries
HIGH TEMP SOUNDS - https://github.com/adafruit/MAX6675-library

FIREBASE - https://github.com/mobizt/Firebase-ESP-Client
