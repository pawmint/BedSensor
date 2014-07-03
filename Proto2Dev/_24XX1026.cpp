/**
 *  @copybrief _24XX1026.h
 *  @copydetails _24XX1026.h
 *
 *  @file _24XX1026.cpp
 *  @date 30 jun 2014
 *  @copyright PAWM International
 *  
 *  @author Mickael Germain
 *  
 */
 
#include "_24XX1026.h"

uint8_t _24XX1026_writePage(const uint8_t i2cAddr, const uint8_t block_id, uint16_t writeAddr, uint8_t const* data, const uint16_t nbData)
{
    uint16_t i;
    
    assert(i2cAddr < 4);
    assert(block_id < _24XX1026_NB_BLOC);
    assert(data != NULL);
    assert(nbData <= _24XX1026_PAGE_SIZE); /* Always true thanks to uint8_t. */
    /* Write Byte :
     * MSB|6|5|4| 3| 2|1|LSB
     *   1|0|1|0|A1|A0|B|R/W
     * 
     * Omit the LSB
     */
    Wire.beginTransmission((_24XX1026_HARDWARE_ADDRESS << 3) | (i2cAddr << 1) | block_id);
    
    /*  Address Bytes :
     *   - Address High Byte
     *   - Address Low Byte
     */
    Wire.write((uint8_t)(writeAddr >> 8));
    Wire.write((uint8_t) writeAddr);
    
    /*  Data Bytes :
     *   - Data 0
     *   - Data ...
     *   - Data nbData - 1
     */
    for (i = 0 ; i < nbData ; i++)
    {
        Wire.write(data[i]);
    }
    
    return Wire.endTransmission(true);		
}

inline uint8_t _24XX1026_writeByte(const uint8_t i2cAddr, const uint8_t block_id, uint16_t writeAddr, const uint8_t data)
{
    return _24XX1026_writePage(i2cAddr, block_id, writeAddr, &data, 1);
}

uint8_t _24XX1026_readSequential(const uint8_t i2cAddr, const uint8_t block_id, uint16_t const* readAddr, uint8_t* values, const uint16_t nbValue)
{
    uint8_t ret;
    int addr;
    uint16_t remainingValues;
    uint16_t readValues;
    uint16_t i;
    
    assert(i2cAddr < 4);
    assert(block_id < _24XX1026_NB_BLOC);
    assert(values != NULL);
    
    addr = (int)(_24XX1026_HARDWARE_ADDRESS << 3) | (i2cAddr << 1) | block_id;
    
    if (readAddr != NULL)
    {
        /* Write Byte :
         * MSB|6|5|4| 3| 2|1|LSB
         *   1|0|1|0|A1|A0|B|R/W
         * 
         * Omit the LSB
         */
        Wire.beginTransmission(addr);
    
        /*  Address Bytes :
         *   - Address High Byte
         *   - Address Low Byte
         */
         Wire.write((uint8_t)((*readAddr) >> 8));
         Wire.write((uint8_t) (*readAddr));
     
         ret = Wire.endTransmission(false);
         if (ret != TWI_SUCCESS)
            return ret;
    }
     
    /* Read Byte :
     * MSB|6|5|4| 3| 2|1|LSB
     *   1|0|1|0|A1|A0|B|R/W
     * 
     * Omit the LSB
     */
    remainingValues = nbValue;
    readValues = 0;
    // Basically, you can read the entire block but because of the TWI buffer size limitation, the reads are performed with A buffer.
    // So it's not a full sequential read but it's transparent for user and looks like in the datasheet.
    
    // Only one while could be used but need more computation. */
    while (remainingValues > TWI_BUFFER_LENGTH)
    {
        if (Wire.requestFrom(addr, (int)TWI_BUFFER_LENGTH, false) == TWI_BUFFER_LENGTH)
        {
            for (i = 0 ; i < TWI_BUFFER_LENGTH ; i++)
            {
               values[readValues++] = Wire.read();
            }
        }
        else
            return TWI_OTHER_ERROR;
        
        remainingValues -= TWI_BUFFER_LENGTH;
    }
    // Read the last incomplete buffer.
    if (Wire.requestFrom(addr, (int)remainingValues, true) == remainingValues)
    {
        for (i = 0 ; i < remainingValues ; i++)
        {
            values[readValues++] = Wire.read();
        }
    }
    else
        ret = TWI_OTHER_ERROR;
  
    return ret;
}

inline uint8_t _24XX1026_readCurrentAddress(const uint8_t i2cAddr, const uint8_t block_id, uint8_t* value)
{
    return _24XX1026_readSequential(i2cAddr, block_id, NULL, value, 1);
}

inline uint8_t _24XX1026_readRandom(const uint8_t i2cAddr, const uint8_t block_id, uint16_t const* readAddr, uint8_t* value)
{
    return _24XX1026_readSequential(i2cAddr, block_id, readAddr, value, 1);
}

_24XX1026Manager::_24XX1026Manager(uint8_t i2cAddr, uint32_t addr)
{
    assert(i2cAddr < 4);
    _24XX1026_init();
    i2cAddr_ = i2cAddr;
    setCursor(addr);
}

boolean _24XX1026Manager::gotoNextBlock()
{
    boolean bRet = false;
    if (idBlock_ < _24XX1026_NB_BLOC - 1)
    {
        // We aren't in the last block.
        idBlock_ ++;
        idPage_ = 0;
        blockAddr_ = 0;
        bRet = true;
    }
    
    return bRet;
}

boolean _24XX1026Manager::gotoNextPage()
{
    boolean bRet = true;
    
    if (blockAddr_ + _24XX1026_PAGE_SIZE > _24XX1026_BLOC_SIZE)
    {
        // We are in the last page of the block.
        bRet = gotoNextBlock();
    }
    else
    {
        idPage_ ++;
        blockAddr_ = idPage_ * _24XX1026_PAGE_SIZE;
    }
    
    return bRet;
}

boolean _24XX1026Manager::setCursor(uint32_t addr)
{
    boolean bRet = false;
    if (addr < _24XX1026_MEMORY_SIZE)
    {
        idBlock_ = addr / _24XX1026_BLOC_SIZE;
        blockAddr_ = addr - idBlock_ * _24XX1026_BLOC_SIZE;
        idPage_ = blockAddr_ / _24XX1026_PAGE_SIZE;
        bRet = true;
    }
    
    return bRet;
}

inline uint32_t _24XX1026Manager::getCursor() const
{
    return blockAddr_ + idBlock_ * _24XX1026_BLOC_SIZE;
}

inline uint32_t _24XX1026Manager::getRemainingSpace() const
{
    return _24XX1026_MEMORY_SIZE - getCursor();
}

inline boolean _24XX1026Manager::backwardCursor(uint32_t shifting)
{
    return ((getCursor() >= shifting) && setCursor(getCursor() - shifting));
}

inline boolean _24XX1026Manager::forwardCursor(uint32_t shifting)
{
    return setCursor(getCursor() + shifting);
}


inline size_t _24XX1026Manager::write(uint8_t data)
{
    return write(&data, 1);
}

size_t _24XX1026Manager::write(uint8_t* data, size_t quantity)
{
    size_t nextWrite;
    size_t remaining = quantity;
    if (remaining > getRemainingSpace())
        // Not enough space.
        return 0;
        
    while (blockAddr_ + remaining > (idPage_ + 1) * _24XX1026_PAGE_SIZE)
    {
        // The writing cross multiple page.
        nextWrite = (idPage_ + 1) * _24XX1026_PAGE_SIZE - blockAddr_;
        if (_24XX1026_writePage(i2cAddr_, idBlock_, blockAddr_, data + quantity - remaining, nextWrite) != TWI_SUCCESS)
            return quantity - remaining;
        gotoNextPage();
        remaining -= nextWrite;
    }
    // The writing are located in one page.
    if (_24XX1026_writePage(i2cAddr_, idBlock_, blockAddr_, data + quantity - remaining, remaining) != TWI_SUCCESS)
        return quantity - remaining;
    forwardCursor(remaining);
        
    return quantity;
}

inline size_t _24XX1026Manager::read(uint8_t* value)
{
    return read(value, 1);
}

size_t _24XX1026Manager::read(uint8_t* values, size_t quantity)
{
    size_t nextWrite;
    size_t remaining = quantity;
    if (remaining > getRemainingSpace())
        // Not enough space.
        return 0;
    if (blockAddr_ + remaining > _24XX1026_BLOC_SIZE)
    {
        // The reading cross two block.
        if (_24XX1026_readSequential(i2cAddr_, idBlock_, &blockAddr_, values, _24XX1026_BLOC_SIZE - blockAddr_) != TWI_SUCCESS)
            return 0;
        remaining -= _24XX1026_BLOC_SIZE - blockAddr_;
        gotoNextBlock();
    }
    // The reading are located in one block.
    if (_24XX1026_readSequential(i2cAddr_, idBlock_, &blockAddr_, values + quantity - remaining, remaining) != TWI_SUCCESS)
        return quantity - remaining;
    forwardCursor(remaining);
    
    return quantity;
}

size_t _24XX1026Manager::peek(uint8_t* value)
{
    size_t ret = read(value, 1);
    backwardCursor(1);
    
    return ret;
}

