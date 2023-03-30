# AI_Repeater
ESP8266WiFi WIFI_AP_STA mode
youtube video

[![IMAGE ALT TEXT](http://img.youtube.com/vi/jaqputbJiU4/0.jpg)](http://www.youtube.com/watch?v=jaqputbJiU4 "Video Title")
ESP8266 WiFi Router without NAT


WIFI_AP_STA will route traffic from AP side to Sta side
we need to make sure on the main router there is route
AP network ie :  192.168.4.0/24
route add 192.168.4.0 netmask 255.255.255.0 gw WiFi.localIP()
 
otherwise the traffic from main router side 
will not route to 192.168.4.0  network

there is a flaw in library, by default dhcp server will set the softAPIP 
(interface ip) as the dns server. Where there is no dns service running on it host.
as workaround  we have two choices :
a) run dns server on esp8266
b) set the ip correct ip of dns server
we prefer the second option

for esp8266 core 3.0.x :
use LwipDhcpServer.h library
#include <LwipDhcpServer.h>
in void setup() 
set the dns ip address of station side dns as the  entry
dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));

for esp8266 core 3.1.x
add following line in void setup() 
auto& dserver = WiFi.softAPDhcpServer();
dserver.setDns(WiFi.dnsIP(0));
we could only set single dns entry in this version
