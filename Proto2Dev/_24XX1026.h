/**
 *  24XX1026 memory family's driver for Arduino.
 *  
 *  Functions for using 1024K I2C Serial EEPROM 24AA1026, 24LC1026 and 24FC1026.
 *  
 *  For more informations, refer to the <a href="http://ww1.microchip.com/downloads/en/DeviceDoc/20002270D.pdf">24XX1026 family's data sheet</a>.
 *  
 *  @file _24XX1026.h
 *  @date 30 jun 2014
 *  @copyright PAWM International
 *  
 *  @author Mickael Germain
 *  
 */

#ifndef _24XX1026_H
 #define _24XX1026_H
 
 #if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
 #else
  #include "WProgram.h"
 #endif

 /**
  *	Size of the entire memory in bytes.
  */
 #define _24XX1026_MEMORY_SIZE          128 * 512 * 2

 /**
  *	Number of bloc in the memory.
  *  
  *  @note The memory is divided in multiple part call block.
  */
 #define _24XX1026_NB_BLOC              2

 /**
  *	Size of a bloc in bytes
  */
 #define _24XX1026_BLOC_SIZE            _24XX1026_MEMORY_SIZE / _24XX1026_NB_BLOC

 /**
  *	Size of a page in bytes.
  *	
  *	@note A block is divided in multiple page. It's the biggest contiguous area of 
  *  memory who could be written in one time.
  */
 #define _24XX1026_PAGE_SIZE            128

 /**
  *	Size of TWI buffer required by 24XX1026 to work.
  */
 #define _24XX1026_TWI_BUFFER_LENGTH    _24XX1026_PAGE_SIZE + 2

 /**
  * @def TWI_BUFFER_LENGTH
  * @brief Size of TWI buffer.
  * 
  * @see twi.h
  */
 #ifndef TWI_BUFFER_LENGTH
  #define TWI_BUFFER_LENGTH  _24XX1026_TWI_BUFFER_LENGTH
 #else
  #if TWI_BUFFER_LENGTH < _24XX1026_TWI_BUFFER_LENGTH
   #undef TWI_BUFFER_LENGTH
   #define TWI_BUFFER_LENGTH _24XX1026_TWI_BUFFER_LENGTH
  #endif
 #endif

 #include "Wire.h"
 #include <assert.h>
 #include <inttypes.h>
 
 /* I have performed a pull request on arduino@github in order to add 
  * following defines in twi.c and twi.h
  */
 #ifndef TWI_SUCCESS
  #define TWI_SUCCESS                   0
 #endif
 #ifndef TWI_DATA_TOO_LONG
  #define TWI_DATA_TOO_LONG             1
 #endif
 #ifndef TWI_NACK_ON_ADDRESS 
  #define TWI_NACK_ON_ADDRESS           2
 #endif
 #ifndef TWI_NACK_ON_DATA
  #define TWI_NACK_ON_DATA              3
 #endif
 #ifndef TWI_OTHER_ERROR
  #define TWI_OTHER_ERROR               4
 #endif
 
 /**
  * Factory pre-set I2C Slave address of 24XX1026 memory's family.
  */
 #define _24XX1026_HARDWARE_ADDRESS    0b1010

 /**
  * @def _24XX1026_init()
  * @brief Prepares arduino for communication with 24XX1026.
  * 
  * @warning If arduino act as a I2C slave the communication will be broken.
  * @pre I2C pin need to be free.
  */
 
 #ifdef _24XX1026_WP
  #define _24XX1026_init() Wire.begin(); pinMode(_24XX1026_WP, OUTPUT)
  /**
   *	@def _24XX1026_writeProtect()
   * @brief Allows or inhibits write operations on the memory.
   * 
   * To use this function you should link an arduino's pin with 
   * the WP pin of the 24XX1026 memory. To identify WP pin, refer
   * to the datasheet. The selected arduino's pin should be provided through 
   * defining _24XX1026_WP with the id of the pin.
   * 
   * @param [in] bProtect
   *     true or false to respectively inhibits or allows write operations.
   * 
   * @pre _24XX1026_WP need to be defined.
   */
  #define _24XX1026_writeProtect(bProtect) digitalWrite(_24XX1026_WP, (bProtect ? HIGH : LOW))
 #else
  #define _24XX1026_init() Wire.begin()
 #endif

 /**
  * Writes up to one page of data in memory.
  * 
  * Extract from data sheet <a href="http://ww1.microchip.com/downloads/en/DeviceDoc/20002270D.pdf#G1.1034634">write operation section</a> :
  * 
  * > Page write operations are limited to writing
  * > bytes within a single physical page,
  * > regardless of the number of bytes actually
  * > being written. Physical page boundaries
  * > start at addresses that are integer
  * > multiples of the page buffer size (or 'page
  * > size') and end at addresses that are
  * > integer multiples of [page size - 1]. If a
  * > Page Write command attempts to write
  * > across a physical page boundary, the
  * > result is that the data wraps around to the
  * > beginning of the current page (overwriting
  * > data previously stored there).
  * 
  * @param [in] i2cAddr
  *     24XX1026's I2C address on the bus [0;4[.
  * @param [in] block_id
  *     Identifier of the block containing the page to be written [0;_24XX1026_NB_BLOC[.
  * @param [in] writeAddr
  *     Address in the block where start writing [0;_24XX1026_BLOC_SIZE[.
  * @param [in] data
  *     Reference to the array where find data to be written in the page.
  * @param [in] nbData
  *     Number of data to be written  [0;_24XX1026_PAGE_SIZE[.
  * 
  * @return TWI_SUCCESS, if writing succeeded, else an error among
  *         - TWI_DATA_TOO_LONG
  *         - TWI_NACK_ON_ADDRESS
  *         - TWI_NACK_ON_DATA
  *         - TWI_OTHER_ERROR
  * 
  * @see _24XX1026_writeByte()
  */
        uint8_t _24XX1026_writePage(         const uint8_t i2cAddr, const uint8_t block_id, uint16_t writeAddr, uint8_t const* data, const uint16_t nbData);

 /**
  * Writes a byte in the memory.
  * 
  * @param [in] i2cAddr
  *     24XX1026's I2C address on the bus [0;4[.
  * @param [in] block_id
  *     Identifier of the block where performed writing [0;_24XX1026_NB_BLOC[.
  * @param [in] writeAddr
  *     Address in the block where writing [0;_24XX1026_BLOC_SIZE[.
  * @param [in] data
  *     Data to be written.
  * 
  * @return TWI_SUCCESS, if writing succeeded, else an error among
  *         - TWI_DATA_TOO_LONG
  *         - TWI_NACK_ON_ADDRESS
  *         - TWI_NACK_ON_DATA
  *         - TWI_OTHER_ERROR
  * 
  * @see _24XX1026_writePage()
  * @see _24XX1026_readRandom()
  */
inline  uint8_t _24XX1026_writeByte(         const uint8_t i2cAddr, const uint8_t block_id, uint16_t writeAddr, const uint8_t data);

 /**
  * Reads up to one block of memory.
  * 
  * @param [in] i2cAddr
  *     24XX1026's I2C address on the bus [0;4[.
  * @param [in] block_id
  *     Identifier of the block where performed reading [0;_24XX1026_NB_BLOC[.
  * @param [in] readAddr
  *     Address in the block where start reading [0;_24XX1026_BLOC_SIZE[.
  * @param [in,out] values
  *     Reference to the array where store values read.
  * @param [in] nbValue
  *     Number of values to be read  [0;_24XX1026_BLOC_SIZE[.
  * 
  * @return TWI_SUCCESS, if reading succeeded, else an error among
  *         - TWI_DATA_TOO_LONG
  *         - TWI_NACK_ON_ADDRESS
  *         - TWI_NACK_ON_DATA
  *         - TWI_OTHER_ERROR
  * 
  * @see _24XX1026_readRandom()
  * @see _24XX1026_readCurrentAddress()
  */
        uint8_t _24XX1026_readSequential(    const uint8_t i2cAddr, const uint8_t block_id, uint16_t const* readAddr, uint8_t* values, const uint16_t nbValue);

 /**
  * Reads a byte in the memory.
  * 
  * @param [in] i2cAddr
  *     24XX1026's I2C address on the bus [0;4[.
  * @param [in] block_id
  *     Identifier of the block where performed reading [0;_24XX1026_NB_BLOC[.
  * @param [in] readAddr
  *     Address in the block where start reading [0;_24XX1026_BLOC_SIZE[.
  * @param [in,out] value
  *     Reference where store value read.
  * 
  * @return TWI_SUCCESS, if reading succeeded, else an error among
  *         - TWI_DATA_TOO_LONG
  *         - TWI_NACK_ON_ADDRESS
  *         - TWI_NACK_ON_DATA
  *         - TWI_OTHER_ERROR
  * 
  * @see _24XX1026_readSequential()
  * @see _24XX1026_readCurrentAddress()
  */
inline  uint8_t _24XX1026_readRandom(        const uint8_t i2cAddr, const uint8_t block_id, uint16_t const* readAddr, uint8_t* value);

 /**
  * Reads the byte following the address of the last memory access.
  * 
  * @param [in] i2cAddr
  *     24XX1026's I2C address on the bus [0;4[.
  * @param [in] block_id
  *     Identifier of the block where performed reading [0;_24XX1026_NB_BLOC[.
  * @param [in,out] value
  *     Reference where store value read.
  * 
  * @return TWI_SUCCESS, if reading succeeded, else an error among
  *         - TWI_DATA_TOO_LONG
  *         - TWI_NACK_ON_ADDRESS
  *         - TWI_NACK_ON_DATA
  *         - TWI_OTHER_ERROR
  * 
  * @see _24XX1026_readSequential()
  * @see _24XX1026_readRandom()
  */
inline  uint8_t _24XX1026_readCurrentAddress(const uint8_t i2cAddr, const uint8_t block_id, uint8_t* value);

/**
 *	Class for high level handling of 24XX1026 memory's family.
 */
class _24XX1026Manager
{
    private:
        
        uint8_t  i2cAddr_;
        uint8_t  idBlock_;
        uint8_t  idPage_;
        uint16_t blockAddr_;
        
        /**
         *	Moves cursor to the start of the next page.
         *  @internal
         *  @return false if there isn't other page, true otherwise.
         *  
         *  @see gotoNextBlock()
         */
        boolean gotoNextPage();
        
        /**
         *	Moves cursor to the start of the next block.
         *  @internal
         *  @return false if there isn't other block, true otherwise.
         *  
         *  @see gotoNextPage()
         */
        boolean gotoNextBlock();
        
    public:
        /**
         *	_24XX1026Manager constructor.
         *  
         *  @param [in] i2cAddr
         *     24XX1026's I2C address on the bus [0;4[.
         *  @param [in] addr
         *      Address where put the cursor [0;_24XX1026_MEMORY_SIZE[.
         *      Default: 0.
         */
        _24XX1026Manager(uint8_t i2cAddr, uint32_t addr = 0);
        
        /**
         *	Changes cursor position in the memory.
         *  
         *  @param [in] addr
         *      Address where put the cursor [0;_24XX1026_MEMORY_SIZE[.
         *  
         *  @return true if setting can be performed, false otherwise.
         *  
         *  @see getCursor()
         *  @see backwardCursor()
         *  @see forwardCursor()
         */
        boolean setCursor(uint32_t addr);
        
        /**
         *	Backwards the cursor position in the memory.
         *  
         *  @param [in] shifting
         *      Number of bytes which the cursor should be moved.
         *  
         *  @return true if setting can be performed, false otherwise.
         *  
         *  @see setCursor()
         *  @see forwardCursor()
         */
        inline boolean backwardCursor(uint32_t shifting);
        
        /**
         *	Forwards the cursor position in the memory.
         *  
         *  @param [in] shifting
         *      Number of bytes which the cursor should be moved.
         *  
         *  @return true if setting can be performed, false otherwise.
         *  
         *  @see setCursor()
         *  @see backwardCursor()
         */
        inline boolean forwardCursor(uint32_t shifting);
        
        /**
         *	Retrieves cursor position in the memory.
         *  
         *  @return The address corresponding to the cursor position.
         *  
         *  @see setCursor()
         */
        inline uint32_t getCursor() const;
        
        /**
         *	Computes the remaining space of memory after the cursor position.
         *  
         *  @return The remaining space in bytes.
         */
        inline uint32_t getRemainingSpace() const;
        
        /**
         *	Writes a byte at the cursor position in the memory.
         *  
         *  @param [in] data
         *      Data to be written.
         *  
         *  @return The number of bytes written.
         */
        inline size_t write(uint8_t data);
        
        /**
         *	Writes data from the cursor position in the memory.
         *  
         *  @param [in] data
         *      Reference to the array where find data to be written.
         *  @param [in] quantity
         *      Number of data to be written.
         *  
         *  @return The number of bytes written.
         */
        size_t write(uint8_t* data, size_t quantity);
        
        /**
         *	Reads value at the cursor position in the memory.
         *  
         *  @param [in] value
         *      Reference where store values read.
         *  
         *  @return The number of bytes read.
         */
        inline size_t read(uint8_t* value);
        
        /**
         *	Reads values from the cursor position in the memory.
         *  
         *  @param [in] values
         *      Reference to the array where store value read.
         *  @param [in] quantity
         *      Number of values to be read.
         *  
         *  @return The number of bytes read.
         */
        size_t read(uint8_t* values, size_t quantity);
        
        /**
         *	Reads value at the cursor position without moving the cursor.
         *  
         *  @param [in] value
         *      Reference where store value read.
         *  
         *  @return The number of bytes read.
         */
        size_t peek(uint8_t* value);
};
 
#endif