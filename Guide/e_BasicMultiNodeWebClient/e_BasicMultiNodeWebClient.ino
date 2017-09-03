/*
  This example is the barebones form of the NanodeRF_multinode example
  
  It combines the c_BasicWebClient example with the d_PacketBuffer and the b_MultiNode example
  
  It uses the code detailed in the b_MultiNode example to receive the RF data packet.
  The d_PacketBuffer code to construct a CSV string ready to be sent to emoncms
  and c_BasicWebClient to send the request.
  
  You will need to put in your write api key on line 85
  
  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
*/

#define RF69_COMPAT  0                                                // Set to 1 if using RFM69CW or 0 is using RFM12B

#include <JeeLib.h>	     //https://github.com/jcw/jeelib

class PacketBuffer : public Print {
public:
    PacketBuffer () : fill (0) {}
    const char* buffer() { return buf; }
    byte length() { return fill; }
    void reset()
    { 
      memset(buf,0,sizeof(buf));
      fill = 0; 
    }
    virtual size_t write (uint8_t ch)
        { if (fill < sizeof buf) buf[fill++] = ch; }
    byte fill;
    char buf[150];
    private:
};
PacketBuffer str;

#include <EtherCard.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];

const char website[] PROGMEM = "emoncms.org";

void setup () 
{
  Serial.begin(9600);
  Serial.println("05 - Basic MultiNode Web Client");

  if (ether.begin(sizeof Ethernet::buffer, mymac, 8) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
  
  rf12_initialize(15,RF12_433MHZ,210); // NodeID, Frequency, Group
}

void loop () 
{
  ether.packetLoop(ether.packetReceive());
  
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {      
    int node_id = (rf12_hdr & 0x1F);
    byte n = rf12_len;
         
    str.reset();
    str.print("&node=");  str.print(node_id);
    str.print("&csv=");
    for (byte i=0; i<n; i+=2)
    {
      int num = ((unsigned char)rf12_data[i+1] << 8 | (unsigned char)rf12_data[i]);
      if (i) str.print(",");
      str.print(num);
    }

    str.print("\0");  //  End of json string    
    Serial.print("Data sent: "); Serial.println(str.buf);
    ether.browseUrl(PSTR("/api/post?apikey=YOURAPIKEY"),str.buf, website, 0);
  }

}
