                               _      
                              | |     
 _ __   __ _   _ __   ___   __| | ___ 
| '_ \ / _` | | '_ \ / _ \ / _` |/ _ \
| | | | (_| | | | | | (_) | (_| |  __/
|_| |_|\__,_| |_| |_|\___/ \__,_|\___| RF
                             _   _          
   ___ _ __ ___   ___  _ __ | |_) | __ _ ___  ___ 
  / _ \ '_ ` _ \ / _ \| '_ \|  _ < / _` / __|/ _ \
 |  __/ | | | | | (_) | | | | |_) | (_| \__ \  __/
  \___|_| |_| |_|\___/|_| |_|____/ \__,_|___/\___|                                    

                                                  
                                                  
emonBase (emonTx base station) to post data to emoncms or Pachube 
**************************

Builds on JeeLabs and EtherCard software. Thanks to JCW and Andrew Lindsay
-----------------------------------------------
To make this work you will need:
Arduino 1.0:				http://arduino.googlecode.com/files/arduino-1.0-linux.tgz
EtherCard development branch: 		https://github.com/jcw/ethercard/tarball/development
JeeLib: 				https://github.com/jcw/jeelib

-----------------------------------------------
The is a collaboration between OpenEnergyMonitor.org and Nanode.eu (Ken Boak and London Hackspace) 

This repo contains the firmware to allow a Nanodes equipped with a RFM12B breakout board or NanodeRF's with RFM12 onboard to received monitoring data from the emonTx and post the data online to emoncms. The Nanode also serves a local webpage with a copy of the last received data packet. 

Older version of the Nanode can be used with certain changes, see: http://openenergymonitor.org/emon/node/143

Links:
emonTx: openenergymonitor.org/emon/emontx
Nanode: http://wiki.hackspace.org.uk/wiki/Project:Nanode
 
