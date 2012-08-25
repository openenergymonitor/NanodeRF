/*
  The Packet Buffer
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
  Serial.println("d_PacketBuffer");
  
  str.reset();
  str.print("{power1:");  str.print(252.6);
  str.print(",power2:");  str.print(3200);
  str.print(",power3:");  str.print(650);
  str.print("}\0");
  
  Serial.println(str.buf);
}

void loop () { }
