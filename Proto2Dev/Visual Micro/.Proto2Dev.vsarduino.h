/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define _VMDEBUG 1
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

bool sendData(uint8_t* buffer, uint16_t size);
bool getFSRSensor(uint16_t* values);
bool getFSCSensor(uint16_t* values);
char monRead(void);
//
//
bool acquisitionMode(const uint32_t timeMax, const uint32_t fsrDelay, const uint32_t fscDelay, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
uint8_t sleepMode(const uint32_t timeMax, const uint32_t delay, const uint16_t delta, uint8_t buffer[BUFFER_SIZE], uint16_t* bufferPos);
bool mysleep(uint32_t time, uint32_t timeout);

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\Proto2Dev.ino"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\ADS7828.cpp"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\ADS7828.h"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\endian.c"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\endian.h"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\protocol.c"
#include "C:\Fichiers\Cours\ISIMA\Dropbox\ZZ2\Stage\LIRMM\HomeGateway\Mai\BedSensor\Proto2Dev\Proto2Dev\protocol.h"
