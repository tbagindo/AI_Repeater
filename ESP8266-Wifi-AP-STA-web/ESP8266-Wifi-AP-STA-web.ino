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
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//Station SSID
const char *stSSID = "mktApaITU";
const char *stPASS = "1234567890";

//Access Point SSID
const char *SSID = "ApaITU";
const char *PASS = "1234567890";

//Construct webserver
ESP8266WebServer server;

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

  while(WiFi.status() != 3){
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());

  //Access Point
  WiFi.softAP(SSID,PASS);
  Serial.println(WiFi.softAPIP());

  //WebServer
  server.on("/",handleRoot);
  server.begin();
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}

void handleRoot(){
  String  s ="<html><body><p>WiFi.mode(WIFI_AP_STA) status</p>";
          s +="<p>SSID : " + WiFi.SSID() + "</p>";
          s +="<p>RSSI : " + String(rssi) + "db</p>";
          s +="<p>stat : " + wifiStatus() + "</p>";
          s +="</body></html>"; 
  server.send(200,"text/html",s);         
}

String wifiStatus(){
  String s;
  if(WiFi.status() == 0)s += "WL_IDLE_STATUS";
  if(WiFi.status() == 1)s += "WL_NO_SSID_AVAIL";
  if(WiFi.status() == 2)s += "WL_SCAN_COMPLETED";
  if(WiFi.status() == 3)s += "WL_CONNECTED";
  if(WiFi.status() == 4)s += "WL_CONNECT_FAILED";
  if(WiFi.status() == 5)s += "WL_CONNECTION_LOST";
  if(WiFi.status() == 6)s += "WL_DISCONNECTED";
  return s;
}

void loop(){
  server.handleClient();
  if(millis()-pMillis<1000){
    pMillis=millis();
    rssi = WiFi.RSSI();
    Serial.print(stSSID);
    Serial.println(" : " + String(rssi) + "db");
  }
  
}
