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
#include <LwipDhcpServer.h> 

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

//IPAddress
IPAddress local_IP(192,168,2,1); 
IPAddress gateway(192,168,2,1); 
IPAddress subnet(255,255,255,0); 

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
  // esp8266 core version 3.0.x
  dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  
  // esp8266 core versio. 3.1.x
  // auto& dserver = WiFi.softAPDhcpServer();
  // dserver.setDns(WiFi.dnsIP(0));

  WiFi.softAPConfig(local_IP,gateway,subnet);
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
          s +="</body></html>"; 
  server.send(200,"text/html",s);         
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
