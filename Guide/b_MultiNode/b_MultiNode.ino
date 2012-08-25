/*
  The intention of this method is to get around the issue of having to know the exact number of
  variables as in the previous example.
  
  You can have one node sending 5 integers and another sending 3, it will just print the output.
  The idea is to just relay the data to emoncms where it can be identified and organised there
  rather than on the nanode. Making it possible to have a standard sketch on the base station
  that does not need updating every time variables are added to a node or an additional node is added.
*/

#include <JeeLib.h>	     //https://github.com/jcw/jeelib

void setup () 
{
  Serial.begin(9600);
  Serial.println("EmonBase example 02");
  rf12_initialize(15,RF12_433MHZ,210); // NodeID, Frequency, Group
}

void loop () 
{
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {      
    int node_id = (rf12_hdr & 0x1F);
    byte n = rf12_len;
         
    Serial.print("Node: ");
    Serial.print(node_id);

    Serial.print(" data: ");
    for (byte i=0; i<n; i+=2)
    {
      int num = ((unsigned char)rf12_data[i+1] << 8 | (unsigned char)rf12_data[i]);
      if (i) Serial.print(",");
      Serial.print(num);
    }
    Serial.println();
  }
}
