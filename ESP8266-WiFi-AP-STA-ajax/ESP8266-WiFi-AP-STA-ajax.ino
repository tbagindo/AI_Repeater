/*
 * ESP8266 core version 3.0.2
 * Project ESP8266-01 Wifi Router plus plus
 * we make "RangeExtender" actually WiFi router based on esp8266-01
 * we use ApaITU SmartSwitch board as the base system
 * the board integrated transformerless PSU 100mA @3.3V
 * as well SSR without snubber circuit, this will prevent current leakage by
 * common AC SSR and caused the LED is not turn off complitly.
 * 
 * to drive the SSR (OptoTriac) we use gpio2 on esp8266-01
 * the gpio0 will be connected to header pin optionally to DHT11/22 sensor.
 * 
 * Project target :
 * - WiFi Router without NAT  (WIFI_AP_STA)
 * - custom IP Address  for  softAP network
 * - increase max-connection limit from 4 (default) to 8 client
 * - increase wifi signal quality by adding  external antenna to ESP8266-01 Module
 * - Adding PIR sensor, we could place automatic/manual mode, if automatic mode choosen, we activate the SSR(LED) on present of Human
 * - Single page web based dashboard
 * - SSID,  RSSI, Client connected, file Name, PIR state, auto/manual toggle etc
 * 
 */


#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <LwipDhcpServer.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <DHT.h>
#include "index.h"

#define DEBUG true  //set to true for debug output, false for no debug output
#define DEBUG_SERIAL if(DEBUG)Serial
#define timeSeconds 15

//variable
String page = FPSTR(MAIN_PAGE);
float t;
float h;
int dhtPin = 0;

int ledState=1;
float txPwr =20.5;
uint32_t pMillis;
int32_t rssi;



// Set GPIO for PIR Motion Sensor
const int motionSensor = 14;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
int tCount=0;
bool mFlag=false;


//IPAddress
//station interface
IPAddress staIP(192,168,1,2); 
IPAddress staGW(192,168,1,200); 
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,1,200);
//softAP interface
IPAddress apIP(192,168,2,1);
IPAddress apGW(192,168,2,1);

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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,!ledState);
  pinMode(dhtPin,INPUT);
  delay(3000);
  ledState = !ledState;
  digitalWrite(LED_BUILTIN,!ledState);
  dht.begin();


  //Connection to Main RouterAP
  WiFi.setOutputPower(txPwr); 
  WiFi.mode(WIFI_AP_STA);
  WiFi.config(staIP,staGW,subnet,dns);
  WiFi.begin(stSSID,stPASS);
  /*
  while(WiFi.status() != 3){
    DEBUG_SERIAL.print(".");
    delay(500);
  }
  */
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  DEBUG_SERIAL.println(WiFi.localIP());
  DEBUG_SERIAL.print("connected ... to : ");
  DEBUG_SERIAL.println(WiFi.SSID());

  //Access Point
  dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  WiFi.softAPConfig(apIP,apGW,subnet);
  WiFi.softAP(SSID,PASS);
  DEBUG_SERIAL.println(WiFi.softAPIP());
  
  //webserver  request handler
  server.on("/", indexHTML);
  server.on("/on", ledOn);
  server.on("/off", ledOff);
  server.on("/modeon, modeOn);
  server.on("/modeoff, modeOff);
  server.on("/Temp",getTemp);
  server.on("/Humid",getHumid);
  server.on("/state",getState);
  server.on("/getPTimer, getPTimer);
  server.on("/getRSSI",getRSSI);
  server.on("/getSSID",getSSID);
  server.on("/rssi",handleRSSI);
  server.on("/reboot", [](){DEBUG_SERIAL.println("Reboot...");for(int i=0;i<20; i++){DEBUG_SERIAL.print("*"); delay(10);ESP.restart();}});
  
  //start webserver
  server.begin();
  DEBUG_SERIAL.print("http://");
  DEBUG_SERIAL.print(WiFi.localIP());
  DEBUG_SERIAL.println("/");
  //ledState = !ledState;
  //digitalWrite(LED_BUILTIN,!ledState);
  otaSetup();

  // PIR Motion Sensor mode INPUT
  //pinMode(motionSensor, INPUT);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  //attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

// Checks if motion was detected, sets LED HIGH and starts a timer
ICACHE_RAM_ATTR void detectsMovement() {
  DEBUG_SERIAL.println("MOTION DETECTED!!!");
  //startTimer & reset lastTrigger to current time
  startTimer = true;
  lastTrigger = millis();
  if(mFlag && !ledState){                 // if Mode=Auto && LED OFF >> turn on light
    ledState = 1;                         // ledState 1 = ON, ledState 0 = OFF 
    digitalWrite(LED_BUILTIN,!ledState);  //turn ON light (we have active low system always use !ledState on digitalWrite)
  }
}

void getPTimer(){
  int i = (millis()-lastTrigger)/1000;
  String s = String(i);
  server.send(200,"text/plain",s);
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
      DEBUG_SERIAL.println("Failed to read from DHT sensor!");
      t=1000.0;
  }
  else {
      t = tempT;
      DEBUG_SERIAL.println(t);
  }
  String s =  String(t);
  server.send(200,"text/plain",s);
}

void getHumid(){
  float tempH = dht.readHumidity();
  if (isnan(tempH)) {
      DEBUG_SERIAL.println("Failed to read from DHT sensor!");
      h=1000.0;
  }
  else {
      h= tempH;
      DEBUG_SERIAL.println(h);
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
  DEBUG_SERIAL.println(digitalRead(2));
  server.send(200,"text/html",s);
}

void modeOn(){
  mFlag = true;
  server.send(200,"text/html","Auto");
  DEBUG_SERIAL.println("Mode Auto");
}

void modeOff(){
  mFlag = false;
  server.send(200,"text/html","Manual");
  DEBUG_SERIAL.println("Mode Manual");
}

void ledOn(){
  ledState = 1;
  digitalWrite(2,!ledState);
  server.send(200,"text/html","ON");
  DEBUG_SERIAL.println("LED ON");
}

void ledOff(){
  ledState = 0;
  digitalWrite(2,!ledState);
  server.send(200,"text/html","OFF");
  DEBUG_SERIAL.println("LED OFF");
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
    DEBUG_SERIAL.print(stSSID);
    DEBUG_SERIAL.println(" : " + String(rssi) + "db");
    if(startTimer){
      tCount++;
      DEBUG_SERIAL.println("Timer Seconds Counter : " + String(tCount));
    }
  }

  // Current time
  now = millis();
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    DEBUG_SERIAL.println("Motion stopped...");
    if(mFlag && ledState){                  //if auto Mode && led ON >> turn off light
      ledState = 0;                         //ledState 1=ON,  ledState 0=OFF
      digitalWrite(LED_BUILTIN, !ledState); // we have active low system, use !ledState instead  ledState on digitalWrite() statemenmt
    }
    startTimer = false;
    tCount=0;
  }
}
