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
const char *SSID = "ApaITU-DEMO";
const char *PASS = "1234567890";

//Construct webserver
ESP8266WebServer server;

//variable
uint32_t pMillis;
int32_t rssi;
String fName = __FILE__;

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println(fName);

  //Connection to Main RouterAP
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(stSSID,stPASS);

  //blocking
  /*
  while(WiFi.status() != 3){
    Serial.print(".");
    delay(500);
  }
  */
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  
  Serial.println(WiFi.localIP());

  //Access Point
  WiFi.softAP(SSID,PASS);
  Serial.println(WiFi.softAPIP());

  //WebServer
  server.on("/",handleRoot);
  server.on("/scan",handleScan);
  server.begin();
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}

void handleRoot(){
  String  s ="<html><body><p>WiFi.mode(WIFI_AP_STA) STATUS!!!</p>";
          s +="<p>SSID : " + WiFi.SSID() + "</p>";
          s +="<p>RSSI : " + String(rssi) + "db</p>";
          s +="<p>File : " + fName + "</p>";
          s +="<p>conn : " + wifiStatus() + "</p>";
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
  
  
void  handleScan(){
  int n = WiFi.scanNetworks();
  String scanResult="<html><body><p>Number of AP's Scanned : " + String(n) + "</p>";
  scanResult +="<table><tr><th>NO</th><th>SSID</th><th>RSSI</th></tr>";
  for(int i=0; i<n; i++){
    scanResult +="<tr>";
    scanResult +="<td>" + String(i) + "</td>";
    scanResult +="<td>" + WiFi.SSID(i) + "</td>";
    scanResult +="<td>" + String(WiFi.RSSI(i)) + " db </td><tr>";
  }
  scanResult +="</table></body></html>";
  server.send(200,"text/html",scanResult);
}

void loop(){
  server.handleClient();
  if(millis()-pMillis>1000){
    pMillis=millis();
    rssi = WiFi.RSSI();
    Serial.print(stSSID);
    Serial.println(" : " + String(rssi) + "db");
    if(rssi == 31)Serial.println("WiFi not Connected");
    if(WiFi.status() != 3)Serial.println("WiFi not Connected   : "  + wifiStatus());
  }
  
}
