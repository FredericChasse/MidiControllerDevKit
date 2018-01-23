//******************************************************************
// Copyright 2010  Carré Technologies inc.
//
//
// UTILITY DEFINES
//
//******************************************************************

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <stdint.h> 

/* Note: All references to these macros must not have equations in them, as the preprocessor will duplicate the equation.
  ex: ABS(x*y) == (((x*y) < 0) ? -(x*y) : (x*y)) ==> (x*y) computed two times instead of one
  ex: MAX(i++, 4) == (i++) > (4) ? (i++) : (4) ==> i is incremented 2 times instead of one.
*/

//Utilities
#ifndef MAX
  #define MAX(_maxx,_maxy) (_maxx) > (_maxy) ? (_maxx) : (_maxy)
#endif
#ifndef MIN
  #define MIN(_minx,_miny) (_minx) > (_miny) ? (_miny) : (_minx)
#endif
#ifndef ABS
  #define ABS(_absx) (((_absx) < 0) ? -(_absx) : (_absx))
#endif

#define REG_SETBITS(reg, bitmask)       (reg |= bitmask)
#define REG_CLRBITS(reg, bitmask)       (reg &= ~bitmask)

// Convert Byte array to 16, 32 and 64 bits integer using *big* endian byte order.
#define BytesToU16BE(B) \
      (uint16_t)((B)[0]) << 8 \
    | (uint16_t)((B)[1])

#define BytesToU32BE(B) \
      (uint32_t)((B)[0]) << 24 \
    | (uint32_t)((B)[1]) << 16 \
    | (uint32_t)((B)[2]) << 8  \
    | (uint32_t)((B)[3])

#define BytesToU64BE(B) \
      (uint64_t)((B)[0]) << 56 \
    | (uint64_t)((B)[1]) << 48 \
    | (uint64_t)((B)[2]) << 40 \
    | (uint64_t)((B)[3]) << 32 \
    | (uint64_t)((B)[4]) << 24 \
    | (uint64_t)((B)[5]) << 16 \
    | (uint64_t)((B)[6]) << 8  \
    | (uint64_t)((B)[7])

// Convert Byte array to 16, 32 and 64 bits integer using *little* endian byte order.
#define BytesToU16LE(B) \
      (uint16_t)((B)[1]) << 8 \
    | (uint16_t)((B)[0])

#define Bytes3ToU32BE(B) \
      (uint32_t)((B)[0]) << 16 \
    | (uint32_t)((B)[1]) << 8 \
    | (uint32_t)((B)[2])

#define BytesToU32LE(B) \
      (uint32_t)((B)[3]) << 24 \
    | (uint32_t)((B)[2]) << 16 \
    | (uint32_t)((B)[1]) << 8  \
    | (uint32_t)((B)[0])

#define BytesToU64LE(B) \
      (uint64_t)((B)[7]) << 56 \
    | (uint64_t)((B)[6]) << 48 \
    | (uint64_t)((B)[5]) << 40 \
    | (uint64_t)((B)[4]) << 32 \
    | (uint64_t)((B)[3]) << 24 \
    | (uint64_t)((B)[2]) << 16 \
    | (uint64_t)((B)[1]) << 8  \
    | (uint64_t)((B)[0])

//Used for hexoskin_time_t transmitted from protocol.
uint64_t Bytes6ToU64LE(const uint8_t* byteArray);



// Convert 16, 32 and 64 bits integer to byte array using *big* endian byte order.
#define U16BEToBytes(out, in) \
    do {\
        (out)[0] = (uint8_t)(in >> 8); \
        (out)[1] = (uint8_t)(in);      \
    } while(0);

#define U32BEToBytes(out, in) \
    do {\
        (out)[0] = (uint8_t)((in) >> 24); \
        (out)[1] = (uint8_t)((in) >> 16); \
        (out)[2] = (uint8_t)((in) >>  8); \
        (out)[3] = (uint8_t)(in);         \
    } while(0);

#define U64BEToBytes(out, in) \
    do {\
        (out)[0] = (uint8_t)((in) >> 56); \
        (out)[1] = (uint8_t)((in) >> 48); \
        (out)[2] = (uint8_t)((in) >> 40); \
        (out)[3] = (uint8_t)((in) >> 32); \
        (out)[4] = (uint8_t)((in) >> 24); \
        (out)[5] = (uint8_t)((in) >> 16); \
        (out)[6] = (uint8_t)((in) >>  8); \
        (out)[7] = (uint8_t)(in);         \
    } while(0);

#define U64BEToBytes6(out, in) \
    do {\
        (out)[0] = (uint8_t)((in) >> 40); \
        (out)[1] = (uint8_t)((in) >> 32); \
        (out)[2] = (uint8_t)((in) >> 24); \
        (out)[3] = (uint8_t)((in) >> 16); \
        (out)[4] = (uint8_t)((in) >> 8); \
        (out)[5] = (uint8_t)(in); \
    } while(0);

// Convert 16, 32 and 64 bits integer to byte array using *little* endian byte order.
#define U16LEToBytes(out, in) \
    do {\
        (out)[1] = (uint8_t)(in >> 8); \
        (out)[0] = (uint8_t)(in);      \
    } while(0);

#define U32LEToBytes(out, in) \
    do {\
        (out)[3] = (uint8_t)((in) >> 24); \
        (out)[2] = (uint8_t)((in) >> 16); \
        (out)[1] = (uint8_t)((in) >>  8); \
        (out)[0] = (uint8_t)(in);         \
    } while(0);

#define U64LEToBytes(out, in) \
    do {\
        (out)[7] = (uint8_t)((in) >> 56); \
        (out)[6] = (uint8_t)((in) >> 48); \
        (out)[5] = (uint8_t)((in) >> 40); \
        (out)[4] = (uint8_t)((in) >> 32); \
        (out)[3] = (uint8_t)((in) >> 24); \
        (out)[2] = (uint8_t)((in) >> 16); \
        (out)[1] = (uint8_t)((in) >>  8); \
        (out)[0] = (uint8_t)(in);         \
    } while(0);

#define U64LEToBytes6(out, in) \
    do {\
        (out)[5] = (uint8_t)((in) >> 40); \
        (out)[4] = (uint8_t)((in) >> 32); \
        (out)[3] = (uint8_t)((in) >> 24); \
        (out)[2] = (uint8_t)((in) >> 16); \
        (out)[1] = (uint8_t)((in) >> 8);  \
        (out)[0] = (uint8_t)(in);         \
    } while(0);

// Define some LFSR macros for 8, 16 and 32 bits to help in generating random data.

//taps: 32 31 29 1; feedback polynomial: x^32 + x^31 + x^29 + x + 1
//The 1+~... performs a 2's complement to get the negative number.
//Using the - unary operator was causing a compiler warning.
#define LFSR32(lfsr_in) ((uint32_t)(lfsr_in) >> 1) ^ (1+~((uint32_t)(lfsr_in) & UINT32_C(1)) & UINT32_C(0xD0000001));

//taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1
#define LFSR16(lfsr_in) ((uint16_t)(lfsr_in) >> 1) ^ (1+~((uint16_t)(lfsr_in) & UINT16_C(1)) & UINT16_C(0xB400));

//taps: 8 6 5 4; feedback polynomial: x^8 + x^6 + x^5 + x^4 + 1
#define LFSR8(lfsr_in) ((uint8_t)(lfsr_in) >> 1) ^ (1+~((uint8_t)(lfsr_in) & (uint8_t)(1)) & (uint8_t)(0xB8));

#define LFSR32_ARR(buf, count, lfsr) \
do { \
    int i; \
    *(lfsr) = LFSR32(*(lfsr)); \
    for (i = 0; i < count; i++) { \
        *(((uint32_t*)(buf))+i) = *(lfsr); \
        *(lfsr) = LFSR32(*(lfsr)); \
    } \
} while(0);

#define LFSR16_ARR(buf, count, lfsr) \
do { \
    int i; \
    *(lfsr) = LFSR16(*(lfsr)); \
    for (i = 0; i < count; i++) { \
        *(((uint16_t*)(buf))+i) = *(lfsr); \
        *(lfsr) = LFSR16(*(lfsr)); \
    } \
} while(0);

#define LFSR8_ARR(buf, count, lfsr) \
do { \
    int i; \
    *(lfsr) = LFSR8(*(lfsr)); \
    for (i = 0; i < count; i++) { \
        *(((uint8_t*)(buf))+i) = *(lfsr); \
        *(lfsr) = LFSR8(*(lfsr)); \
    } \
} while(0);

#define LOCKING_CALL(__status, __fct_call)    \
  do {                                        \
    __status = __fct_call;                    \
  } while ( __status == 0 );


// Blocking delay of ms x 1ms at 19.2MHz
#define delayms(ms)    \
do { \
    unsigned long i; \
    const unsigned long end = (ms); \
    for (i = end; i > 0; --i) { \
        __delay_cycles(19200); \
    } \
} while(0);

#endif
