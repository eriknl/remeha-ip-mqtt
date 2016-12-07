# remeha-ip-mqtt
Remeha 'serial-over-ip' to MQTT bridge

Simple implementation of the Remeha protocol to read Remeha boilers. It has only been tested on the model AvantaV1_P1, feel free to give feedback about other models. The implementation based on the XML files supplied with Recom which cannot be reproduced here.

Information on the Paho MQTT library can be found at https://www.eclipse.org/paho/clients/c/

This software will connect to a given serial to TCP server and write updates to a MQTT broker every 60 seconds.
Invoke with:
```
remeha-ip-mqtt <Remeha host> <Remeha port> <MQTT broker host> <MQTT clientid> <MQTT topic>
```
Note that you will need some way to access the serial port on the boiler over TCP, this can be done with [sernet](https://github.com/eriknl/sernet)

As it is not a proper daemon you'll need to run in through screen or write a wrapper script around it.

The Remeha name and the Remeha product and services names are trade marks of Remeha B.V.
The author of this software has no affiliation whatsoever with Remeha B.V.
