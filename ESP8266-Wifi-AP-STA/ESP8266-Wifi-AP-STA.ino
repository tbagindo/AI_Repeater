/*
 * WIFI_AP_STA will route traffic from AP side to Sta side
 * we need to make sure on the main router there is route
 * AP network ie :  192.168.4.0/24
 * route add 192.168.4.0 netmask 255.255.255.0 gw WiFi.localIP()
 * 
 * otherwise the traffic from main router side 
 * will not route to 192.168.4.0  network
 */

#include <ESP8266WiFi.h>

//Station SSID & Password
const char *stSSID = "mktApaITU";
const char *stPASS = "1234567890";

//Access Point SSID & Password
const char *SSID = "ApaITU-Demo";
const char *PASS = "1234567890";


//variable
uint32_t pMillis;
int32_t rssi;

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);

  //Connection to Main RouterAP
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(stSSID,stPASS);

  /* Blocking 
  while(WiFi.stastus() != WL_CONNECTED){
    Serial.println(".");
    delay(500);
  }
  */
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.println(WiFi.localIP());

  //Access Point
  WiFi.softAP(SSID,PASS);
  Serial.println(WiFi.softAPIP());
  
}

void loop(){
  if(millis()-pMillis>1000){
    pMillis=millis();
    rssi = WiFi.RSSI();
    Serial.print(stSSID);
    Serial.println(" : " + String(rssi) + "db");
  }
  
}
