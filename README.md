<b># AI_Repeater ESP8266WiFi WIFI_AP_STA mode</b>

[![IMAGE ALT TEXT](http://img.youtube.com/vi/jaqputbJiU4/0.jpg)](http://www.youtube.com/watch?v=jaqputbJiU4 "Video Title")
ESP8266 WiFi Router without NAT


WIFI_AP_STA will route traffic from AP side to Sta side. we need to make sure on the main router there is a static route to AP network ie :  192.168.4.0/24

<b>route add 192.168.4.0 netmask 255.255.255.0 gw WiFi.localIP()</b>
 
otherwise the traffic from main router side will not be routed to 192.168.4.0  network

There is a flaw in library, by default dhcp server will set the softAPIP (interface ip) as the dns server. Where there is no dns service running on it host.

<b>as workaround  we have two choices :
<p>a) run dns server on esp8266</p>
<p>b) set the ip correct ip of dns server</b></p>


<p>we prefer the second option</p>


<b>for esp8266 core 3.0.x :</b>

use LwipDhcpServer.h library
<p>#include <LwipDhcpServer.h></p>

<b>in void setup()</b>
<p>set the dns ip address of station side dns as the  entry</p>

dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));


<b>for esp8266 core 3.1.x
<p>in void setup()</p></b>
 
auto& dserver = WiFi.softAPDhcpServer();
dserver.setDns(WiFi.dnsIP(0));

we could only set single dns entry in this version
