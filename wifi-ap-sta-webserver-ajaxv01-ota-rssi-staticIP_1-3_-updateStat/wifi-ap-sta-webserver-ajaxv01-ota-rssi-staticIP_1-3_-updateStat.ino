#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <LwipDhcpServer.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <DHT.h>
#include "index.h"

//variable
String page = FPSTR(MAIN_PAGE);
float t;
float h;
int dhtPin = 0;

int ledState=1;
float txPwr =20.5;
uint32_t pMillis;
int32_t rssi;

#define timeSeconds 15

// Set GPIO for PIR Motion Sensor
const int motionSensor = 14;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int tCount=0;

// Checks if motion was detected, sets LED HIGH and starts a timer
ICACHE_RAM_ATTR void detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  if(!ledState){
    digitalWrite(2, LOW);
    startTimer = true;
    lastTrigger = millis();
  }
}

//IPAddress
IPAddress local_IP(192,168,2,1); 
IPAddress gateway(192,168,2,1); 
IPAddress subnet(255,255,255,0);

//Station SSID
const char *stSSID = "mktApaITU";
const char *stPASS = "1234567890";

//Access Point SSID
const char *SSID = "ApaITU";
const char *PASS = "1234567890";

//constructor
ESP8266WebServer server(80);
DHT dht(dhtPin, DHT22);


void setup() {                                                                                                                                                                                                                                                             
  Serial.begin(115200);
  Serial.println();
  Serial.print("Sketch File Name : ");
  Serial.println(__FILE__);
  pinMode(2, OUTPUT);
  digitalWrite(2,!ledState);
  pinMode(dhtPin,INPUT);
  delay(3000);
  ledState = !ledState;
  digitalWrite(LED_BUILTIN,!ledState);
  dht.begin();


  //Connection to Main RouterAP
  WiFi.setOutputPower(txPwr); 
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(stSSID,stPASS);

  while(WiFi.status() != 3){
    Serial.print(".");
    delay(500);
  }
  Serial.println(WiFi.localIP());
  Serial.print("connected ... to : ");
  Serial.println(WiFi.SSID());

  //Access Point
  dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  WiFi.softAPConfig(local_IP,gateway,subnet);
  WiFi.softAP(SSID,PASS);
  Serial.println(WiFi.softAPIP());
  
  //webserver  request handler
  server.on("/", indexHTML);
  server.on("/on", ledOn);
  server.on("/off", ledOff);
  server.on("/Temp",getTemp);
  server.on("/Humid",getHumid);
  server.on("/state",getState);
  server.on("/getRSSI",getRSSI);
  server.on("/getSSID",getSSID);
  server.on("/rssi",handleRSSI);
  server.on("/reboot", [](){Serial.println("Reboot...");for(int i=0;i<20; i++){Serial.print("*"); delay(10);ESP.restart();}});
  
  //start webserver
  server.begin();
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  //ledState = !ledState;
  //digitalWrite(LED_BUILTIN,!ledState);
  otaSetup();

  // PIR Motion Sensor mode INPUT
  //pinMode(motionSensor, INPUT);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  //attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

void getRSSI(){
  String s="";
  if(WiFi.status() == WL_CONNECTED){
    s += String(WiFi.RSSI());
  }else{
    s +="WiFi Not Connected";
  }
  server.send(200,"text/plain",s);

}

void getSSID(){
  String s="";
  if(WiFi.status() == WL_CONNECTED){
    s += WiFi.SSID();
  }else{
    s +="WiFi Not Connected";
  }
  server.send(200,"text/plain",s);

}

void getState(){
  String s = String(!digitalRead(2));
  server.send(200,"text/plain",s);
}

void getTemp(){
  float tempT = dht.readTemperature();
  if (isnan(tempT)) {
      Serial.println("Failed to read from DHT sensor!");
      t=1000.0;
  }
  else {
      t = tempT;
      Serial.println(t);
  }
  String s =  String(t);
  server.send(200,"text/plain",s);
}

void getHumid(){
  float tempH = dht.readHumidity();
  if (isnan(tempH)) {
      Serial.println("Failed to read from DHT sensor!");
      h=1000.0;
  }
  else {
      h= tempH;
      Serial.println(h);
  }
  String s =  String(h);
  server.send(200,"text/plain",s);
}

void indexHTML(){
  String s;
   s+=page;
  if(digitalRead(2)){
    s.replace("@@LEDSTATE@@","");
  }else{
    s.replace("@@LEDSTATE@@","checked");
  }
  Serial.println(digitalRead(2));
  server.send(200,"text/html",s);
}

void ledOn(){
  ledState = 1;
  digitalWrite(2,!ledState);
  server.send(200,"text/html","ON");
  Serial.println("LED ON");
}

void ledOff(){
  ledState = 0;
  digitalWrite(2,!ledState);
  server.send(200,"text/html","OFF");
  Serial.println("LED OFF");
}

void handleRSSI(){
  String  s ="<html><body><p>WiFi.mode(WIFI_AP_STA) status</p>";
          s +="<p>SSID : " + WiFi.SSID() + "</p>";
          s +="<p>RSSI : " + String(rssi) + "db</p>";
          s +="</body></html>"; 
  server.send(200,"text/html",s);         
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  
  if(millis()-pMillis>4000){
    pMillis=millis();
    rssi = WiFi.RSSI();
    Serial.print(stSSID);
    Serial.println(" : " + String(rssi) + "db");
    if(startTimer){
      tCount++;
      Serial.println("Timer Seconds Counter : " + String(tCount));
    }
  }

  // Current time
  now = millis();
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    digitalWrite(2, HIGH);
    startTimer = false;
    tCount=0;
  }
}
