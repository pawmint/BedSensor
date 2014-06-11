/**
 *  ADS7828 Driver
 *  
 *  Functions for using the 12-Bit, 8-Channel 
 *  Sampling ANALOG-TO-DIGITAL CONVERTER with I2C Interface ADS7828.
 *  
 *  For more informations, refer to http://www.ti.com/lit/ds/symlink/ads7828.pdf
 *  
 *  @file ADS7828.h
 *  @date 23 may 2014
 *  @copyright PAWN International
 *  
 *  @author Mickael Germain
 *  
 */
 
#ifndef ADS7828_H
 #define ADS7828_H
 
 #if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
 #else
  #include "WProgram.h"
 #endif
 
 #include "Wire.h"
 #include <assert.h>
 #include <inttypes.h>
 
 #define TWI_SUCCESS                0
 #define TWI_DATA_TOO_LONG          1
 #define TWI_NACK_ON_ADDRESS        2
 #define TWI_NACK_ON_DATA           3
 #define TWI_OTHER_ERROR            4
 
 /**
  * Factory pre-set I2C Slace address.
  */
 #define ADS7828_HARDWARE_ADDRESS    0b10010
 
 /**
  * Use differential inputs.
  * 
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_DIFFERENTIAL_I     0b00000000
 
 /**
  * Use single ended inputs.
  * 
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_SINGLE_ENDED_I     0b10000000
 
 /**
  * Don't use the internal reference to perform convertion.
  */
 #define ADS7828_INTERNAL_REF_OFF   0b00000000
 
 /**
  * Use the internal reference to perform convertion.
  * 
  * @note The turn on time isn't insignificant. Made sure to let minimum delay.
  *       High level functions as ADS7828_configAndGet() don't take care of this
  *       turn on time. To use reference in these funtions, turn on internal reference 
  *       previously using ADS7828_config() .
  * 
  * @see ADS7828_waitRefTurnOn()
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_INTERNAL_REF_ON    0b00001000
 
 /**
  * Turn off A/D converter.
  * 
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_AD_CONVERTER_OFF   0b00000000
 
 /**
  * Turn on A/D converter.
  * 
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_AD_CONVERTER_ON    0b00000100
 
 /**
  * Number of available analog channel on ADS7828.
  * 
  * @see ADS7828_config()
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  */
 #define ADS7828_NB_CHANNEL         8
 
 /**
  * Resolution of the A/D converter in bit.
  */
 #define ADS7828_RESOLUTION         4096
 
 /**
  * Prepares arduino for communication with ADS7828.
  * 
  * @warning If arduino act as a I2C slave the communication will be broken.
  * @pre I2C pin need to be free.
  */
 #define ADS7828_init() Wire.begin()
 
 /**
  * Power down the ADS7828.
  * 
  * @param [in] i2cAddr
  *     ADS7828's I2C address on the bus [0;4[.
  * 
  * @note To further information refer to ADS7828_config() .
  */
 #define ADS7828_powerDown(addr) ADS7828_config(addr, ADS7828_INTERNAL_REF_OFF | ADS7828_AD_CONVERTER_OFF, 0, true)
 
 /**
  * Wait during internal reference's turn on time.
  */
 #define ADS7828_waitInternalRefTurnOn() delayMicroseconds(1240)
 
 /**
  * Enters in high speed mode (3.4 MHz).
  * 
  * To remain in HS mode, set "stop" parameter to false.
  * 
  * @return true, if communication fall into high speed mode, false otherwise.
  * 
  */
 bool ADS7828_startHSMode(void);
 
 /**
  * Sends a configuration command to the ADS7828.
  * 
  * @param [in] i2cAddr
  *     ADS7828's I2C address on the bus [0;4[.
  * @param [in] flags
  *     Flags for configuration commands.
  * @param [in] channelNumber
  *     Channel number to be sampled [0;ADS7828_NB_CHANNEL[.
  * @param [in] stop
  *     Set at true to stop the I2C communication at the end of the command.
  * 
  * @return TWI_SUCCESS, if configuration succeded, else an error among
  *         TWI_DATA_TOO_LONG
  *         TWI_NACK_ON_ADDRESS
  *         TWI_NACK_ON_DATA
  *         TWI_OTHER_ERROR 
  * 
  */
 uint8_t ADS7828_config(const uint8_t i2cAddr, const uint8_t flags, const uint8_t channelNumber, const bool stop);
 
 /**
  * Retrieves sampled value from ADS7828.
  * 
  * @param [in] i2cAddr
  *     ADS7828's I2C address on the bus [0;4[. 
  * @param [in] stop
  *     Set at true to stop the I2C communication at the end of the command.
  * 
  * @return true hopefully, false otherwise.
  * 
  * @see ADS7828_configAndGet()
  * @see ADS7828_getAllValues()
  * @see ADS7828_configAndGet()
  */
 bool ADS7828_getValue(const uint8_t i2cAddr, const bool stop, uint16_t* value);
 
 /**
  * Configure the ADS7828 and then retrieves sampled value.
  * 
  * @param [in] i2cAddr
  *     ADS7828's I2C address on the bus [0;4[. 
  * @param [in] flags
  *     Flags for configuration commands.
  * @param [in] channelNumber
  *     Channel number to be sampled [0;ADS7828_NB_CHANNEL[.
  * @param [in] stop
  *     Set at true to stop the I2C communication at the end of the command.
  * @param [out] value
  *     Reference to store the sampled value.
  * 
  * @return TWI_SUCCESS, if operation succeded, else an error among
  *         TWI_DATA_TOO_LONG
  *         TWI_NACK_ON_ADDRESS
  *         TWI_NACK_ON_DATA
  *         TWI_OTHER_ERROR 
  * 
  */
 uint8_t ADS7828_configAndGet(const uint8_t i2cAddr, const uint8_t flags, const uint8_t channelNumber, const bool stop, uint16_t* value);
 
  /**
  * Retrieves sampled value from ADS7828.
  * 
  * @param [in] i2cAddr
  *     ADS7828's I2C address on the bus [0;4[.
  * @param [in] flags
  *     Flags for configuration commands.
  * @param [out] values
  *     Array where store data from ADS7828.
  * 
  * @return TWI_SUCCESS, if operation succeded, else an error among
  *         TWI_DATA_TOO_LONG
  *         TWI_NACK_ON_ADDRESS
  *         TWI_NACK_ON_DATA
  *         TWI_OTHER_ERROR 
  * 
  * @see ADS7828_getValue()
  */
 uint8_t ADS7828_getAllValues(const uint8_t i2cAddr, const uint8_t flags,  const bool stop, uint16_t values[ADS7828_NB_CHANNEL]);
 
#endif
