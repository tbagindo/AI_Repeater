/*
 * WIFI_AP_STA will route traffic from AP side to Sta side
 * we need to make sure on the main router there is route
 * AP network ie :  192.168.4.0/24
 * route add 192.168.4.0 netmask 255.255.255.0 gw WiFi.localIP()
 * 
 * otherwise the traffic from main router side 
 * will not route to 192.168.4.0  network
 * 
 * there is a flaw in library, by default dhcp server will set the softAPIP 
 * (interface ip) as the dns server. Where there is no dns service running on it host.
 * as workaround  we have two choices :
 * a) run dns server on esp8266
 * b) set the ip correct ip of dns server
 * we prefer the second option
 *
 * for esp8266 core 3.0.x :
 * use LwipDhcpServer.h library
 * #include <LwipDhcpServer.h>
 * in void setup() 
 * set the dns ip address of station side dns as the  entry
 * dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
 * dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
 *
 * for esp8266 core 3.1.x
 * add following line in void setup() 
 * auto& dserver = WiFi.softAPDhcpServer();
 * dserver.setDns(WiFi.dnsIP(0));
 * we could only set single dns entry in this version
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
String iLink = "<p><img src=\"data:image/jpeg;base64,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\" alt=\"\" /></p>";
String vLink = "<p><iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/UKlN-OfHCBQ\" title=\"YouTube video player\" frameborder=\"0\" allow=\"accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share\" allowfullscreen></iframe></p>";

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
  //esp8266 core version 3.1.x
  auto& dserver = WiFi.softAPDhcpServer();
  daerver.setDns(WiFi.dnsIP(0));

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
  String  s ="<html><body><center><h2>WiFi.mode(WIFI_AP_STA) STATUS !!!</h2>";
          s += iLink;
          s +="<table>";
          s +="<tr><td>SSID : </td><td>" + WiFi.SSID() + "</td></tr>";
          s +="<tr><td>RSSI : </td><td>" + String(rssi) + "db</td></tr>";
          s +="<tr><td>File : </td><td>" + fName + "</td></tr>";
          s +="<tr><td>conn : </td><td>" + wifiStatus() + "</td></tr>";
          s +="</table>";
          s += vLink;
          s +="</center></body></html>"; 
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
