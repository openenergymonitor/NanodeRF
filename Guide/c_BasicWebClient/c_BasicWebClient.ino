/*
  A basic web client demo sending test data to emoncms
  
  Features: DCHP and DNS Lookup
*/

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
unsigned long timer;

char website[] PROGMEM = "emoncms.org";

void setup () 
{
  Serial.begin(9600);
  Serial.println("Example 03");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if ((millis()-timer)>5000) {
    timer = millis();
    Serial.println("Request sent");
    ether.browseUrl(PSTR("/api/post.json?apikey=256977f2fd55691981af23d9473efa8f&json="), "{power:252.4,temperature:15.4}", website, 0);
  }
}
