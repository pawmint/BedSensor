/*
  Copyright (C) 2008. LIS Laboratory, EPFL, Lausanne

  This file is part of Aeropic.

  Aeropic is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  Aeropic is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Aeropic.  If not, see <http://www.gnu.org/licenses/>.
*/ 
/*!
*	\file xbee_api.h
*	\brief Header file of the XBee API library
*
*	\author Alexandre Habersaat
*	\date 2008-08-07
*
*	This file contains the public interface of the library
*
*/

#ifndef __XBEE_API_H__
#define __XBEE_API_H__

#ifdef AEROPIC_PREFIX
	#include AEROPIC_PREFIX
#endif

#ifdef __cplusplus
	extern "C" {
#endif


//-------------------------
// Public type definitions
//-------------------------

//! The structure for the reception of packets with 64-bits address
typedef struct
{
	long long sourceAddress; //!< the 64-bits source address
	unsigned char RSSI; //!< the signal strength when the last packet was received
	unsigned char options; //!< the options of this packet (address broadcast, PAN broadcast)
	unsigned char * data; //!< the buffer that contains the data
	unsigned char dataSize; //!< the number of readable data bytes in the buffer
}
xbee_RxFrame64;


//! The structure for the reception of packets with 16-bits address
typedef struct
{
	short int sourceAddress; //!< the 16-bits source address
	unsigned char RSSI; //!< the signal strength when the last packet was received
	unsigned char options; //!< the options of this packet (address broadcast, PAN broadcast)
	unsigned char * data; //!< the buffer that contains the data
	unsigned char dataSize; //!< the number of readable data bytes in the buffer
}
xbee_RxFrame16;


//! The structure to send a packet with a 64-bits address
typedef struct
{
	unsigned char frameId; //!< the id of the packet to send
	long long destinationAddress; //!< the 64-bits address of the destination
	unsigned char options; //!< the options of this packet (disable ACK, PAN broadcast)
	unsigned char * data; //!< the address containing the data to send
	unsigned char dataSize; //!< the number of bytes to send from the pointer
}
xbee_TxFrame64;


//! The structure to send a packet with a 64-bits address
typedef struct
{
	unsigned char frameId; //!< the id of the packet to send
	short int destinationAddress; //!< the 16-bits address of the destination
	unsigned char options; //!< the options of this packet (disable ACK, PAN broadcast)
	unsigned char * data; //!< the address containing the data to send
	unsigned char dataSize; //!< the number of bytes to send from the pointer
}
xbee_TxFrame16;


//! The structure for the status of a transmission
typedef struct
{
	unsigned char frameId; //!< the id of the packet that is reported
	unsigned char status; //!< the status of the transmission
}
xbee_TxStatus;


//! The structure for the status sent by the modem
typedef struct
{
	unsigned char status; //!< the status of the modem (hardware reset, watchdog timer reset, associated, disassociated, sync lost, coordinator realignement, coordinator started)
}
xbee_ModemStatus;

//! The structure to execute an AT command
typedef struct
{
	unsigned char frameId; //!< the id of the packet, 0 means no response
	char command[2]; //!< the 2-character AT command
	unsigned char * parameter; //!< the pointer to the parameter value of the AT command, 0 means "read"
	unsigned char parameterSize; //!< the size of the parameter to set
}
xbee_AtCommand;

//! The structure for the answer to an AT command request
typedef struct
{
	unsigned char frameId; //!< the id of the request
	char command[2]; //!< the command that was passed
	unsigned char status; //!< the status of the command, 0 = OK, 1 = ERROR
	unsigned char * response; //!< the value of the queried register
	unsigned char responseSize; //!< the size of the response
}
xbee_AtCommandResponse;

//! Type definition for call-back function when a modem status message is received
typedef void (*xbee_RcvModemStatusFunc)(xbee_ModemStatus *);
//! Type definition for call-back function when a AT command response is received
typedef void (*xbee_RcvAtCommandResponseFunc)(xbee_AtCommandResponse *);
//! Type definition for call-back function when a transmission status message is received
typedef void (*xbee_RcvTxStatusFunc)(xbee_TxStatus *);
//! Type definition for call-back function when a RF packet with 64-bits address is received
typedef void (*xbee_RcvRfData64Func)(xbee_RxFrame64 *);
//! Type definition for call-back function when a RF packet with 16-bits address is received
typedef void (*xbee_RcvRfData16Func)(xbee_RxFrame16 *);
//! Type definition for call-back function when a RF byte is received
typedef void (*xbee_RcvRfDataByteFunc)(unsigned char);
//! Type definition for function used to send bytes through communication
typedef void (*xbee_SendBytesFunc)(unsigned char *, unsigned long);
//! Type definition for function used to flush external output communication buffer
typedef void (*xbee_FlushFunc)();


//----------------------------
// Public function prototypes
//----------------------------

// High level functions
//! This function must be called from outside each time a byte is receivesd from the communication
void xbee_ReceiveDataByte(unsigned char data);

//! This function inits the different pointers to call-back functions to call when packets are received and to functions to call to send data through communication
void xbee_Init(xbee_RcvModemStatusFunc rcvModemStatusFunc, xbee_RcvAtCommandResponseFunc rcvAtCommandResponseFunc, xbee_RcvTxStatusFunc rcvTxStatusFunc, xbee_RcvRfData64Func rcvRfData64Func, xbee_RcvRfData16Func rcvRfData16Func, xbee_RcvRfDataByteFunc rcvRfDataByteFunc, xbee_SendBytesFunc sendBytesFunc, xbee_FlushFunc flushFunc);

//! This function receives bytes to send, adds them to a 16-bits packet and sends it to communication when it is full or when flush_packet is called
void xbee_PacketizeData(unsigned char * data, unsigned long length);

//! This function sends the packet with its current content to the communication
void xbee_FlushPacket();

//! This function sets the packet destination and options
void xbee_SetDestinationAndOptions(unsigned short destination, unsigned char options);

//! This function modifies the send function of the XBee API
void xbee_SetSendFunction(xbee_SendBytesFunc sendBytesFunc);

//! This function modifies the flush function of the XBee API
void xbee_SetFlushFunction(xbee_FlushFunc flushFunc);

//  Mid level functions
//! This function sends a data packet to the module for RF transmission, using 64-bits address
void xbee_SendTxFrame64(xbee_TxFrame64 * packet);

//! This function sends a data packet to the module for RF transmission, using 16-bits address
void xbee_SendTxFrame16(xbee_TxFrame16 * packet);

//! This function sends an AT command to the module
void xbee_SendAtCommand(xbee_AtCommand * packet);


#ifdef __cplusplus
	}
#endif


#endif
