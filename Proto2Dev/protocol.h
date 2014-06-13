/**
 *  Bed sensor's communication protocol.
 *    
 *  Provide functions and structures in order 
 *  to creates and manages communication's 
 *  frames between sensor and platform.
 *  
 *  @file protocol.h
 *  @date 12 juin 2014
 *  @copyright PAWM International
 *    
 *  @author Mickael Germain
 *  
 *  @todo global tutorial.
 */

#ifndef PROTOCOL_H
# define PROTOCOL_H

# include <inttypes.h>
# include <string.h>
# include <assert.h>
 
# ifndef __cplusplus
#  include <stdbool.h>
# endif
 
# include "endian.h"

  /**
   *    Leading character of frames.
   */
# define PROTOCOL_FRAME_START       '$'
  /**
   *    Terminating character of frames.
   */
# define PROTOCOL_FRAME_END         '\n'
  /**
   *    Separating character between argument in frames.
   */
# define PROTOCOL_FRAME_SEP         ','
  /**
   *    Size of the start of frames.
   */
# define PROTOCOL_FRAME_START_SIZE  sizeof(char)
  /**
   *    Size of the end of frames.
   */
# define PROTOCOL_FRAME_END_SIZE    sizeof(char)
  /**
   *    Size of a separator in frames.
   */
# define PROTOCOL_FRAME_SEP_SIZE    sizeof(char)
  /**
   *    Size of a frame type identifier.
   */
# define PROTOCOL_FRAME_TYPE_SIZE   sizeof(char) * 3
  /**
   *    Size of a time argument in frames.
   */
# define PROTOCOL_FRAME_TIME_SIZE   sizeof(uint32_t)
  /**
   *    Size of an error argument in frames.
   */
# define PROTOCOL_FRAME_ERR_SIZE    sizeof(eProtocolError)
  /**
   *    Size of a runtime mode identifier in a frames.
   */
# define PROTOCOL_FRAME_MOD_SIZE    sizeof(eProtocolMode)
  /**
   *    Number of FSR sensors in the bed.
   *    
   *    @todo Think about an extern constant.
   */
# define PROTOCOL_FSR_NUMBER        8
  /**
   *    Number of FSC sensors in the bed.
   *    
   *    @todo Think about an extern constant.
   */
# define PROTOCOL_FSC_NUMBER        2
  /**
   *    Size of an FSR sensor's data in frames.
   */
# define PROTOCOL_FRAME_FSR_SIZE    sizeof(uint16_t)
  /**
   *    Size of an FSC sensor's data in frames.
   */
# define PROTOCOL_FRAME_FSC_SIZE    sizeof(uint16_t)
  /**
   *    Size of an ACK frame.
   */
# define PROTOCOL_ACK_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *    Size of an YOP frame.
   */
# define PROTOCOL_YOP_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (2)                                                 + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (2)                                                 + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *    Size of a SYN frame.
   */                                                                
# define PROTOCOL_SYN_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size of an ERR frame.
   */
# define PROTOCOL_ERR_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_ERR_SIZE)                           + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size of an MOD frame.
   */
# define PROTOCOL_MOD_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_MOD_SIZE)                           + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size of a DR1 frame.
   */
# define PROTOCOL_DR1_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size of a DC1 frame.
   */
# define PROTOCOL_DC1_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)   + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size of a DA1 frame.
   */
# define PROTOCOL_DA1_SIZE          (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)   + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Size between a DCN frame of n elements and a DCN frame of n + 1 elements.
   */
# define PROTOCOL_DCN_VAR_SIZE      (   (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)     \
                                    )
  /**
   *	Minimum size of a DCN frame.
   */
# define PROTOCOL_DCN_MIN_SIZE      (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_DCN_VAR_SIZE)                             + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *    Size between a DAN frame of n elements and a DAN frame of n + 1 elements.
   */
# define PROTOCOL_DAN_VAR_SIZE      (   (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)     \
                                    )
  /**
   *    Minimum size of a DAN frame.
   */
# define PROTOCOL_DAN_MIN_SIZE      (   (PROTOCOL_FRAME_START_SIZE)                         + \
                                        (PROTOCOL_FRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_FRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_FRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_DAN_VAR_SIZE)                             + \
                                        (PROTOCOL_FRAME_END_SIZE)                             \
                                    )
  /**
   *	Maximum size of a frame in this protocol.
   */
# define PROTOCOL_DATA_SIZE_MAX     1024
  /**
   *	Maximum number of samples in a DCN frame.
   */
# define PROTOCOL_DCN_SAMPLE_MAX    (((PROTOCOL_DATA_SIZE_MAX) - (PROTOCOL_DCN_MIN_SIZE)) / (PROTOCOL_DCN_VAR_SIZE) + 1)
  /**
   *	Maximum number of samples in a DAN frame.
   */
# define PROTOCOL_DAN_SAMPLE_MAX    (((PROTOCOL_DATA_SIZE_MAX) - (PROTOCOL_DAN_MIN_SIZE)) / (PROTOCOL_DAN_VAR_SIZE) + 1)

 /**
  *	Enumeration of all frame's type.
  * 
  * @see protocol_FrameIdentification()
  */
 typedef enum
 {
    cProtocolFrameACK = 0,  /**< ACK : Frame for acknowledgment. */
    cProtocolFrameYOP,      /**< YOP : Frame send by a bed sensor at startup in order to be identified by a platform. */
    cProtocolFrameSYN,      /**< SYN : Frame for time synchronization between a bed sensor and a platform. */
    cProtocolFrameERR,      /**< ERR : Frame to report a error. */
    cProtocolFrameBAT,      /**< BAT : Frame to inform platform about bed sensor's level of battery */
    cProtocolFrameMOD,      /**< MOD : Frame use by platform to order bed sensor to fall in a specific runtime mode. */
    cProtocolFrameDR1,      /**< DR1 : Frame for FSR's data sample encapsulation. */
    cProtocolFrameDC1,      /**< DC1 : Frame for one FSC's data sample encapsulation. */
    cProtocolFrameDCN,      /**< DCN : Frame for multiple FSC's data samples encapsulation. */
    cProtocolFrameDA1,      /**< DA1 : Frame for one FSR's data sample and one FSC's data sample encapsulation. */
    cProtocolFrameDAN,      /**< DAN : Frame for multiple FSR's data samples and FSC's data samples  (nbFSR = nbFSC) encapsulation. */
    /* ^-insert new frames at the end-^ */
    /*--------END OF ENUMERATION--------*/
    cProtocolFrameNumber,   /**< Number of frame's type. */
    cProtocolFrameUnknow    /**< Value for an unknown type of frame. */
 } eProtocolFrame;
 
 /**
  *	Enumeration of bed sensor's runtime mode.
  */
 typedef enum
 {
    cProtocolModeSleep = 0, /**< Waiting for a person lies. */
    cProtocolModeNormal,    /**< Sampling with a normal accuracy in order to monitor person's breathing. */
    cProtocolModeAccurate,  /**< Sampling with a high resolution in order to detect and analyze problems. */
    /* ^-insert new runtime mode at the end-^ */
    /*-----------END OF ENUMERATION-----------*/
    cProtocolModeNumber,    /**< Number of runtime modes. */
    cProtocolModeUnknow     /**< Value for an unknown runtime modes. */
 } eProtocolMode;

 /**
  *	Enumeration of type of protocol errors.
  * @todo
  */
 typedef enum
 {
    cProtocolErrorFirst = 0,
    /* ^-insert new error at the end-^ */
    /*--------END OF ENUMERATION-------*/
    cProtocolErrorNumber,   /**< Number of type of errors. */
    cProtocolErrorUnknow    /**< Value for an unknown type of errors. */
 } eProtocolError;

/* @todo maybe*/
 typedef struct
 {
    bool (*parser)(void* data);
    //bool (*constructor)();
 } sProtocolFrame;
 
 /**
  *	Container for DR1 frame data.
  */
 struct sProtocolDR1
 {
    uint32_t time;                              /**< Time of acquisition from millis(). */
    uint16_t fsrValues[PROTOCOL_FSR_NUMBER];    /**< FSR's data samples. */
 };

 /**
  *	Container for DA1 frame data.
  */
 struct sProtocolDA1
 {
    uint32_t time;                              /**< Time of acquisition from millis(). */
    uint16_t fsrValues[PROTOCOL_FSR_NUMBER];    /**< FSR's data samples. */
    uint16_t fscValues[PROTOCOL_FSC_NUMBER];    /**< FSC's data samples. */
 };

 /**
  *	Container for DC1 frame data.
  */
 struct sProtocolDC1
 {
    uint32_t time;                              /**< Time of acquistion from millis(). */
    uint16_t fscValues[PROTOCOL_FSC_NUMBER];    /**< FSC's data samples. */
 };
 
 /**
  *	Container for DCN frame data.
  */
 struct sProtocolDCN
 {
    uint32_t time;                                                      /**< Time of the first acquisition from millis(). */
    uint32_t delta;                                                     /**< Time between both consecutive sampling waves. */
    uint16_t fscValues[PROTOCOL_DCN_SAMPLE_MAX][PROTOCOL_FSC_NUMBER];   /**< FSC's data from sampling waves sort by time of acquisition. */
    uint16_t nbSamples;                                                 /**< Number of sampling waves in the frame. */
 };

 struct sProtocolDAN
 {
    uint32_t time;                                                      /**< Time of the first acquisition from millis(). */
    uint32_t delta;                                                     /**< Time between both consecutive sampling waves. */
    uint16_t fscValues[PROTOCOL_DAN_SAMPLE_MAX][PROTOCOL_FSC_NUMBER];   /**< FSR's data from sampling waves sort by time of acquisition. */
    uint16_t fsrValues[PROTOCOL_DAN_SAMPLE_MAX][PROTOCOL_FSR_NUMBER];   /**< FSC's data from sampling waves sort by time of acquisition. */
    uint16_t nbSamples;                                                 /**< Number of sampling waves in the frame. */
 };

 /* @todo documentation */
 typedef uint8_t tProtocol_bufferACK [PROTOCOL_ACK_SIZE];
 typedef uint8_t tProtocol_bufferYOP [PROTOCOL_YOP_SIZE];
 typedef uint8_t tProtocol_bufferSYN [PROTOCOL_SYN_SIZE];
 typedef uint8_t tProtocol_bufferERR [PROTOCOL_ERR_SIZE];
 typedef uint8_t tProtocol_bufferMOD [PROTOCOL_MOD_SIZE];
 typedef uint8_t tProtocol_bufferDR1 [PROTOCOL_DR1_SIZE];
 typedef uint8_t tProtocol_bufferDC1 [PROTOCOL_DC1_SIZE];
 typedef uint8_t tProtocol_bufferDA1 [PROTOCOL_DA1_SIZE];
 typedef uint8_t tProtocol_bufferDCN [PROTOCOL_DATA_SIZE_MAX];
 typedef uint8_t tProtocol_bufferDAN [PROTOCOL_DATA_SIZE_MAX];
 
/**
 *	Waiting to find a start of frame delimiter.
 */
# define protocol_waitStartOfFrame()    while (!protocol_isStartOfFrame())
/**
 *	Determines if the next character is a start of frame delimiter.
 *  
 *  @return true if the next character is a start of frame delimiter, false otherwise.
 */
# define protocol_isStartOfFrame()      (protocol_readChar() == PROTOCOL_FRAME_START)
/**
 *	Determines if the next character is a end of frame delimiter.
 *  
 *  @return true if the next character is a end of frame delimiter, false otherwise.
 */
# define protocol_isEndOfFrame()        (protocol_readChar() == PROTOCOL_FRAME_END)
/**
 *	Determines if the next character is an argument separator.
 *  
 *  @return true if the next character is an argument separator, false otherwise.
 */
# define protocol_isSeparator()         (protocol_readChar() == PROTOCOL_FRAME_SEP)
 
 /**
  *	Table of frame identifier.
  * 
  * @see cProtocolFrame
  */
 extern char const protocol_FrameId[cProtocolFrameNumber][PROTOCOL_FRAME_TYPE_SIZE + 1];
/**
 *	Reference of the function for reading communication stream.
 *  
 *  This function should return the next character in the stream.
 *  
 *  @warning It must be set by user.
 */
 extern char (*protocol_readChar)(void);

# ifdef __cplusplus
  extern "C"{
# endif

 //////////////////////////////////////////////////////////////////////////
 // Frame Identification
 
 /**
  * Identifies the type of frame.
  *	
  *	@param [in] buffer
  *     String containing the frame identifier to be analyzed.
  *	
  *	@return The corresponding value of the identified frame hopefully, cProtocolFrameUnknow otherwise.
  */
  eProtocolFrame protocol_FrameIdentification(char const buffer[PROTOCOL_FRAME_TYPE_SIZE]);
 
 //////////////////////////////////////////////////////////////////////////
 // Frame Parsing
 
 /**
  *	Tries to parse argument of an ACK frame.
  *  
  *  @return true if the parsing succeeded, false otherwise.
  *  
  *  @see protocol_createACK()
  */
# define protocol_parseACK() protocol_isEndOfFrame()

 /**
  *	Tries to parse a YOP frame.
  *	
  *	@param [out] fsrNumber
  *	    Reference for storing the number of FSR of the bed sensor.
  * @param [out] fscNumber
  *     Reference for storing the Number of FSC of the bed sensor.
  * 
  * @return true if the parsing succeeded, false otherwise.
  * 
  * @see protocol_createYOP()
  */
 bool protocol_parseYOP(uint8_t* fsrNumber, uint8_t* fscNumber);
 
 /**
  *	Tries to parse a SYN frame.
  *	
  *	@param [out] timeData
  *     Reference for storing synchronization time.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createSYN()
  */
 bool protocol_parseSYN(uint32_t* timeData);
 
 /**
  *	Tries to parse a ERR frame.
  *	
  *	@param [out] errNum
  *     Reference for storing error number.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createERR()
  */
 bool protocol_parseERR(eProtocolError* errNum);
 
 /**
  *	Tries to parse a MOD frame.
  *	
  *	@param [out] modeNum
  *     Reference for storing mode number.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createMOD()
  */
 bool protocol_parseMOD(eProtocolMode* modeNum);
 
 /**
  *	Tries to parse a DR1 frame.
  *	
  *	@param [out] sDr1
  *     Reference for storing data of the frame.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createDR1()
  */
 bool protocol_parseDR1(struct sProtocolDR1* sDr1);
 
 /**
  *	Tries to parse a DC1 frame.
  *	
  *	@param [out] sDc1
  *     Reference for storing data of the frame.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createDC1()
  */
 bool protocol_parseDC1(struct sProtocolDC1* sDc1);
 
 /**
  *	Tries to parse a DCN frame.
  *	
  *	@param [out] sDcn
  *     Reference for storing data of the frame.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createDCN()
  */
 bool protocol_parseDCN(struct sProtocolDCN* sDcn);
 
 /**
  *	Tries to parse a DA1 frame.
  *	
  *	@param [out] sDa1
  *     Reference for storing data of the frame.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createDA1()
  */
 bool protocol_parseDA1(struct sProtocolDA1* sDa1);
 
 /**
  *	Tries to parse a DAN frame.
  *	
  *	@param [out] sDan
  *     Reference for storing data of the frame.
  *
  * @return true if the parsing succeeded, false otherwise.
  *
  * @see protocol_createDAN()
  */
 bool protocol_parseDAN(struct sProtocolDAN* sDan);
 
 //////////////////////////////////////////////////////////////////////////
 // Frame Creation
 
 /**
  *	Create an ACK frame.
  * 
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseACK()
  */
 uint16_t protocol_createACK(tProtocol_bufferACK buffer);
 
 /**
  *	Create a YOP frame.
  * 
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  */
 uint16_t protocol_createYOP(tProtocol_bufferYOP buffer);
 
 /**
  *	Create a SYN frame.
  * 
  * @param [in]  timeData
  *     Time to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseSYN()
  * 
  * @todo Change to direct use of millis ?
  */
 uint16_t protocol_createSYN(const uint32_t timeData, tProtocol_bufferSYN buffer);
 
 /**
  *	Create a ERR frame.
  * 
  * @param [in]  errNum
  *     The error number id to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseERR()
  */
 uint16_t protocol_createERR(const eProtocolError errNum, tProtocol_bufferERR buffer);
 
 /**
  *	Create an MOD frame.
  * 
  * @param [in]  modeNum
  *     The runtime mode number id to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseMOD()
  */
 uint16_t protocol_createMOD(const eProtocolMode modeNum, tProtocol_bufferMOD buffer);
 
 /**
  *	Create a DR1 frame.
  * 
  * @param [in]  sDr1
  *     Reference to the container where find data to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseDR1()
  */
 uint16_t protocol_createDR1(struct sProtocolDR1 const* sDr1, tProtocol_bufferDR1 buffer);
 
 /**
  *	Create a DC1 frame.
  * 
  * @param [in]  sDc1
  *     Reference to the container where find data to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseDR1()
  */
 uint16_t protocol_createDC1(struct sProtocolDC1 const* sDc1, tProtocol_bufferDC1 buffer);
 
 /**
  *	Create a DA1 frame.
  * 
  * @param [in]  sDa1
  *     Reference to the container where find data to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseDA1()
  */
 uint16_t protocol_createDA1(struct sProtocolDA1 const* sDa1, tProtocol_bufferDA1 buffer);

 // DCN
 
 /**
  *	Create a DCN frame.
  * 
  * @param [in]  sDcn
  *     Reference to the container where find data to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed start of frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseDCN()
  * @see protocol_initDCN()
  * @see protocol_extendDCN()
  * @see protocol_endDCN()
  */
 uint16_t protocol_createDCN(struct sProtocolDCN const* sDcn, tProtocol_bufferDCN buffer);
 
 /**
  *	Initializes a DCN frame.
  * 
  * Create the start of a DCN frame including 
  * the header part (start of frame and frame id),
  * timing arguments and a first wave of sampling.
  * 
  * @warning You should at least perform a call to the protool_endDCN()
  *         function in order to end the frame properly.
  * 
  * @param [in]  sDc1
  *     Reference to the container where find data to put in the frame.
  * @param [in] delta
  *     Time between two waves of sampling.
  * @param [out] buffer
  *     Reference to the current end of the frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_createDCN()
  * @see protocol_extendDCN()
  * @see protocol_endDCN()
  */
 uint16_t protocol_initDCN(struct sProtocolDC1 const* sDc1, const uint32_t delta, uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_FRAME_END]);
 
 /**
  *	Extends a DCN frame.
  * 
  * Add one other wave of sampling to an pre-initialized frame.
  * You can chain call to this function in order to put more wave 
  * of sampling in the frame.
  *
  * @warning You should at least perform a call to the protool_endDCN()
  *         function in order to end the frame properly.
  * 
  * @param [in]  fscValues
  *     Reference to the wave of FSC's sampling to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  * 
  * @todo check the number of extends.
  * 
  * @see protocol_createDCN()
  * @see protocol_initDCN()
  * @see protocol_endDCN()
  */
 uint16_t protocol_extendDCN(uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DCN_VAR_SIZE]);
 
 /**
  *	Ends a DCN frame.
  * 
  * @param [out] buffer
  *     Reference to the current end of the frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_createDCN()
  * @see protocol_initDCN()
  * @see protocol_extendDCN()
  */
 uint16_t protocol_endDCN(uint8_t buffer[PROTOCOL_FRAME_END_SIZE]);

 // DAN
 
 /**
  *	Create a DAN frame.
  * 
  * @param [in]  sDan
  *     Reference to the container where find data to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed start of frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_parseDAN()
  * @see protocol_initDAN()
  * @see protocol_extendDAN()
  * @see protocol_endDAN()
  */
 uint16_t protocol_createDAN(struct sProtocolDAN const* sDan, tProtocol_bufferDAN buffer);
 
 /**
  *	Initializes a DAN frame.
  * 
  * Create the start of a DAN frame including 
  * the header part (start of frame and frame id),
  * timing arguments and a first wave of sampling.
  * 
  * @warning You should at least perform a call to the protool_endDAN()
  *         function in order to end the frame properly.
  * 
  * @param [in]  sDa1
  *     Reference to the container where find data to put in the frame.
  * @param [in] delta
  *     Time between two waves of sampling.
  * @param [out] buffer
  *     Reference to the current end of the frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_createDAN()
  * @see protocol_extendDAN()
  * @see protocol_endDAN()
  */
 uint16_t protocol_initDAN(struct sProtocolDA1 const* sDa1, const uint32_t delta, uint8_t buffer[PROTOCOL_DAN_MIN_SIZE - PROTOCOL_FRAME_END]);
 
 /**
  *	Extends a DAN frame.
  * 
  * Add one other wave of sampling to an pre-initialized frame.
  * You can chain call to this function in order to put more wave 
  * of sampling in the frame.
  *
  * @warning You should at least perform a call to the protool_endDAN()
  *         function in order to end the frame properly.
  * 
  * @param [in]  fsrValues
  *     Reference to the wave of FSR's sampling to put in the frame.
  * @param [in]  fscValues
  *     Reference to the wave of FSC's sampling to put in the frame.
  * @param [out] buffer
  *     Reference where store the computed frame.
  * 
  * @return The length of the frame.
  *
  * @todo check the number of extends.
  *
  * @see protocol_createDAN()
  * @see protocol_initDAN()
  * @see protocol_endDAN()
  */
 uint16_t protocol_extendDAN(uint16_t const fsrValues[PROTOCOL_FSR_NUMBER], uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DAN_VAR_SIZE]);
 
 /**
  *	Ends a DAN frame.
  * 
  * @param [out] buffer
  *     Reference to the current end of the frame.
  * 
  * @return The length of the frame.
  *
  * @see protocol_createDAN()
  * @see protocol_initDAN()
  * @see protocol_extendDAN()
  */
 inline uint16_t protocol_endDAN(uint8_t buffer[PROTOCOL_FRAME_END_SIZE]);
 
# ifdef __cplusplus
  }
# endif
#endif
