# NanodeRF - internet enabled base station

Builds on JeeLabs and EtherCard software. Thanks to JCW and Andrew Lindsay

## To run these examples you will need:
* Arduino 1.x:				http://arduino.cc/en/Main/Software
* EtherCard: 					https://github.com/jcw/ethercard
* JeeLib: 					   https://github.com/jcw/jeelib
* RTClib:         https://github.com/jcw/                                                                                                   

The is a collaboration between OpenEnergyMonitor.org and Nanode.eu (Ken Boak) 

This repo contains the firmware to allow a Nanodes equipped with a RFM12B breakout board or NanodeRF's with RFM12B onboard to received monitoring data from the emonTx and post the data online to emoncms.

The same frequency RFM12B's should be used on both the emonTx and NanodeRF, the frequency of the modules must also be set in the sketch.
 
Older version of the Nanode can be used with certain changes, see: http://openenergymonitor.org/emon/node/227

## NanodeRF Examples

* **NanodeRF_multinode**
The multinode firmware is the most flexible firmware for posting data up to emoncms, it relays data from up to the 30 nodes that the rfm12 library implementation can handle up to emoncms. You dont need to re-program the firmware each time you add a new node or variables on a particular node. It constructs a CSV format string to be sent to emoncms.

* **NanodeRF_Power_RTCrelay_GLCDtemp**
This firmware was the standard firmware up until the addition of the multinode example. It's build specifically for use with one emontx and one emonglcd. One of its advantages is that it constructs a nice key:value pair json string to be sent to emoncms, which makes the emoncms inputs appear with the names: power1, power2, power3, voltage and so on. The disadvantage with this sketch is that is requires reprogramming if you want to add a new node or add new variables.

* **NanodeRF_Cosm**
Receive data from emonTx via RFM12B wireless and post to Cosum. Just insert your Cosm feed ID and API Key. Thanks to Roger James for this contribution. See forum thread for discussion: http://openenergymonitor.org/emon/node/376

## Documentation Links:
* emonBase :http://openenergymonitor.org/emon/emonbase
* emoncms: http://openenergymonitor.org/emon/emoncms
* emonTx: http://openenergymonitor.org/emon/emontx
* Nanode: http://ichilton.github.com/nanode/
 
