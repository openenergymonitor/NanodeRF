/*                          _                                                      _      
                           | |                                                    | |     
  ___ _ __ ___   ___  _ __ | |__   __ _ ___  ___       _ __   __ _ _ __   ___   __| | ___ 
 / _ \ '_ ` _ \ / _ \| '_ \| '_ \ / _` / __|/ _ \     | '_ \ / _` | '_ \ / _ \ / _` |/ _ \
|  __/ | | | | | (_) | | | | |_) | (_| \__ \  __/  _  | | | | (_| | | | | (_) | (_| |  __/
 \___|_| |_| |_|\___/|_| |_|_.__/ \__,_|___/\___| (_) |_| |_|\__,_|_| |_|\___/ \__,_|\___|
                                                                                          
*/
//--------------------------------------------------------------------------------------
// Relay's data recieved by emontx up to emoncms

// emonBase Documentation: http://openenergymonitor.org/emon/emonbase

// Authors: Trystan Lea and Glyn Hudson
// Part of the: openenergymonitor.org project
// Licenced under GNU GPL V3
//http://openenergymonitor.org/emon/license

// EtherCard Library by Jean-Claude Wippler and Andrew Lindsay
// JeeLib Library by Jean-Claude Wippler
//--------------------------------------------------------------------------------------

#define DEBUG 

#include <JeeLib.h>	     //https://github.com/jcw/jeelib
#include <avr/wdt.h>

#define MYNODE 35            // node ID 30 reserved for base station
#define freq RF12_433MHZ     // frequency
#define group 210            // network group 

// The RF12 data payload - a neat way of packaging data when sending via RF - JeeLabs
// must be same structure as transmitted from emonTx
typedef struct
{
  int ct1;		     // current transformer 1
  //int ct2;                 // current transformer 2 - un-comment as appropriate 
  //int ct3;                 // current transformer 1 - un-comment as appropriate 
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
#include <EtherCard.h>		//https://github.com/jcw/ethercard 

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x44,0x29,0x49,0x21,0x30,0x31 };

//IP address of remote sever, only needed when posting to a server that has not got a dns domain name (staticIP e.g local server) 
byte Ethernet::buffer[700];
static uint32_t timer;

//Domain name of remote webserver - leave blank if posting to IP address 
char website[] PROGMEM = "vis.openenergymonitor.org";
//static byte hisip[] = { 213,138,101,177 };    // un-comment for posting to static IP server (no domain name)            

//--------------------------------------------------------------------------
// Flow control varaiables
int dataReady=0;                                                  // is set to 1 when there is data ready to be sent
unsigned long lastRF;                                             // used to check for RF recieve failures
int post_count;                                                   // used to count number of ethernet posts that dont recieve a reply
int dhcp_count =0;

//NanodeRF error indication LED variables 
const int redLED=6;                      //NanodeRF RED indicator LED
const int greenLED=5;                    //NanodeRF GREEN indicator LED
int error=0;                             //Etherent (controller/DHCP) error flag
int RFerror=0;                           //RF error flag - high when no data received 

int dhcp_status = 0;
int dns_status = 0;
int request_attempt = 0;

char line_buf[50];

//-----------------------------------------------------------------------------------
// Ethernet callback
// recieve reply and decode
//-----------------------------------------------------------------------------------
static void my_callback (byte status, word off, word len) {
  
  get_header_line(2,off);      // Get the date and time from the header
  Serial.println(line_buf);    // Print out the date and time
  
  get_reply_data(off);
  if (strcmp(line_buf,"ok")) {Serial.println("ok recieved"); request_attempt = 0;}
  
}

//**********************************************************************************************************************
// SETUP
//**********************************************************************************************************************
void setup () {
  
  //Nanode RF LED indictor  setup - green flashing means good - red on for a long time means bad! 
  //High means off since NanodeRF tri-state buffer inverts signal 
  pinMode(redLED, OUTPUT); digitalWrite(redLED,LOW);            
  pinMode(greenLED, OUTPUT); digitalWrite(greenLED,LOW);       
  delay(100); digitalWrite(redLED,HIGH);                        //turn off redLED
  
  Serial.begin(9600);
  Serial.println("\n[webClient]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
    Serial.println( "Failed to access Ethernet controller");
    error=1;  
  }
 
  rf12_initialize(MYNODE, freq,group);
  lastRF = millis()-40000;                                        // setting lastRF back 40s is useful as it forces the ethernet code to run straight away
   
  digitalWrite(greenLED,HIGH);                                    //Green LED off - indicate that setup has finished
 
  wdt_enable(WDTO_8S); 
}
//**********************************************************************************************************************


//**********************************************************************************************************************
// LOOP
//**********************************************************************************************************************
void loop () {
  
  wdt_reset();
  //-----------------------------------------------------------------------------------
  // Get DHCP address
  // Putting DHCP setup and DNS lookup in the main loop allows for: 
  // powering nanode before ethernet is connected
  //-----------------------------------------------------------------------------------
  if (ether.dhcpExpired()) dhcp_status = 0;    // if dhcp expired start request for new lease by changing status
  
  if (!dhcp_status){
    wdt_disable();
    dhcp_status = ether.dhcpSetup();           // DHCP setup
    wdt_enable(WDTO_8S);
    Serial.print("DHCP status: ");             // print
    Serial.println(dhcp_status);               // dhcp status
    
    if (dhcp_status){                          // on success print out ip's
      ether.printIp("IP:  ", ether.myip);
      ether.printIp("GW:  ", ether.gwip);  
      
      static byte dnsip[] = {8,8,8,8};  
      ether.copyIp(ether.dnsip, dnsip);
      ether.printIp("DNS: ", ether.dnsip);                            //comment out this line if posting to a static IP server this includes local host 
      //ether.copyIp(ether.hisip, hisip);                             // un-comment for posting to static IP server (no domain name)
      //dns_status = 1;                                               // un-comment for posting to static IP server (no domain name)            
    } else { error=1; }  
  }
  
  //-----------------------------------------------------------------------------------
  // Get server address via DNS
  //-----------------------------------------------------------------------------------
  if (dhcp_status && !dns_status){
    wdt_disable();
    dns_status = ether.dnsLookup(website);    // Attempt DNS lookup
    wdt_enable(WDTO_8S);
    Serial.print("DNS status: ");             // print
    Serial.println(dns_status);               // dns status
    if (dns_status){
      ether.printIp("SRV: ", ether.hisip);      // server ip
    } else { error=1; }  
  }
  
  if (error==1 || RFerror==1 || request_attempt > 0) digitalWrite(redLED,LOW);      //turn on red LED if RF / DHCP or Etherent controllor error. Need way to notify of server error
    else digitalWrite(redLED,HIGH);

  //---------------------------------------------------------------------
  // On data receieved from rf12
  //---------------------------------------------------------------------
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) 
  {
    digitalWrite(greenLED,LOW);                                   // turn green LED on to indicate RF recieve 
    emontx=*(Payload*) rf12_data;                                 // Get the payload
    // emontx_nodeID=rf12_hdr & 0x1F;                             //extract node ID from received packet - only needed when multiple emonTx are posting on same network 
    
    RFerror=0;                                                    //reset RF error flag
    
    // JSON creation: JSON sent are of the format: {key1:value1,key2:value2} and so on
    str.reset();                                                 // Reset json string      
    str.print("{rf_fail:0");                                     // RF recieved so no failure
    str.print(",ct1:");    str.print(emontx.ct1);                // Add CT 1 reading 
    //str.print(",ct2:");    str.print(emontx.ct2);              // Add CT 2 reading  - un-comment if needed
    //str.print(",ct3:");    str.print(emontx.ct3);              // Add CT 3 reading  - un-comment if needed
    str.print(",battery:");    str.print(emontx.supplyV);        // Add emontx battery voltage reading

    dataReady = 1;                                                // Ok, data is ready
    lastRF = millis();                                            // reset lastRF timer
    digitalWrite(greenLED,HIGH);                                  // Turn green LED on OFF
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
    RFerror=1;
  }

  
  ether.packetLoop(ether.packetReceive());
  
  if (dataReady) {
    str.print("}\0");
    
    #ifdef DEBUG 
      Serial.println(str.buf);  
      Serial.println(request_attempt);  
    #endif    // Print final json string to terminal    
    
    // Example of posting to emoncms v3 demo account goto http://vis.openenergymonitor.org/emoncms3 
    // and login with sandbox:sandbox
    // To point to your account just enter your WRITE APIKEY 
    request_attempt ++;
    ether.browseUrl(PSTR("/emoncms3/api/post.json?apikey=ff64c806e5b618c64708280f868a39e0&json="),str.buf, website, my_callback);
    dataReady =0;
  }
  
  if (request_attempt > 10) delay(10000); // Reset the nanode if more than 10 request attempts have been tried without a reply
}
//**********************************************************************************************************************
