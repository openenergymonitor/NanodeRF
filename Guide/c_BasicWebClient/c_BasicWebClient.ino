/*
  This is a basic web client demo sending test data to emoncms
  It sends a couple of example variables in a semi-json like format: {power:252.4,temperature:15.4}
  
  Try creating an account on emoncms.org then get the write api key and enter in line 51 replacing
  the text YOURAPIKEY.
  
  This example features both DCHP and DNS Lookup.
  
  DHCP is where we ask the router for an ip address.
  
  DNS is where we ask a Domain name server for the ip address of the server we want to send data to:
  the domain name emoncms.org is linked to the ip address 213.138.101.177
  Using DNS Lookup we can save having to remember these hard to remember strings of numbers. 
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
  Serial.println("03 - Basic Web Client");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
    
  // DHCP Setup
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  // DNS Setup
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if ((millis()-timer)>5000) {
    timer = millis();
    Serial.println("Request sent");
    
    // Send some test data to the server:
    ether.browseUrl(PSTR("/api/post.json?apikey=YOURAPIKEY&json="), "{power:252.4,temperature:15.4}", website, 0);
  }
}
