/*
  The first task of the base station is to recieve data packets from the wireless nodes.
  
  This example shows the simplest way to recieve packets.
  
  To work the basestation needs to know the structure of the data as set on the emontx.
  
  Which means that any addition of variables on a sensor node, or any addition of sensor
  nodes requires reprogramming of the basestation. Depending on your needs this might be 
  fine, however the next example shows a method that is flexible for any number of variables
  as long as the structure is a series of integers.
*/

#include <JeeLib.h>	     //https://github.com/jcw/jeelib

typedef struct { int power1, power2, power3, voltage; } PayloadTX;
PayloadTX emontx;

void setup () 
{
  Serial.begin(9600);
  Serial.println("EmonBase example 01");
  rf12_initialize(15,RF12_433MHZ,210); // NodeID, Frequency, Group
}

void loop () 
{
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {
    int node_id = (rf12_hdr & 0x1F);
    
    if (node_id == 10)                 
    {
      emontx = *(PayloadTX*) rf12_data;
      Serial.println(emontx.power1);
    }
  }
}
