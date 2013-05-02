/*

  The disadvantage with the a_FixedPackets method is that any addition of variables on a sensor node, 
  or any addition of sensor nodes requires reprogramming of the basestation.
  
  This example gets around the issue by standardising only on the variable format
  
  If each variable sent from a node is an integer which is 2 bytes of data and the integers are packed consecutively i.e:
  
  | integer 1 (2 bytes) | integer 2 (2 bytes) | integer 3 (2 bytes | ... | 
  
  Then we can calculate the number of variables that are contained in a packet of data by the length of the packet:
  
  number_of_variables = packet_length / 2
  
  Its easy to extract the variables from the packet we just go through the packet 2 bytes at a time creating an integer
  from each block of two bytes.
  
  With this method you can have one node sending 5 integers and another sending 3, it will just print the output.
  The idea is to just relay the data to emoncms where it can be identified and organised there
  rather than on the emonbase. Making it possible to have a standard sketch on the base station
  that does not need updating every time variables are added to a node or an additional node is added.

  -----------------------------------------
  Part of the openenergymonitor.org project
  Licence: GNU GPL V3 
*/

#include <JeeLib.h>	     //https://github.com/jcw/jeelib

void setup () 
{
  Serial.begin(9600);
  Serial.println("02 multi node");
  rf12_initialize(15,RF12_433MHZ,210); // NodeID, Frequency, Group
}

void loop () 
{
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {      
    int node_id = (rf12_hdr & 0x1F);
    
    // Get the packet length (number of integers sent is n/2)
    byte n = rf12_len;
         
    Serial.print("Node: ");
    Serial.print(node_id);

    Serial.print(" data: ");
    
    // Go throught the packet 2 bytes at a time
    for (byte i=0; i<n; i+=2)
    {
      // Create an integer from each block of two bytes (using bitwise math)
      int num = ((unsigned char)rf12_data[i+1] << 8 | (unsigned char)rf12_data[i]);
      
      // Print the output as CSV
      if (i) Serial.print(",");
      Serial.print(num);
    }
    Serial.println();
  }
}
