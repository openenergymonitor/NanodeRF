/*
  This example fetches an emoncms feed value from a remote server and turns the 
  LED on if the feed is 1 or more and off if the feed is 0.
  
  This provides a mechamism for remotely switching a digital output on and off.
    
  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
  
*/

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
unsigned long timer;

char website[] PROGMEM = "emoncms.org";

// This is the char array that holds the reply data
char line_buf[50];

const int redLED = 6;                     // NanodeRF RED indicator LED
const int greenLED = 5;                   // NanodeRF GREEN indicator LED

void setup () 
{
  Serial.begin(9600);
  Serial.println("07 - Fetch Emoncms Feed Value");

  pinMode(redLED, OUTPUT); digitalWrite(redLED,HIGH);            
  pinMode(greenLED, OUTPUT); digitalWrite(greenLED,HIGH); 

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

void loop () 
{
  ether.packetLoop(ether.packetReceive());

  if ((millis()-timer)>5000) {
    timer = millis();
    Serial.println("Request sent");
    // Make our request - here we ask for the emoncms feed value
    // Emoncms.org account example username: switch, password: switch
    ether.browseUrl(PSTR("/feed/value.json?apikey=729a12fc9ff2a35de6f7bfad3e751088&id=2898"),"", website, my_callback);
  }
}

static void my_callback (byte status, word off, word len) {

  // get_reply_data gets the data part of the reply and puts it in the line_buf char array
  int lsize =   get_reply_data(off);
  
  Serial.print("Feed value: ");

  int value = atoi(line_buf);
  Serial.println(value);
  
  if (value>0)
  {
    digitalWrite(redLED,LOW);            
    digitalWrite(greenLED,LOW); 
  }
  else
  {
    digitalWrite(redLED,HIGH);            
    digitalWrite(greenLED,HIGH);  
  }
  
}

