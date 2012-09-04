/*
  This last example details how to receive and decode a reply from the server.
  
  It can be used to fetch the time from the server or to get any data from the server.
  It could form the basis for an internet actived lighting or even a heating system.
  Where the basestation polls the server periodically asking if the user has requested
  a light to be on etc.
  
  As in the BasicWebClient example we send a request using:
  ether.browseUrl(PSTR("/time/local.json?apikey=YOURAPIKEY"),"", website, my_callback);
  
  But notice the my_callback bit - this is the name of the function to be called when a reply is received
  
  In the my_callback function we get the data part of the reply and put it in the line_buf char array buffer.
  We then go through the line_buf char array and decode it as needed. (see comments below)
  
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

void setup () 
{
  Serial.begin(9600);
  Serial.println("06 - Receiving a reply");

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
    // Make our request - here we ask for the user local time
    ether.browseUrl(PSTR("/time/local.json?apikey=YOURAPIKEY"),"", website, my_callback);
  }
}

static void my_callback (byte status, word off, word len) {

  // get_reply_data gets the data part of the reply and puts it in the line_buf char array
  int lsize =   get_reply_data(off);
  
  // The format of the time reply is: t12,43,12 (t for time, 12h 43mins 12seconds)
  // char 0 is the character t (by asking if it is indeed t we can indentify the reply)
  if(line_buf[0]=='t')
  {
    Serial.print("Time: ");
    Serial.println(line_buf);
    
    // The next two characters form the hour number.
    // to convert these two char entries into an actual number we need to use
    // the character array to integer function
    
    // 1) We isolate the hour characters and put them in a temporary buffer
    char tmp[] = {line_buf[1],line_buf[2]};
    // 2) We convert to an integer (or actually byte in this case as an integer would be an overkill
    // for a number that can not be larger than 24.
    byte hour = atoi(tmp);
    
    // We then repeat this same procedure for minutes and seconds.
    tmp[0] = line_buf[4]; tmp[1] = line_buf[5];
    byte minute = atoi(tmp);
    tmp[0] = line_buf[7]; tmp[1] = line_buf[8];
    byte second = atoi(tmp);
  }
}

