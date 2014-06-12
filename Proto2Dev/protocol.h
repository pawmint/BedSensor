#ifndef PROTOCOL_H
# define PROTOCOL_H

# include <inttypes.h>
# include <string.h>
# include <assert.h>
 
# ifndef __cplusplus
#  include <stdbool.h>
# endif
 
# include "endian.h"

# define PROTOCOL_TRAME_START       '$'
# define PROTOCOL_TRAME_END         '\n'
# define PROTOCOL_TRAME_SEP         ','

# define PROTOCOL_TRAME_START_SIZE  sizeof(char)
# define PROTOCOL_TRAME_END_SIZE    sizeof(char)
# define PROTOCOL_TRAME_SEP_SIZE    sizeof(char)

# define PROTOCOL_TRAME_TYPE_SIZE   sizeof(char) * 3
# define PROTOCOL_TRAME_TIME_SIZE   sizeof(uint32_t)
# define PROTOCOL_TRAME_ERR_SIZE    sizeof(eProtocolError)
# define PROTOCOL_TRAME_MOD_SIZE    sizeof(eProtocolMode)


# define PROTOCOL_FSR_NUMBER        8
# define PROTOCOL_FSC_NUMBER        2
# define PROTOCOL_TRAME_FSR_SIZE    sizeof(uint16_t)
# define PROTOCOL_TRAME_FSC_SIZE    sizeof(uint16_t)

# define PROTOCOL_ACK_SIZE			(	(PROTOCOL_TRAME_START_SIZE)							+ \
										(PROTOCOL_TRAME_TYPE_SIZE)							+ \
										(PROTOCOL_TRAME_END_SIZE)							  \
									)

# define PROTOCOL_YOP_SIZE			(	(PROTOCOL_TRAME_START_SIZE)							+ \
										(PROTOCOL_TRAME_TYPE_SIZE)							+ \
										(PROTOCOL_TRAME_SEP_SIZE)							+ \
										(2)													+ \
										(PROTOCOL_TRAME_SEP_SIZE)							+ \
										(2)													+ \
										(PROTOCOL_TRAME_END_SIZE)							  \
									)
																
# define PROTOCOL_SYN_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_ERR_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_ERR_SIZE)                           + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_MOD_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_MOD_SIZE)                           + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_DR1_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_DC1_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)   + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_DA1_SIZE          (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)   + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_DCN_VAR_SIZE      (   (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)     \
                                    )

# define PROTOCOL_DCN_MIN_SIZE      (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_DCN_VAR_SIZE)                             + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )

# define PROTOCOL_DAN_VAR_SIZE      (   (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSR_SIZE) * (PROTOCOL_FSR_NUMBER)   + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_FSC_SIZE) * (PROTOCOL_FSC_NUMBER)     \
                                    )

# define PROTOCOL_DAN_MIN_SIZE      (   (PROTOCOL_TRAME_START_SIZE)                         + \
                                        (PROTOCOL_TRAME_TYPE_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_TRAME_SEP_SIZE)                           + \
                                        (PROTOCOL_TRAME_TIME_SIZE)                          + \
                                        (PROTOCOL_DAN_VAR_SIZE)                             + \
                                        (PROTOCOL_TRAME_END_SIZE)                             \
                                    )
 
# define PROTOCOL_DATA_SIZE_MAX     1024

# define PROTOCOL_DCN_SAMPLE_MAX    (((PROTOCOL_DATA_SIZE_MAX) - (PROTOCOL_DCN_MIN_SIZE)) / (PROTOCOL_DCN_VAR_SIZE) + 1)
# define PROTOCOL_DAN_SAMPLE_MAX    (((PROTOCOL_DATA_SIZE_MAX) - (PROTOCOL_DAN_MIN_SIZE)) / (PROTOCOL_DAN_VAR_SIZE) + 1)

 typedef enum
 {
    cProtocolTrameACK = 0,
    cProtocolTrameYOP,
    cProtocolTrameSYN,
    cProtocolTrameERR,
    cProtocolTrameBAT,
    cProtocolTrameMOD,
    cProtocolTrameDR1,
    cProtocolTrameDC1,
    cProtocolTrameDCN,
    cProtocolTrameDA1,
    cProtocolTrameDAN,
    /*-----*/
    cProtocolTrameNumber,
    cProtocolTrameUnknow
 } eProtocolTrame;

 typedef enum
 {
    cProtocolModeSleep = 0,
    cProtocolModeNormal,
    cProtocolModeAccurate,
    /*-----*/
    cProtocolModeNumber,
    cProtocolModeUnknow
 } eProtocolMode;

 typedef enum
 {
    cProtocolErrorFirst = 0,
    /*-----*/
    cProtocolErrorNumber,
    cProtocolErrorUnknow
 } eProtocolError;

 typedef struct
 {
    bool (*parser)(void* data);
    //bool (*constructor)();
 } sProtocolTrame;

 struct sProtocolDR1
 {
    uint32_t time;
    uint16_t fsrValues[PROTOCOL_FSR_NUMBER];
 };

  struct sProtocolDA1
 {
    uint32_t time;
    uint16_t fsrValues[PROTOCOL_FSR_NUMBER];
    uint16_t fscValues[PROTOCOL_FSC_NUMBER];
 };

 struct sProtocolDC1
 {
    uint32_t time;
    uint16_t fscValues[PROTOCOL_FSC_NUMBER];
 };

 struct sProtocolDCN
 {
    uint32_t time;
    uint32_t delta;
    uint16_t fscValues[PROTOCOL_DCN_SAMPLE_MAX][PROTOCOL_FSC_NUMBER];
    uint16_t nbSamples;
 };

 struct sProtocolDAN
 {
    uint32_t time;
    uint32_t delta;
    uint16_t fscValues[PROTOCOL_DAN_SAMPLE_MAX][PROTOCOL_FSC_NUMBER];
    uint16_t fsrValues[PROTOCOL_DAN_SAMPLE_MAX][PROTOCOL_FSR_NUMBER];
    uint16_t nbSamples;
 };

# define protocol_waitStartOfTrame()    while (!protocol_isStartOfTrame())
# define protocol_isStartOfTrame()      (protocol_readChar() == PROTOCOL_TRAME_START)
# define protocol_isEndOfTrame()        (protocol_readChar() == PROTOCOL_TRAME_END)
# define protocol_isSeparator()         (protocol_readChar() == PROTOCOL_TRAME_SEP)

# define protocol_parseACK() protocol_isEndOfTrame()
 
 extern char const protocol_trameId[cProtocolTrameNumber][PROTOCOL_TRAME_TYPE_SIZE + 1];

 extern char (*protocol_readChar)(void);

# ifdef __cplusplus
  extern "C"{
# endif

 eProtocolTrame protocol_trameIdentification(char const buffer[PROTOCOL_TRAME_TYPE_SIZE]);
 bool protocol_parseYOP(uint8_t* fsrNumber, uint8_t* fscNumber);
 bool protocol_parseSYN(uint32_t* timeData);
 bool protocol_parseERR(eProtocolError* errNum);
 bool protocol_parseMOD(eProtocolMode* modeNum);
 bool protocol_parseDR1(struct sProtocolDR1* sDr1);
 bool protocol_parseDC1(struct sProtocolDC1* sDc1);
 bool protocol_parseDCN(struct sProtocolDCN* sDcn);
 bool protocol_parseDA1(struct sProtocolDA1* sDa1);
 bool protocol_parseDAN(struct sProtocolDAN* sDan);
 
 uint16_t protocol_createACK(uint8_t buffer[PROTOCOL_ACK_SIZE]);
 uint16_t protocol_createYOP(uint8_t buffer[PROTOCOL_YOP_SIZE]);
 uint16_t protocol_createSYN(const uint32_t timeData, uint8_t buffer[PROTOCOL_SYN_SIZE]);
 uint16_t protocol_createERR(const eProtocolError errNum, uint8_t buffer[PROTOCOL_ERR_SIZE]);
 uint16_t protocol_createMOD(const eProtocolMode modeNum, uint8_t buffer[PROTOCOL_MOD_SIZE]);
 uint16_t protocol_createDR1(struct sProtocolDR1 const* sDr1, uint8_t buffer[PROTOCOL_DR1_SIZE]);
 uint16_t protocol_createDC1(struct sProtocolDC1 const* sDc1, uint8_t buffer[PROTOCOL_DC1_SIZE]);
 uint16_t protocol_createDA1(struct sProtocolDA1 const* sDa1, uint8_t buffer[PROTOCOL_DA1_SIZE]);


 uint16_t protocol_createDCN(struct sProtocolDCN const* sDcn, uint8_t buffer[PROTOCOL_DATA_SIZE_MAX]);
 uint16_t protocol_initDCN(struct sProtocolDC1 const* sDc1, const uint32_t delta, uint8_t buffer[PROTOCOL_DCN_MIN_SIZE - PROTOCOL_TRAME_END]);
 uint16_t protocol_extendDCN(uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DCN_VAR_SIZE]);
 uint16_t protocol_endDCN(uint8_t buffer[PROTOCOL_TRAME_END_SIZE]);


 uint16_t protocol_createDAN(struct sProtocolDAN const* sDan, uint8_t buffer[PROTOCOL_DATA_SIZE_MAX]);
 uint16_t protocol_initDAN(struct sProtocolDA1 const* sDa1, const uint32_t delta, uint8_t buffer[PROTOCOL_DAN_MIN_SIZE - PROTOCOL_TRAME_END]);
 uint16_t protocol_extendDAN(uint16_t const fsrValues[PROTOCOL_FSR_NUMBER], uint16_t const fscValues[PROTOCOL_FSC_NUMBER], uint8_t buffer[PROTOCOL_DAN_VAR_SIZE]);
 inline uint16_t protocol_endDAN(uint8_t buffer[PROTOCOL_TRAME_END_SIZE]);
 
# ifdef __cplusplus
  }
# endif
#endif
