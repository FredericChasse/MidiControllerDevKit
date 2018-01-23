/*
 * utilities.c
 *
 *  Created on: Sep 12, 2017
 *      Author: Marc
 */


#include "utilities.h"

uint64_t Bytes6ToU64LE(const uint8_t* byteArray)
{
   uint64_t ret = 0;
   ret  = (uint64_t) byteArray[5] << 40;
   ret += (uint64_t) byteArray[4] << 32;
   ret += (uint64_t) byteArray[3] << 24;
   ret += (uint64_t) byteArray[2] << 16;
   ret += (uint64_t) byteArray[1] << 8;
   ret += (uint64_t) byteArray[0];
   return ret;

}

