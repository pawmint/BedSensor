BedSensor
=========

Integration of the bed sensors into an arduino

The program :

- Configure Xbee for API Mode
- Send a YOP frame (old style with sensor number)
- Enter in infinite loop :
  
  + If sensors values change then send a DR1 frame
  + Else send a ACK frame periodicaly
