/*
  Recieving and decoding a reply
*/

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
unsigned long timer;

char website[] PROGMEM = "emoncms.org";
char line_buf[50];                        // Used to store line of http reply header

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

void loop () 
{
  ether.packetLoop(ether.packetReceive());

  if ((millis()-timer)>5000) {
    timer = millis();
    Serial.println("Request sent");
    ether.browseUrl(PSTR("/time/local.json?apikey=256977f2fd55691981af23d9473efa8f"),"", website, my_callback);
  }
}

static void my_callback (byte status, word off, word len) {

  int lsize =   get_reply_data(off);
  
  if(line_buf[0]=='t')
  {
    Serial.print("Time: ");
    Serial.println(line_buf);
    
    char tmp[] = {line_buf[1],line_buf[2]};
    byte hour = atoi(tmp);
    tmp[0] = line_buf[4]; tmp[1] = line_buf[5];
    byte minute = atoi(tmp);
    tmp[0] = line_buf[7]; tmp[1] = line_buf[8];
    byte second = atoi(tmp);
  }
}

