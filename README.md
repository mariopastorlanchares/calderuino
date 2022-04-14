# CALDERUINO PROJECT

## What is Calderuino?

Calderuino is a project to monitor and manage multiple boilers via a web application and an arduino (preferably NodeMcu)
.

## Installation

Create a secrets.h header file for arduino project in /calderuino/secrets.h to store all your passwords for wifi and
firebase connections.

Copy this contents:

 ```
#define SECRET_SSID "YOUR_SSID"
#define SECRET_PASS "your_password"
 ```
