// This demo does web requests via DHCP and DNS lookup.
// 2011-07-05 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>
#define RF69_COMPAT  0                                                // Set to 1 if using RFM69CW or 0 is using RFM12B
#include <JeeLib.h>

#define REQUEST_RATE 5000 // milliseconds

const int greenLED=5;
const int redLED=6;

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// remote website name
char website[] PROGMEM = "google.com";

byte Ethernet::buffer[700];
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  pinMode(greenLED,OUTPUT);
  pinMode(redLED,OUTPUT);
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  
   rf12_initialize(10, RF12_868MHZ, 210);

   for (int i=0; i<10; i++)                                              //Send RFM12B test sequence (for factory testing)
   {
     int payload=i; 
     rf12_sendNow(0, &payload, sizeof payload);
     delay(100);
   }

digitalWrite(redLED, HIGH);

  Serial.begin(9600);
  Serial.println("\n[getDHCPandDNS]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
  {
    Serial.println( "Failed to access Ethernet controller");
    digitalWrite(redLED, LOW); 
  }

  if (!ether.dhcpSetup())
  {
    Serial.println("DHCP failed");
    digitalWrite(redLED, LOW);
  }
  
  ether.printIp("My IP: ", ether.myip);
  // ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  digitalWrite(greenLED, LOW);

  if (!ether.dnsLookup(website))
  {
    Serial.println("DNS failed");
    digitalWrite(redLED, LOW);
  }
  

  ether.printIp("Server: ", ether.hisip);
  
  timer = - REQUEST_RATE; // start timing out right away

 

}

void loop () {
    
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
    ether.browseUrl(PSTR("/foo/"), "bar", website, my_result_cb);
  }
}
