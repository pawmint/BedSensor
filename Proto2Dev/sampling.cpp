#include "sampling.h"

uint8_t getBatteryLvl(void)
{
    return 42;
}

bool getFSRSensor(uint16_t* values)
{
    #ifdef ONESENSOR
    values[0] = analogRead(A0) << 2;
    #else
    ADS7828_getAllValues(0b00, ADS7828_AD_CONVERTER_ON | ADS7828_INTERNAL_REF_ON | ADS7828_SINGLE_ENDED_I, false, values);
    #endif
    return true;
}

bool getFSCSensor(uint16_t* values)
{
    return true;
}