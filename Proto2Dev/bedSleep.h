
#ifndef BEDSLEEP_H
#define BEDSLEEP_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <avr/sleep.h>
#include <avr/wdt.h>

void deepSleep(const uint8_t wdtTime);
void longDeepSleep(uint32_t delay);

#endif