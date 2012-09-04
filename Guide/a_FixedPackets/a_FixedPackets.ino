/*
  The first task of the base station is to receive data packets from the wireless nodes.
  
  This example shows the simplest way to receive packets it uses a struct: "a structured record
  type that aggregates a fixed set of labelled objects, possibly of different types, into a single object" - Wikipedia
  
  This struct is used to tell the program how to extract individual elements from the packet data received.
  It tells the program that the first variable is an integer or second could be a float for example.
  It requires that the same struct definition is used on the transmitting node.

  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3
*/

#include <JeeLib.h>	     //https://github.com/jcw/jeelib

// Define the data structure of the packet to be recieved 
typedef struct { int power, voltage; } PayloadTX;

// Create a variable to hold the received data of the defined structure .
PayloadTX emontx;

void setup () 
{
  Serial.begin(9600);
  Serial.println("01 fixed packets");
  rf12_initialize(15,RF12_433MHZ,210); // NodeID, Frequency, Group
}

void loop () 
{
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {
    int node_id = (rf12_hdr & 0x1F);
    
    if (node_id == 10)                 
    {
      // The packet data is contained in rf12_data, the *(PayloadTX*) part tells the compiler 
      // what the format of the data is so that it can be copied correctly
      emontx = *(PayloadTX*) rf12_data;

      Serial.print(emontx.power);
      Serial.print(' ');
      Serial.println(emontx.voltage);
    }
  }
}
