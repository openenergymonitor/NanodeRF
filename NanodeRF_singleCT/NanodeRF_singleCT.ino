/*                          _                                                      _      
                           | |                                                    | |     
  ___ _ __ ___   ___  _ __ | |__   __ _ ___  ___       _ __   __ _ _ __   ___   __| | ___ 
 / _ \ '_ ` _ \ / _ \| '_ \| '_ \ / _` / __|/ _ \     | '_ \ / _` | '_ \ / _ \ / _` |/ _ \
|  __/ | | | | | (_) | | | | |_) | (_| \__ \  __/  _  | | | | (_| | | | | (_) | (_| |  __/
 \___|_| |_| |_|\___/|_| |_|_.__/ \__,_|___/\___| (_) |_| |_|\__,_|_| |_|\___/ \__,_|\___|
                                                                                          
*/
//--------------------------------------------------------------------------------------
// Relay's data recieved by emontx up to emoncms
// Minimal CT and supply voltage only version

// Authors: Trystan Lea and Glyn Hudson
// Part of the: openenergymonitor.org
// Licenced under GNU GPL V3

// EtherCard Library by Jean-Claude Wippler and Andrew Lindsay
// JeeLib Library by Jean-Claude Wippler

// #define DEBUG 

#include <JeeLib.h>

#define MYNODE 35            // node ID 30 reserved for base station
#define freq RF12_433MHZ     // frequency
#define group 210            // network group 

// The RF12 data payload - a neat way of packaging data when sending via RF - JeeLabs
typedef struct
{
  int ct1;		     // current transformer 1
  int supplyV;               // emontx voltage
} Payload;
Payload emontx;     

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
    char buf[150];
    private:
};
PacketBuffer str;

//--------------------------------------------------------------------------
// Ethernet
//--------------------------------------------------------------------------
#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;

char website[] PROGMEM = "www.vis.openenergymonitor.org";

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  
  Ethernet::buffer[off+300] = 0;
  #ifdef DEBUG 
  Serial.println(">>>");
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
  #endif
}
//--------------------------------------------------------------------------
// Flow control varaiables
int dataReady=0;                                                  // is set to 1 when there is data ready to be sent
unsigned long lastRF;                                             // used to check for RF recieve failures
int post_count;                                                   // used to count number of ethernet posts that dont recieve a reply
int dhcp_count =0;

void setup () {
  Serial.begin(9600);
  Serial.println("\n[webClient]");

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
  
  rf12_initialize(MYNODE, freq,group);
  lastRF = millis()-40000;                                        // setting lastRF back 40s is useful as it forces the ethernet code to run straight away
 
}

void loop () {
  
  digitalWrite(6,HIGH);    //turn inidicator LED off! yes off! input gets inverted by buffer

  //---------------------------------------------------------------------
  // On data receieved from rf12
  //---------------------------------------------------------------------
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) 
  {
    digitalWrite(6,LOW);                                          // Flash LED on recieve ON
    emontx=*(Payload*) rf12_data;                                 // Get the payload
    // emontx_nodeID=rf12_hdr & 0x1F;   //extract node ID from received packet 
    
    // JSON creation: JSON sent are of the format: {key1:value1,key2:value2} and so on
    str.reset();                                                  // Reset json string      
    str.print("{rf_fail:0,");                                     // RF recieved so no failure
    str.print("ct1:");    str.print(emontx.ct1);                  // Add CT 1 reading 
    str.print(",battery:");    str.print(emontx.supplyV);         // Add Emontx battery voltage reading

    dataReady = 1;                                                // Ok, data is ready
    lastRF = millis();                                            // reset lastRF timer
    digitalWrite(6,HIGH);                                         // Flash LED on recieve OFF
    #ifdef DEBUG 
      Serial.println("RF recieved");
    #endif
    
  }
  
  // If no data is recieved from rf12 module the server is updated every 30s with RFfail = 1 indicator for debugging
  if ((millis()-lastRF)>30000)
  {
    lastRF = millis();                                            // reset lastRF timer
    str.reset();                                                  // reset json string
    str.print("{rf_fail:1");                                      // No RF received in 30 seconds so send failure 
    dataReady = 1;                                                // Ok, data is ready
  }

  
  ether.packetLoop(ether.packetReceive());
  
  if (dataReady) {
    str.print("}\0");
    
    #ifdef DEBUG 
      Serial.print(str.buf);  
    #endif    // Print final json string to terminal    
    
    // Example of posting to emoncms v3 demo account goto http://vis.openenergymonitor.org/emoncms3 and login with demo:demo
    // To point to your account just enter your WRITE APIKEY 
    ether.browseUrl(PSTR("/emoncms3/api/post.json?apikey=53c13213f301f6427c857a9426ce0efa&json="),str.buf, website, my_callback);
    dataReady =0;
  }
}
