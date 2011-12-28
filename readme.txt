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
*********************************************************************

Builds on JeeLabs and EtherCard software. Thanks to JCW and Andrew Lindsay
-----------------------------------------------
To make this work you will need:
Arduino 1.0:					http://arduino.cc/en/Main/Software
EtherCard: 					https://github.com/jcw/ethercard
JeeLib: 					https://github.com/jcw/jeelib

-----------------------------------------------
The is a collaboration between OpenEnergyMonitor.org and Nanode.eu (Ken Boak) 

This repo contains the firmware to allow a Nanodes equipped with a RFM12B breakout board or NanodeRF's with RFM12B onboard to received monitoring data from the emonTx and post the data online to emoncms.

The same frequency RFM12B's should be used on both the emonTx and NanodeRF, the frequency of the modules must also be set in the sketch.
 

Older version of the Nanode can be used with certain changes, see: http://openenergymonitor.org/emon/node/227

For Older emonBase/NanodeRF examples which use the EtherShield library (now un-supported) see: https://github.com/openenergymonitor/NanodeRF_ethershield

Documentation Links:
emonBase : http://openenergymonitor.org/emon/emonbase
emonTx: http://openenergymonitor.org/emon/emontx
Nanode: http://ichilton.github.com/nanode/
 
