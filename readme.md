# NanodeRF - internet enabled base station

Builds on JeeLabs and EtherCard software. Thanks to JCW and Andrew Lindsay

## To run these examples you will need:
* Arduino 1.x:				http://arduino.cc/en/Main/Software
* EtherCard: 			        https://github.com/jcw/ethercard
* JeeLib:                               Recommend older library snapshot: https://github.com/openenergymonitor/EtherCardOct2012
* RTClib:                               https://github.com/jcw/                                                                                                   

NnodeRF design has been a collaboration between OpenEnergyMonitor.org, Nanode.eu (Ken Boak) and Wicked Device.com

This repo contains the firmware to allow a Nanodes equipped with a RFM12B breakout board or NanodeRF's with RFM12B onboard to received monitoring data from the emonTx and post the data online to emoncms.

The same frequency RFM12B's should be used on both the emonTx and NanodeRF, the frequency of the modules must also be set in the sketch.
 
Older version of the Nanode can be used with certain changes, see: http://openenergymonitor.org/emon/node/227

## NanodeRF Code guide
The NanodeRF code guide goes through main components required to put a full base station firmware together. It's recommended that you work through these examples first so that you have a good understanding of how the full firmware's work.

The guide includes:

* [01 - Fixed Packets](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/a_FixedPackets/a_FixedPackets.ino)
* [02 - MultiNode](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/b_MultiNode/b_MultiNode.ino)
* [03 - Basic Web Client](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/c_BasicWebClient/c_BasicWebClient.ino)
* [04 - PacketBuffer](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/d_PacketBuffer/d_PacketBuffer.ino)
* [05 - Basic MultiNode Web Client](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/e_BasicMultiNodeWebClient/e_BasicMultiNodeWebClient.ino)
* [06 - Receiving a Reply](https://github.com/openenergymonitor/NanodeRF/blob/master/Guide/f_ReceivingReply/f_ReceivingReply.ino)

## Full NanodeRF Firmware's

* **NanodeRF_multinode**
The multinode firmware is the most flexible firmware for posting data up to emoncms, it relays data from up to the 30 nodes that the rfm12 library implementation can handle up to emoncms. You dont need to re-program the firmware each time you add a new node or variables on a particular node. It constructs a CSV format string to be sent to emoncms.

* **NanodeRF_Power_RTCrelay_GLCDtemp**
This firmware was the standard firmware up until the addition of the multinode example. It's build specifically for use with one emontx and one emonglcd. One of its advantages is that it constructs a nice key:value pair json string to be sent to emoncms, which makes the emoncms inputs appear with the names: power1, power2, power3, voltage and so on. The disadvantage with this sketch is that is requires reprogramming if you want to add a new node or add new variables.

* **NanodeRF_Cosm**
Receive data from emonTx via RFM12B wireless and post to Cosm. Just insert your Cosm feed ID and API Key. Thanks to Roger James for this contribution. See forum thread for discussion: http://openenergymonitor.org/emon/node/376

## Documentation Links:
* emonBase :http://openenergymonitor.org/emon/emonbase
* emoncms: http://openenergymonitor.org/emon/emoncms
* emonTx: http://openenergymonitor.org/emon/emontx
* Nanode: http://ichilton.github.com/nanode/
 
