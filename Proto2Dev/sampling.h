

#ifndef SAMPLING_H
#define SAMPLING_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include "stdint.h"
#include "ADS7828.h"

uint8_t getBatteryLvl(void);
bool getFSRSensor(uint16_t* values);
bool getFSCSensor(uint16_t* values);

#endif