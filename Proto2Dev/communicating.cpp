#include "communicating.h"
#include "sampling.h"
#include "ADS7828.h"

static void sleepXbee(void);
static void wakeUpXbee(void);

static void sendBuffer(uint8_t* buffer, uint16_t* length);
static void waitData(const uint32_t delay);

static void callBackFuncACK(tProtocol_ACK_data*);
static void callBackFuncMOD(tProtocol_MOD_data* sData);
static void askCallBackFuncDR1(void);
static void askCallBackFuncDC1(void);
static void askCallBackFuncSTA(void);

static inline void sendBytesFunc(unsigned char* buffer, unsigned long size);
static inline void flushFunc(void);
static inline void rcvRfData64Func(xbee_RxFrame64* rx64);

static uint8_t sleepPin;
static xbee_TxFrame64 tx64;
static bool bStopWaiting;
static bool bReply;
static uint8_t* buffer;
static uint16_t* bufferPos;
enum eProtocolMode mode;

void initCommunicating(uint8_t* buf, uint16_t* bufPos)
{
    assert(buf != NULL);
    assert(bufPos != NULL);
    buffer = buf;
    bufferPos = bufPos;
    sleepPin = COMMUNICATING_DEFAULT_SLEEP_PIN;
    tx64.frameId = 1;
    tx64.destinationAddress = XBEE_BROADCAST_64;
    tx64.options = 0x00;
    xbee_Init(NULL, NULL, NULL, &rcvRfData64Func, NULL, NULL, &sendBytesFunc, &flushFunc);
    protocol_STA_setAskCallBackFunc(&askCallBackFuncSTA);
    protocol_DC1_setAskCallBackFunc(&askCallBackFuncDC1);
    protocol_DR1_setAskCallBackFunc(&askCallBackFuncDR1);
    protocol_ACK_setCallBackFunc(&callBackFuncACK);
    protocol_MOD_setCallBackFunc(&callBackFuncMOD);
    
    
}

static inline void rcvRfData64Func(xbee_RxFrame64* rx64)
{
    protocol_parseFrameCluster(rx64->data, rx64->dataSize);
}

static inline void flushFunc(void)
{
    Serial.flush();
}

static inline void sendBytesFunc(unsigned char* buffer, unsigned long size)
{
    Serial.write(buffer, size);
}

void setDestinationAddress(long long destinationAddress)
{
    tx64.destinationAddress = destinationAddress;
}

void setSleepPin(uint8_t pin)
{
    assert(pin < 20);
    sleepPin = pin;
}

static void sleepXbee(void)
{
    pinMode(sleepPin, OUTPUT);
    digitalWrite(sleepPin, HIGH);
}

static void wakeUpXbee(void)
{
    digitalWrite(sleepPin, LOW);
    /* Reduce th power consumption. */
    pinMode(sleepPin, INPUT);
}


void communicating(uint8_t* buffer, uint16_t* length)
{
    wakeUpXbee();
    do
    {
        bReply = false;
        sendBuffer(buffer, length);
        waitData(1000);
    }
    while (bReply);
    
    sleepXbee();
}

static void sendBuffer(uint8_t* buffer, uint16_t* length)
{
    tx64.data = buffer;
    tx64.dataSize = XBEE_MAX_PACKET_SIZE;
    
    while (*length > XBEE_MAX_PACKET_SIZE)
    {
        xbee_SendTxFrame64(&tx64);
        tx64.data += XBEE_MAX_PACKET_SIZE;
        *length -= XBEE_MAX_PACKET_SIZE;
    }
    tx64.dataSize = *length;
    xbee_SendTxFrame64(&tx64);
    *length = 0;
    
}

static void waitData(const uint32_t delay)
{
    char c;
    uint32_t timeout = millis() + delay;
    
    Serial.setTimeout(delay);
    bStopWaiting = false;
    
    while (!bStopWaiting && !bReply && millis() < timeout)
    {
        Serial.readBytes(&c, 1);
        xbee_ReceiveDataByte(c);
        Serial.setTimeout(timeout - millis());
    }
}

static void callBackFuncACK(tProtocol_ACK_data* sData)
{
    assert(sData == NULL);
    bStopWaiting = true;
}

static void callBackFuncMOD(tProtocol_MOD_data* sData)
{
    mode = *sData;
    *bufferPos += protocol_ACK_create(buffer + *bufferPos, NULL);
    bReply = true;
}

static void askCallBackFuncDR1(void)
{
    struct sProtocolDR1 sData;
    
    getFSRSensor(sData.fsrValues);
    sData.time = millis();
    *bufferPos += protocol_DR1_create(buffer + *bufferPos, &sData);
    bReply = true;
}

static void askCallBackFuncDC1(void)
{
    struct sProtocolDC1 sData;
    
    getFSCSensor(sData.fscValues);
    sData.time = millis();
    *bufferPos += protocol_DC1_create(buffer + *bufferPos, &sData);
    bReply = true;
}

static void askCallBackFuncSTA(void)
{
    struct sProtocolSTA sData;
    
    sData.batLvl = getBatteryLvl();
    sData.nbFsr = PROTOCOL_FSR_NUMBER;
    sData.nbFsc = PROTOCOL_FSC_NUMBER;
    sData.syncTime = millis();
    *bufferPos += protocol_STA_create(buffer + *bufferPos, &sData);
    bReply = true;
}

