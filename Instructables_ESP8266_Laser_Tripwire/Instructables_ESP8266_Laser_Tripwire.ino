// Author: customyota
// License: WTFPL 
// Full liscense text available at http://www.wtfpl.net/about/
// V 0.1

// Disclaimer: Do not shoot lasers into your eyes or the eyes of others.

#include <ESP8266WiFi.h>

////////////////////////////////////
// SSID and PASSWORD
////////////////////////////////////
const char* ssid     = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";

///////////////////////////////////
// AUTOREMOTE URL
///////////////////////////////////
const char* host = "autoremotejoaomgcd.appspot.com";
const char* url = "/sendmessage?key=<YOUR KEY HERE!>&message=LA"; // Don't leave the <>, they should be gone.

///////////////////////////////////
// PIN ASSIGNMENTS
///////////////////////////////////
const int laserPin = D2;   // GPIO4 = board D2
const int sensorPin = D1;  // GPIO5 = board D1
const int laserBrightness = 50; // Range is 0-1024, but really once you go higher than around 200, I couldn't notice a difference

///////////////////////////////////
// Default laser to not ready
///////////////////////////////////
bool laser_ready = false;

///////////////////////////////////
// SETUP
///////////////////////////////////

void setup() {
  pinMode(laserPin, OUTPUT);
  
  //// Start WiFi and wait for connection ////
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {delay(100);}
}

///////////////////////////////////
// THIS FUNCTION IS CALLED WHEN THE BEAM IS BROKEN
///////////////////////////////////
void send_alarm_trigger() {
  
  //// Connect to host (autoremote server) ////
  WiFiClient client;
  const int httpPort = 80;
  client.connect(host, httpPort);

  //// Get URL to send message //// 
  //// Times out after f 5 seconds ////
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return;
    }
  }

  //// Read and store response from the host, can printed for debugging ////
  while (client.available()) {
    String line = client.readStringUntil('\r');
  }
}

///////////////////////////////////
// THIS FUNCTION MAKES SURE THE LASER
// IS AIMED AT THE SENSOR BEFORE ARMING
///////////////////////////////////
bool start_laser() {
  analogWrite(laserPin, laserBrightness);
  delay(50);
  if (!digitalRead(sensorPin)) {
    return true;
  }
  else {
    analogWrite(laserPin, 0);
    return false;
  }
}

///////////////////////////////////
// MAIN LOOP
///////////////////////////////////
void loop() {
  
  if (!laser_ready) {
    laser_ready = start_laser();
    delay(250);
  }

  if ((laser_ready) && (digitalRead(sensorPin))) {
    analogWrite(laserPin, 0);
    laser_ready = false;
    send_alarm_trigger();
    delay(5000);
  }
}
