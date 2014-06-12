/**
 *  ADS7828 Driver
 *  
 *  Functions for using the 12-Bit, 8-Channel 
 *  Sampling ANALOG-TO-DIGITAL CONVERTER with I2C Interface ADS7828.
 *  
 *  For more informations, refer to http://www.ti.com/lit/ds/symlink/ads7828.pdf .
 *  
 *  @file ADS7828.cpp
 *  @date 23 may 2014
 *  @copyright PAWM International
 *  
 *  @author Mickael Germain
 *  
 *  @bug startHSMode don't work
 */

#include "ADS7828.h"

/* TODO correct it. */
bool ADS7828_startHSMode(void)
{
    Wire.beginTransmission(0b0000100);
    return (Wire.endTransmission(false) == TWI_NACK_ON_ADDRESS);
}

uint8_t ADS7828_config(const uint8_t i2cAddr, const uint8_t flags, const uint8_t channelNumber, const bool stop)
{
    /* Channel selection bits free. */
    assert((flags & 0x70) == 0x0);
    assert(i2cAddr < 4);
    assert(channelNumber < ADS7828_NB_CHANNEL);
    
    /* Address Byte :
     * MSB|6|5|4|3| 2| 1|LSB
     *   1|0|0|1|0|A1|A0|R/W
     * 
     * Omit the LSB
     */
    Wire.beginTransmission((ADS7828_HARDWARE_ADDRESS << 2) | i2cAddr);
    
    /*  Command Byte :
     *  MSB| 6| 5| 4|  3|  2|1|LSB
     *   SD|C2|C1|C0|PD1|PD0|X|  X
     */
    Wire.write(flags | (channelNumber << 4));
    
    return Wire.endTransmission(stop);		
}

bool ADS7828_getValue(const uint8_t i2cAddr, const bool stop, uint16_t* value)
{
    bool ret = false;
    
    assert(i2cAddr < 4);
    
    if (Wire.requestFrom((uint8_t)((ADS7828_HARDWARE_ADDRESS << 2) | i2cAddr), (uint8_t)2, (uint8_t)stop) == 2)
    {
       /* High part */
       *value = Wire.read() * 256;
       /* Low part */
       *value += Wire.read();
       ret = true;
       
       assert(*value < ADS7828_RESOLUTION);
    }
    
    return ret;
}

uint8_t ADS7828_configAndGet(const uint8_t i2cAddr, const uint8_t flags, const uint8_t channelNumber, const bool stop, uint16_t* value)
{
    uint8_t ret;
    
    if ((ret = ADS7828_config(i2cAddr, flags, channelNumber, false)) != TWI_SUCCESS)
        return ret;
          
    if (!ADS7828_getValue(i2cAddr, stop, value))
        return TWI_OTHER_ERROR;
    
    return TWI_SUCCESS;
}

uint8_t ADS7828_getAllValues(const uint8_t i2cAddr, const uint8_t flags, const bool stop, uint16_t values[ADS7828_NB_CHANNEL])
{
    uint8_t ret;
    
    uint8_t i;
    
    for (i = 0 ; i < ADS7828_NB_CHANNEL - 1 ; i++)
    {
       ret = ADS7828_configAndGet(i2cAddr, flags, i, false, values + i);
       if (ret != TWI_SUCCESS)
          return ret;
    }
    
    return ADS7828_configAndGet(i2cAddr, flags, i, stop, values + i);
}
