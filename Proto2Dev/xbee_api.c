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
*	\file xbee_api.c
*	\brief Main file of the XBee API library
*
*	\author Alexandre Habersaat
*	\date 2008-08-07
*
*	This file contains all the internal variables and functions of the library.
*
*	XBee API library
*
*	This XBee API library has been designed to access functions of the XBee/XBeePRO modules when configurated to run in API mode.
*
*	\section intro Introduction
*	Before using the library, the modules must be configurated to run in API mode (AP = 1, no escape characters). The configuration can be done using X-CTU
*	or directly using AT commands. Refer to XBee module manual for more information.
*
*	The library aims at abstracting the API protocol to the user by providing the required function to both configure and transmit data through the modules.
*
*	The features provided by the API mode are the following:
*	\li Read the source of incoming RF packets
*	\li Set the destination address of outgoing RF packets
*	\li Read the RSSI (signal strenght indicator) or incoming RF packets
*	\li Read the status of outgoing RF packets transmission (ACK/error)
*	\li Read/Set a parameter of the card using AT commands
*
*	If these features are not required, then it is much simpler to configure to module to work in transparent mode (AP = 0).
*
*	\section using_lib Using the library
*	The library must be linked to a communication device/library both for incoming and outgoing communication.
*	\subsection incoming_lib Incoming communication
*	When bytes are received on the communication, the user must give them to the library by calling xbee_ReceiveDataByte.
*	Then, the library decodes the XBee frames and calls a different user-defined function for each type of packet. These user-defined functions must be set
*	by calling the xbee_Init() function before doing anything with the library. If the feature is not needed, the function can be empty.
*	The functions that needs to be defined for the incoming packets are the following :
*	\li #xbee_rcvModemStatusFunc, when the status of the module is modified (associated, disassociated, reset,...).
*	\li #xbee_rcvAtCommandResponseFunc, when an AT command is sent to the module and the module gives a response.
*	\li #xbee_rcvTxStatusFunc, when a RF packet is sent and the module informs if the packet has been transmitted successfully or not.
*	\li #xbee_rcvRfData64Func, when a RF packet with a 64-bits address is received. This function permits to access RSSI and source address information.
*	\li #xbee_rcvRfData16Func, when a RF packet with a 16-bits address is received. This function permits to access RSSI and source address information.
*
*	The two last functions are call when a incoming RF packet is received. In the case where the user wants the received bytes to be immediately forwarded, he can define
*	the function #xbee_rcvRfDataByteFunc which is called each time an RF byte is received. However, the bytes transmitted by this function are not validated and may potentially
*	be corrupted. Refer to the structure definitions in xbee_api.h and to the XBee module manual for more information about these functionalities.
*
*	\subsection outgoing_lib Outgoing communication
*	To be able to send packets, two other functions must be defined :
*	\li #xbee_sendBytesFunc, when the library wants to transmit bytes to the communication (with or without buffer).
*	\li #xbee_flushFunc, when a packet is finished, in order to flush the communication buffer if there is one (e.g. DMA). If not needed, this function may be empty.
*
*	To transmit data, the user can either simply give the bytes he wants to send, or manually creates a packet and send it.
*
*	High level functions are the following :
*	\li xbee_PacketizeData(), it appends the data to the current packet or creates a new one (if no one exists). When the packet is full, it is directly flushed and a new
*	packet is created.
*	\li xbee_FlushPacket(), it sends the current packet, even if it is not full, to the current destination and with the current options (ACK, broadcast)
*	\li xbee_SetDestinationAndOptions(), it modifies the current destination address and the current options of the packets. The modifications apply for all the next
*	packets that are sent.
*
*	Lower level function are the following :
*	\li xbee_SendTxFrame64(), to send a packet with a 64-bits address to the communication. The packet has to be created manually by the user using the corresponding structure.
*	Dont forget that the maximum size for the data is 100 bytes.
*	\li xbee_SendTxFrame16(), to send a packet with a 16-bits address to the communication. The packet has to be created manually by the user using the corresponding structure.
*	Dont forget that the maximum size for the data is 100 bytes.
*	\li xbee_SendAtCommand(), to send AT commands to module in order to read or write parameters.
*
*/


//----------
// Includes
//----------
#include "xbee_api.h"
#include "xbee_api_def.h"


//-------------------
// Private variables
//-------------------

//! The reception buffer of the library
unsigned char xbee_rcvBuffer[XBEE_MAX_PACKET_SIZE];
//! The emission buffer of the library
unsigned char xbee_sndBuffer[XBEE_MAX_PACKET_SIZE];

// Reception variables
//! The current state of the receiving state machine
unsigned char xbee_state = XBEE_STATE_DELIMITER;
//! The type of the current packet
unsigned char xbee_rcvPacketType;
//! While reading the size of a packet, the current byte that is being read
unsigned char xbee_rcvPacketSizeByte = 0;
//! The size of the packet that is being read
unsigned short int xbee_rcvPacketSize = 0;
//! The current byte received in the packet
unsigned short int xbee_rcvPacketCurrentByte = 0;
//! The checksum of the incoming message
unsigned char xbee_rcvPacketChecksum = 0;

// Reception structure
//! The instance of the modem status structure
xbee_ModemStatus xbee_modemStatus;
//! The instance of the AT command response structure
xbee_AtCommandResponse xbee_atCommandResponse;
//! The instance of the transmission status structure
xbee_TxStatus xbee_txStatus;
//! The instance of the structure for incoming data with 64-bits address
xbee_RxFrame64 xbee_rxFrame64;
//! The instance of the structure for incoming data with 16-bits address
xbee_RxFrame16 xbee_rxFrame16;

// Send variables
//! The current size of the send packet
unsigned short int xbee_sndPacketSize = 0;
//! The frame id for the current packet. Warning: a frame id of 0 disables TX status response
unsigned char xbee_sndPacketFrameId = 1;

// Send structure
//! The instance of the structure for outgoing data (16-bits address)
xbee_TxFrame16 xbee_txFrame16;

// Pointer to external user-defined functions used for call-back
//! The pointer to the function to call when a modem status packet is received
xbee_RcvModemStatusFunc xbee_rcvModemStatusFunc;
//! The pointer to the function to call when an AT command response packet is received
xbee_RcvAtCommandResponseFunc xbee_rcvAtCommandResponseFunc;
//! The pointer to the function to call when a TX status packet is received
xbee_RcvTxStatusFunc xbee_rcvTxStatusFunc;
//! The pointer to the function to call when an incoming packet with 64-bits address is received
xbee_RcvRfData64Func xbee_rcvRfData64Func;
//! The pointer to the function to call when an incoming packet with 16-bits address is received
xbee_RcvRfData16Func xbee_rcvRfData16Func;
//! The pointer to the function to call when an incoming data byte is received. Warning: This function does not validate the data (no checksum)
xbee_RcvRfDataByteFunc xbee_rcvRfDataByteFunc;

//! The pointer to the function to call to send data
xbee_SendBytesFunc xbee_sendBytesFunc;
//! The pointer to the function to call to flush the data to send
xbee_FlushFunc xbee_flushFunc;

// Statistics
//! The number of bytes to create packets
unsigned long xbee_numberOfBytesPacketized = 0;
//! The number of bytes send to external function
unsigned long xbee_numberOfBytesSent = 0;


//---------------------------------
// Private functions prototypes
//---------------------------------

//! This function sends bytes
void xbee_SendBytes(unsigned char *, unsigned short);


//---------------------------------
// Private functions implementation
//---------------------------------

/*!
*	This function sends bytes using the external defined function
*/
void xbee_SendBytes(unsigned char * data, unsigned short length)
{
	xbee_numberOfBytesSent += length;
	
	unsigned long len = 0;
	len += length;
	
	if (xbee_sendBytesFunc)
		xbee_sendBytesFunc(data, len);
}

	
//---------------------------------
// Public functions implementation
//---------------------------------

/*!
*	This function gets in input each byte that is received by the communication
*	The function runs a state machine which analyzes the different packets that may be received
*	and store them in a buffer.
*
*	@param c The character received through communication
*/
void xbee_ReceiveDataByte(unsigned char c)
{
	char * p;

	// Wait for the state of a message (do nothing as long as c is the start delimiter)
	if (xbee_state == XBEE_STATE_DELIMITER)
	{
		// printf("\nState Delimiter");
		if (c == XBEE_DELIMITER)
		{
			// Set up next state
			xbee_rcvPacketSize = 0;
			xbee_rcvPacketSizeByte = 0;
			// Go to next state
			xbee_state = XBEE_STATE_LENGTH;
		}
	}
	// Read the length of the message
	else if (xbee_state == XBEE_STATE_LENGTH)
	{
		// printf("\nState Length");
		// Assign byte c to size
		// The first byte receive is the most significant one, the second the least significant one
		p = (char*)&xbee_rcvPacketSize;
		p[1 - xbee_rcvPacketSizeByte++] = c;

		// If size is complete, go to next state
		if (xbee_rcvPacketSizeByte == XBEE_NUMBER_OF_SIZE_BYTES)
		{
			// Set up next state
			xbee_rcvPacketCurrentByte = 0;
			xbee_rcvPacketChecksum = 0;
			// Go to next state
			xbee_state = XBEE_STATE_API_IDENTIFIER;
		}
	}
	// Read the identifier of the message and go to the dedicated state
	else if (xbee_state == XBEE_STATE_API_IDENTIFIER)
	{
		// printf("\nState API Identifier");
		if (c == XBEE_IN_MODEM_STATUS)
		{
			xbee_state = XBEE_STATE_MODEM_STATUS;
		}
		else if (c == XBEE_IN_TX_STATUS)
		{
			xbee_state = XBEE_STATE_TX_STATUS;
		}
		else if (c == XBEE_IN_AT_COMMAND_RESPONSE)
		{
			xbee_atCommandResponse.response = 0;
			xbee_atCommandResponse.responseSize = 0;
			xbee_state = XBEE_STATE_AT_RESPONSE;
		}
		else if (c == XBEE_IN_RX_64)
		{
			xbee_rxFrame64.sourceAddress = 0;
			xbee_rxFrame64.data = 0;
			xbee_rxFrame64.dataSize = 0;
			xbee_state = XBEE_STATE_RX64;
		}
		else if (c == XBEE_IN_RX_16)
		{
			xbee_rxFrame16.sourceAddress = 0;
			xbee_rxFrame16.data = 0;
			xbee_rxFrame16.dataSize = 0;
			xbee_state = XBEE_STATE_RX16;
		}
		else
		{
			// Error
			xbee_state = XBEE_STATE_DELIMITER;
			// printf("\nERR: API identifier not recognized !");
			if (c == XBEE_DELIMITER)
			{
				// Set up next state
				xbee_rcvPacketSize = 0;
				xbee_rcvPacketSizeByte = 0;
				// Go to next state
				xbee_state = XBEE_STATE_LENGTH;
			}
		}

		// Save packet type
		xbee_rcvPacketType = c;
		// Checksum
		xbee_rcvPacketChecksum += c;

		// Start counting the bytes received now
		++xbee_rcvPacketCurrentByte;
	}


	// Read the content of the modem status message
	else if (xbee_state == XBEE_STATE_MODEM_STATUS)
	{
		// printf("\nState Modem status");
		// Byte containing status
		if (xbee_rcvPacketCurrentByte == 1)
			xbee_modemStatus.status = c;

		// Checksum
		xbee_rcvPacketChecksum += c;
		// Increment number of bytes received
		++xbee_rcvPacketCurrentByte;

		if (xbee_rcvPacketCurrentByte == xbee_rcvPacketSize)
			xbee_state = XBEE_STATE_CHECKSUM;
	}


	// Read the content of the AT response message
	else if (xbee_state == XBEE_STATE_AT_RESPONSE)
	{
		// printf("\nState AT response");
		// frame id
		if (xbee_rcvPacketCurrentByte == 1)
		{
			xbee_atCommandResponse.frameId = c;
		}
		// first character of AT command
		else if (xbee_rcvPacketCurrentByte == 2)
		{
			xbee_atCommandResponse.command[0] = c;
		}
		// second character of AT command
		else if (xbee_rcvPacketCurrentByte == 3)
		{
			xbee_atCommandResponse.command[1] = c;
		}
		// status of the command
		else if (xbee_rcvPacketCurrentByte == 4)
		{
			xbee_atCommandResponse.status = c;
		}
		// first byte of the parameter value
		else if (xbee_rcvPacketCurrentByte == 5)
		{
			xbee_atCommandResponse.response = xbee_rcvBuffer;
			xbee_rcvBuffer[xbee_atCommandResponse.responseSize++] = c;
		}
		// following bytes of the parameter value
		else
		{
			xbee_rcvBuffer[xbee_atCommandResponse.responseSize++] = c;
		}

		// Checksum
		xbee_rcvPacketChecksum += c;
		// Increment number of bytes received
		++xbee_rcvPacketCurrentByte;

		if (xbee_rcvPacketCurrentByte == xbee_rcvPacketSize)
			xbee_state = XBEE_STATE_CHECKSUM;
	}

	// Read the content of the TX status message
	else if (xbee_state == XBEE_STATE_TX_STATUS)
	{
		// printf("\nState TX status");
		if (xbee_rcvPacketCurrentByte == 1)
		{
			xbee_txStatus.frameId = c;
		}
		else if (xbee_rcvPacketCurrentByte == 2)
		{
			xbee_txStatus.status = c;
		}

		// Checksum
		xbee_rcvPacketChecksum += c;
		// Increment number of bytes received
		++xbee_rcvPacketCurrentByte;


		if (xbee_rcvPacketCurrentByte == xbee_rcvPacketSize)
			xbee_state = XBEE_STATE_CHECKSUM;
	}


	// Read the content of the incoming RF message with a 64-bits address
	else if (xbee_state == XBEE_STATE_RX64)
	{
		// printf("\nState RX64");
		if (xbee_rcvPacketCurrentByte >= 1 && xbee_rcvPacketCurrentByte <= 8)
		{
			p = (char*)&(xbee_rxFrame64.sourceAddress);
			p[7 - (xbee_rcvPacketCurrentByte - 1)] = c;
		}
		else if (xbee_rcvPacketCurrentByte == 9)
		{
			xbee_rxFrame64.RSSI = c;
		}
		else if (xbee_rcvPacketCurrentByte == 10)
		{
			xbee_rxFrame64.options = c;
		}
		else if (xbee_rcvPacketCurrentByte == 11)
		{
			xbee_rxFrame64.data = xbee_rcvBuffer;
		}

		if (xbee_rcvPacketCurrentByte >= 11)
		{
			xbee_rcvBuffer[xbee_rxFrame64.dataSize++] = c;

			// Give this byte further
			if (xbee_rcvRfDataByteFunc)
				xbee_rcvRfDataByteFunc(c);
		}

		// Checksum
		xbee_rcvPacketChecksum += c;
		// Increment number of bytes received
		++xbee_rcvPacketCurrentByte;


		if (xbee_rcvPacketCurrentByte == xbee_rcvPacketSize)
			xbee_state = XBEE_STATE_CHECKSUM;
	}


	// Read the content of the incoming RF message with a 16-bits address
	else if (xbee_state == XBEE_STATE_RX16)
	{
		// printf("\nState RX16");
		// printf("\n...byte %u",xbee_rcvPacketCurrentByte);

		if (xbee_rcvPacketCurrentByte >= 1 && xbee_rcvPacketCurrentByte <= 2)
		{
			p = (char*)&(xbee_rxFrame16.sourceAddress);
			p[1 - (xbee_rcvPacketCurrentByte - 1)] = c;
		}
		else if (xbee_rcvPacketCurrentByte == 3)
		{
			xbee_rxFrame16.RSSI = c;
		}
		else if (xbee_rcvPacketCurrentByte == 4)
		{
			xbee_rxFrame16.options = c;
		}
		else if (xbee_rcvPacketCurrentByte == 5)
		{
			xbee_rxFrame16.data = xbee_rcvBuffer;
		}

		if (xbee_rcvPacketCurrentByte >= 5)
		{
			xbee_rcvBuffer[xbee_rxFrame16.dataSize++] = c;

			// Give this byte further
			if (xbee_rcvRfDataByteFunc)
				xbee_rcvRfDataByteFunc(c);
		}

		// Checksum
		xbee_rcvPacketChecksum += c;
		// Increment number of bytes received
		++xbee_rcvPacketCurrentByte;

		if (xbee_rcvPacketCurrentByte == xbee_rcvPacketSize)
			xbee_state = XBEE_STATE_CHECKSUM;
	}


	// Read the checksum of the message
	else if (xbee_state == XBEE_STATE_CHECKSUM)
	{
		// printf("\nState checksum");
		// Checksum
		xbee_rcvPacketChecksum += c;

		// printf("\nchecksum %X %X",xbee_rcvPacketChecksum, c);


		// Message is valid
		if (xbee_rcvPacketChecksum == 0xFF)
		{
			// printf("\nPacket valid, type %X", xbee_rcvPacketType);

			if (xbee_rcvPacketType == XBEE_IN_MODEM_STATUS)
			{
				if (xbee_rcvModemStatusFunc)
					xbee_rcvModemStatusFunc(&xbee_modemStatus);
			}
			else if (xbee_rcvPacketType == XBEE_IN_TX_STATUS)
			{
				if (xbee_rcvTxStatusFunc)
					xbee_rcvTxStatusFunc(&xbee_txStatus);
			}
			else if (xbee_rcvPacketType == XBEE_IN_AT_COMMAND_RESPONSE)
			{
				if (xbee_rcvAtCommandResponseFunc)
					xbee_rcvAtCommandResponseFunc(&xbee_atCommandResponse);
			}
			else if (xbee_rcvPacketType == XBEE_IN_RX_64)
			{
				if (xbee_rcvRfData64Func)
					xbee_rcvRfData64Func(&xbee_rxFrame64);
			}
			else if (xbee_rcvPacketType == XBEE_IN_RX_16)
			{
				if (xbee_rcvRfData16Func)
					xbee_rcvRfData16Func(&xbee_rxFrame16);
			}

			xbee_state = XBEE_STATE_DELIMITER;
		}
		// Message is not valid
		else
		{
			// printf("\nERR: Packet NOT valid");
			// Do nothing, drop it !
			if (c == XBEE_DELIMITER)
			{
				// Set up next state
				xbee_rcvPacketSize = 0;
				xbee_rcvPacketSizeByte = 0;
				// Go to next state
				xbee_state = XBEE_STATE_LENGTH;
			}
			else
			{
				xbee_state = XBEE_STATE_DELIMITER;
			}
		}
	}
	else
	{
		// Error, invalid state, should never happen
		xbee_state = XBEE_STATE_DELIMITER;
	}
	// flush(stdout);
}


/*!
*	This function is called to transmit data using a 64-bits address
*
*	@param packet A pointer to a structure containing the packet to send
*/
void xbee_SendTxFrame64(xbee_TxFrame64 * packet)
{
    // Add by Mickael Germain LIRMM 09/07/2014 : change from 5 to 11 (problem with address size between 16 and 64 bits). 
	// unsigned short int size = 5 + packet->dataSize;
    unsigned short int size = 11 + packet->dataSize;
	unsigned char delimiter = XBEE_DELIMITER;
	unsigned char identifier = XBEE_OUT_TX_64;
	unsigned char checksum = 0;
	unsigned short int i = 0;

	checksum += identifier;
	checksum += packet->frameId;
	for (i = 0; i < 8; ++i)
	{
		checksum += *(((char*)&(packet->destinationAddress))+i);
	}
	checksum += packet->options;
	for (i = 0; i < packet->dataSize; ++i)
	{
		checksum += packet->data[i];
	}
	checksum = (0xFF - checksum) & 0xFF;

	xbee_SendBytes((unsigned char*)&delimiter, 1);
	xbee_SendBytes(((unsigned char*)&size) + 1, 1);
	xbee_SendBytes(((unsigned char*)&size), 1);
	xbee_SendBytes((unsigned char*)&identifier, 1);
	xbee_SendBytes((unsigned char*)&(packet->frameId), 1);
	for (i = 0; i < 8; ++i)
	{
		xbee_SendBytes((((unsigned char*)&(packet->destinationAddress)) + (7 - i)), 1);
	}
	xbee_SendBytes((unsigned char*)&(packet->options), 1);
	if (packet->dataSize > 0 && packet->data)
		xbee_SendBytes(packet->data, packet->dataSize);
	xbee_SendBytes((unsigned char*)&checksum, 1);

	if (xbee_flushFunc)
		xbee_flushFunc();
}

/*!
*	This function is called to transmit data using a 16-bits address
*
*	@param packet A pointer to a structure containing the packet to send
*/
void xbee_SendTxFrame16(xbee_TxFrame16 * packet)
{
	unsigned short int size = 5 + packet->dataSize;
	unsigned char delimiter = XBEE_DELIMITER;
	unsigned char identifier = XBEE_OUT_TX_16;
	unsigned char checksum = 0;
	unsigned short int i = 0;

	checksum += identifier;
	checksum += packet->frameId;
	for (i = 0; i < 2; ++i)
	{
		checksum += *(((char*)&(packet->destinationAddress)) + i);
	}
	checksum += packet->options;
	for (i = 0; i < packet->dataSize; ++i)
	{
		checksum += packet->data[i];
	}
	checksum = (0xFF - checksum) & 0xFF;

	xbee_SendBytes((unsigned char*)&delimiter, 1);
	xbee_SendBytes(((unsigned char*)&size) + 1, 1);
	xbee_SendBytes(((unsigned char*)&size), 1);
	xbee_SendBytes((unsigned char*)&identifier, 1);
	xbee_SendBytes((unsigned char*)&(packet->frameId), 1);
	for (i=0; i < 2; ++i)
	{
		xbee_SendBytes((((unsigned char*)&(packet->destinationAddress)) + (1 - i)), 1);
	}
	xbee_SendBytes((unsigned char*)&(packet->options), 1);
	if (packet->dataSize > 0 && packet->data)
		xbee_SendBytes(packet->data, packet->dataSize);
	xbee_SendBytes((unsigned char*)&checksum, 1);

	if (xbee_flushFunc)
		xbee_flushFunc();
	
}

/*!
*	This function is called from outside to send an AT command to the module
*
*	@param command A pointer to a structure containing the AT command to execute
*/
void xbee_SendAtCommand(xbee_AtCommand * command)
{
	unsigned short int size = 4 + command->parameterSize;
	unsigned char delimiter = XBEE_DELIMITER;
	unsigned char identifier = XBEE_OUT_AT_COMMAND;
	unsigned char checksum = 0;
	unsigned short int i = 0;

	checksum += identifier;
	checksum += command->frameId;
	checksum += command->command[0];
	checksum += command->command[1];
	for (i = 0; i < command->parameterSize; ++i)
	{
		checksum += command->parameter[i];
	}

	xbee_SendBytes((unsigned char*)&delimiter, 1);
	xbee_SendBytes(((unsigned char*)&size) + 1, 1);
	xbee_SendBytes(((unsigned char*)&size), 1);
	xbee_SendBytes((unsigned char*)&identifier, 1);
	xbee_SendBytes((unsigned char*)&(command->frameId), 1);
	xbee_SendBytes((unsigned char*)(command->command), 2);
	if (command->parameterSize > 0 && command->parameter)
		xbee_SendBytes((unsigned char *)command->parameter, command->parameterSize);

	checksum = (0xFF - checksum) & 0xFF;
	xbee_SendBytes((unsigned char*)&checksum, 1);

	if (xbee_flushFunc)
		xbee_flushFunc();
}


/*!
*	Warning: all these functions are called during interruptions, so they have to be very short !
*
*	@param rcvModemStatusFunc A pointer to the function to call when a modem status message is received. Can be null.
*	@param rcvAtCommandResponseFunc A pointer to the function to call when an AT command response is received. Can be null.
*	@param rcvTxStatusFunc A pointer to the function to call when a TX status message is received. Can be null.
*	@param rcvRfData64Func A pointer to the function to call when an incoming RF message with 64-bits address is received. Can be null.
*	@param rcvRfData16Func A pointer to the function to call when an incoming RF message with 16-bits address is received. Can be null.
*	@param rcvRfDataByteFunc A pointer to the function to call each time an incoming RF byte arrives. Warning: this function sends each byte of the message content directly, it does not check its validity (no checksum checked). Can be null.
*	@param sendBytesFunc A pointer to the function to call to send bytes through communication. Can NOT be null.
*	@param flushFunc A pointer to the function to call to flush data on the communication. Can be null.
*/
void xbee_Init(xbee_RcvModemStatusFunc rcvModemStatusFunc, xbee_RcvAtCommandResponseFunc rcvAtCommandResponseFunc, xbee_RcvTxStatusFunc rcvTxStatusFunc, xbee_RcvRfData64Func rcvRfData64Func, xbee_RcvRfData16Func rcvRfData16Func, xbee_RcvRfDataByteFunc rcvRfDataByteFunc, xbee_SendBytesFunc sendBytesFunc, xbee_FlushFunc flushFunc)
{
	xbee_rcvModemStatusFunc = rcvModemStatusFunc;
	xbee_rcvAtCommandResponseFunc = rcvAtCommandResponseFunc;
	xbee_rcvTxStatusFunc = rcvTxStatusFunc;
	xbee_rcvRfData64Func = rcvRfData64Func;
	xbee_rcvRfData16Func = rcvRfData16Func;
	xbee_rcvRfDataByteFunc = rcvRfDataByteFunc;

	xbee_sendBytesFunc = sendBytesFunc;
	xbee_flushFunc = flushFunc;
}

/*!
*	This function takes data in input, puts the data in a 16-bits address packet
*	When the packet is full, it is automatically flushed and a new packet is started
*
*	@param data A pointer to the data to packetize
*	@param length The number of bytes to packetize from the data pointer
*
*	\see xbee_SetDestinationAndOptions
*	\see xbee_FlushPacket
*/
void xbee_PacketizeData(unsigned char * data, unsigned long length)
{
	unsigned long i;
	
	xbee_numberOfBytesPacketized += length;

	for (i = 0; i < length; ++i)
	{
		xbee_sndBuffer[xbee_sndPacketSize++] = data[i];

		// packet is full, send it
		if (xbee_sndPacketSize == XBEE_MAX_PACKET_SIZE)
		{
			xbee_FlushPacket();
		}
	}
}

/*!
*	This function flushes the current packet to the communication
*	The packet is send to the current destination address with the current options
*
*	\see xbee_SetDestinationAndOptions
*/
void xbee_FlushPacket()
{
	// set frame id
	++xbee_sndPacketFrameId;
	if (xbee_sndPacketFrameId == 0)
	{
		xbee_sndPacketFrameId = 1; // not 0 because 0 is a special number that disables TX status responses
	}
	xbee_txFrame16.frameId = xbee_sndPacketFrameId;

	// link data to frame
	xbee_txFrame16.data = xbee_sndBuffer;
	xbee_txFrame16.dataSize = xbee_sndPacketSize;

	// send frame
	xbee_SendTxFrame16(&xbee_txFrame16);

	// printf("\nPacket sent : %u",xbee_sndPacketSize);

	// reset size for next packet
	xbee_sndPacketSize = 0;
}

/*!
*	This function sets the destination and the options of all the future packet to be flushed. The current packet is first flushed to the current destination (before
*	modification)
*
*	@param destination The 16-bits destination address of the packet, 0xFFFF means broadcast to all card in the same PAN Id.
*	@param options The options for the packet (flags) : 0x01 = Disable ACK, 0x04 = Send packet with broadcast PAN Id. All other bits must be set to 0.
*/
void xbee_SetDestinationAndOptions(unsigned short destination, unsigned char options)
{
	// first, flush packet with curret data before modifying address
	if (xbee_sndPacketSize > 0)
	{
		xbee_FlushPacket();
	}
	
	xbee_txFrame16.destinationAddress = destination;
	xbee_txFrame16.options = options;
}

/*!
*	This function modifies the send function of the XBee API
*	
*	@param sendBytesFunc The new send function to use
*/
void xbee_SetSendFunction(xbee_SendBytesFunc sendBytesFunc)
{
	xbee_sendBytesFunc = sendBytesFunc;	
}

/*!
*	This function modifies the flush function of the XBee API
*
*	@param flushFunc The new flush function to use
*/
void xbee_SetFlushFunction(xbee_FlushFunc flushFunc)
{
	xbee_flushFunc = flushFunc;	
}
