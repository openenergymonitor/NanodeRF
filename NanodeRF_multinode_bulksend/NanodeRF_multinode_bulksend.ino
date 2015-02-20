/*
  NanodeRF_multinode_bulksend

  Queue data recieved from wireless nodes and send all packets to emoncms
  in one connection every 15s, see blog post for more information.
  
  ENTER YOUR APIKEY ON LINE 190, AND SET WIRELESS NODE ID, GROUP AND FREQUENCY

  Authors: Trystan Lea
  Part of the: openenergymonitor.org project
  Licenced under GNU GPL V3
  http://openenergymonitor.org/emon/license

  EtherCard Library by Jean-Claude Wippler and Andrew Lindsay
  JeeLib Library by Jean-Claude Wippler

  THIS SKETCH REQUIRES:
  
  Libraries in the standard arduino libraries folder:
	- JeeLib		https://github.com/jcw/jeelib
	- EtherCard		https://github.com/jcw/ethercard/

  Other files in project directory (should appear in the arduino tabs above)
	- decode_reply.ino
*/

// FORMAT ERROR??
// If you get format error replies, you may be running out of memory 
// try decrease the time between posts so that less packets are queued

#define UNO       //anti crash wachdog reset only works with Uno (optiboot) bootloader, comment out the line if using delianuova
#define RF69_COMPAT  0                                                // Set to 1 if using RFM69CW or 0 is using RFM12B
#include <JeeLib.h>	     //https://github.com/jcw/jeelib
#include <avr/wdt.h>

#define MYNODE 1            
#define RF_freq RF12_868MHZ     // frequency
#define group 1            // network group

//---------------------------------------------------------------------
// The PacketBuffer class is used to generate the json string that is send via ethernet - JeeLabs
//---------------------------------------------------------------------
class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    const char* buffer() { return buf; }
    byte length() { return fill; }
    void reset()
    { 
      memset(buf,NULL,sizeof(buf));
      fill = 0; 
    }
    virtual size_t write (uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    byte fill;
    char buf[450]; // QUEUE BUFFER SIZE, TRY CHANGING THIS IF YOU GET STABILITY PROBLEMS.
    private:
};
PacketBuffer str;

//--------------------------------------------------------------------------
// Ethernet
//--------------------------------------------------------------------------
#include <EtherCard.h>		//https://github.com/jcw/ethercard 

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x42,0x31,0x42,0x21,0x30,0x31 };

// 1) Set this to the domain name of your hosted emoncms - leave blank if posting to IP address 
char website[] PROGMEM = "emoncms.org";

//IP address of remote sever, only needed when posting to a server that has not got a dns domain name (staticIP e.g local server) 
byte Ethernet::buffer[700];
static uint32_t timer;

char line_buf[50];                        // Used to store line of http reply header

unsigned long time10s = 0;
unsigned long start_time = 0;
unsigned long replytimer = 0;
byte ni = 0;
boolean reply_recieved = true;

byte ethernet_requests = 0;

void setup () {
  Serial.begin(9600);
  Serial.println("NanodeRF_multinode_bulksend");

  //if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) {	//for use with Open Kontrol Gateway 
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {	//for use with NanodeRF
    Serial.println( "Failed to access Ethernet controller");
  }
  
  if (!ether.dhcpSetup()) {
    Serial.println("DHCP failed");
  }
  
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  if (!ether.dnsLookup(website)){
    Serial.println("DNS failed");
  }
  ether.printIp("Server: ", ether.hisip);

  delay(100);
  //For use with the modified JeeLib library to enable setting RFM12B SPI CS pin in the sketch. Download from: https://github.com/openenergymonitor/jeelib 
  // rf12_set_cs(9);  //Open Kontrol Gateway	
  // rf12_set_cs(10); //emonTx, emonGLCD, NanodeRF, JeeNode

  rf12_initialize(MYNODE, RF_freq,group);

  #ifdef UNO
  wdt_enable(WDTO_8S); 
  #endif;
  
  ethernet_requests = 0;
  
  str.print("[");
}

void loop () {
  
  #ifdef UNO
  wdt_reset();
  #endif

  //-----------------------------------------------------------------------------------------------------------------
  // 1) On RF recieve
  //-----------------------------------------------------------------------------------------------------------------
  if (rf12_recvDone() && reply_recieved)
  {      
    if (rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
    {    
      if (ni == 0)
      {
        str.reset();
        str.print("[");
        start_time = millis();
      }
      
      ni++; if (ni>1) str.print(","); 
        
      int node_id = (rf12_hdr & 0x1F); byte n = rf12_len;
      int td = (int) ((millis() - start_time) / 1000.0);
      str.print("["); str.print(td); str.print(","); str.print(node_id); str.print(",");
      
      for (byte i=0; i<n; i+=2)
      {
        int num = ((unsigned char)rf12_data[i+1] << 8 | (unsigned char)rf12_data[i]);
        if (i) str.print(",");
        str.print(num);
      }
      str.print("]");
      
    }
  }

  //-----------------------------------------------------------------------------------------------------------------
  // 2) Send data via ethernet
  //-----------------------------------------------------------------------------------------------------------------
  ether.packetLoop(ether.packetReceive());
  
  if ((millis()-replytimer)>2000 && !reply_recieved)
  {
    Serial.println("No reply continue anyway");
    reply_recieved = true; ni = 0;
  }
  
  if ((millis()-time10s)>15000)
  {
    time10s = millis();     // reset lastRF timer

    if (ni>0)
    {
      reply_recieved = false;
        
      str.print("]");
      str.print("\0");  //  End of json string
    
      Serial.print("SENDING: ");
      Serial.print(ni); // print number of packets in queue
      ni = 0;
      
      Serial.print(" ");
      Serial.print(str.buf);
    
      ether.browseUrl(PSTR("/input/bulk.json?apikey=YOURAPIKEY&data=") ,str.buf, website, data_callback);
      ethernet_requests++;
      replytimer = millis();
    }
    else
    {
      Serial.println("NO DATA");
    }
  }
  
  if (ethernet_requests>10) delay(10000);

}

static void data_callback (byte status, word off, word len) 
{
  reply_recieved = true;
  int lsize =   get_reply_data(off);
  Serial.println(line_buf);
  if (strcmp(line_buf,"ok")==0)
  {
    ethernet_requests = 0;
  }
}
