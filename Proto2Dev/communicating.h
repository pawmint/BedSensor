

#ifndef COMMUNICATING_H
#define COMMUNICATING_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <assert.h>
#include <Wire.h>
#include "xbee_api.h"
#include "xbee_api_def.h"
#include "protocol.h"

#define COMMUNICATING_DEFAULT_SLEEP_PIN 4

void setDestinationAddress(long long destinationAddress);
void setSleepPin(uint8_t pin);
void communicating(uint8_t* buffer, uint16_t* length);

#endif

