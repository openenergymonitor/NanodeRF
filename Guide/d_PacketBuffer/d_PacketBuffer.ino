/*
  The Packet Buffer
  
  In the last example (BasicWebClient) we sent a semi-json like string of test data: {power:252.4,temperature:15.4}. 
  This example details how we can construct this string dynamically - so that the values can change.
  
  It uses the PacketBuffer class written by Jean Claude Wippler of JeeLabs.org: http://jeelabs.org/2010/09/29/sending-strings-in-packets/
  which makes it easy to fill a buffer with string data - that can then be sent in the URL request.
  
  The PacketBuffer class makes use of the arduino print class. 

  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3 
*/

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

void setup () 
{
  Serial.begin(9600);
  Serial.println("04 - PacketBuffer");
}

void loop () 
{ 
  str.reset();
  str.print("{power1:");  str.print(252.6);
  str.print(",power2:");  str.print(3200);
  str.print(",power3:");  str.print(650);
  str.print("}\0");
  
  Serial.println(str.buf);
  delay(1000);
}
