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
*	\file xbee_api_def.h
*	\brief Definition file of the XBee API library
*
*	\author Alexandre Habersaat
*	\date 2008-08-07
*
*	This file contains all the definitions and enumerations of the library.
*
*/

#ifndef __XBEE_API_DEF_H__
#define __XBEE_API_DEF_H__

#ifdef AEROPIC_PREFIX
	#include AEROPIC_PREFIX
#endif

//--------------------
// Public definitions
//--------------------

//! The XBee broadcast address on 64 bits
#define XBEE_BROADCAST_64 0x000000000000FFFF
//! The XBee broadcast address on 16 bits
#define XBEE_BROADCAST_16 0xFFFF

//! The delimiter character
#define XBEE_DELIMITER 0x7E
//! The escape character
#define XBEE_ESCAPE_CHAR 0x7D
//! The XOR value to escape character c xor XBEE_ESCAPE_XOR
#define XBEE_ESCAPE_XOR 0x20

#define XBEE_XON 0x11
#define XBEE_XOFF 0x13

//! API identifier for Modem status
#define XBEE_IN_MODEM_STATUS 0x8A
//! API identifier for AT command
#define XBEE_OUT_AT_COMMAND 0x08
//! API identifier for AT command queue
//#define XBEE_OUT_AT_COMMAND_QUEUE 0x09
//! API identifier for AT command response
#define XBEE_IN_AT_COMMAND_RESPONSE 0x88
//! API identifier for outgoing packets with 64-bits address
#define XBEE_OUT_TX_64 0x00
//! API identifier for outgoing packets with 16-bits address
#define XBEE_OUT_TX_16 0x01
//! API identifier for response to outgoing packets
#define XBEE_IN_TX_STATUS 0x89
//! API identifier for incoming packets with 64-bits address
#define XBEE_IN_RX_64 0x80
//! API identifier for incoming packets with 16-bits address
#define XBEE_IN_RX_16 0x81

//! The number of size bytes used in the packets
#define XBEE_NUMBER_OF_SIZE_BYTES 2

//! The maximum size of an XBee packet
#define XBEE_MAX_PACKET_SIZE 100

//! The state of the Finite-State-Machine for the decoding of incoming data frame from the Xbee module
enum XBee_State
{
	XBEE_STATE_DELIMITER=0, //!< Waiting for the delimiter (start)
	XBEE_STATE_LENGTH, //!< Reading the length of the packet
	XBEE_STATE_API_IDENTIFIER, //!< Reading the type of message
	XBEE_STATE_MODEM_STATUS, //!< Reading a message containing the status of the module
	XBEE_STATE_AT_RESPONSE, //!< Reading a message containing the response to an AT command
	XBEE_STATE_TX_STATUS, //!< Reading a message containing the status of a sent packet
	XBEE_STATE_RX64, //!< Reading a message containing incoming RF data with a 64-bits address
	XBEE_STATE_RX16, //!< Reading a message containing incoming RF data with a 16-bits address
	XBEE_STATE_CHECKSUM 
};


#endif
