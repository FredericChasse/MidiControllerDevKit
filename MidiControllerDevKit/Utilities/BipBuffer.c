/*
 * Copyright (c) 2011, Willem-Hendrik Thiart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The names of its contributors may not be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file
 * @author  Willem Thiart himself@willemthiart.com
 */

#include "BipBuffer.h"
//#include "stdio.h"
//#include <stdlib.h>

/* for memcpy */
#include <string.h>

/**
 * @return bytes of unused space */
int bipbuf_unused(const BipBuffer_t* me)
{
  /* distance between region B and region A */
  int ba;
  /* distance between end of region A and end of array */
  int aend;
  /* usable size */
  int size;

  if (1 == me->b_inuse)
  {
    ba = me->a_start - me->b_end;
    return ba;
  }
  else
  {
    ba = me->a_start;
  }

  aend = me->size - me->a_end;

  size = aend >= ba ? aend : ba;  // Return the larger of both sizes
  return size;

//    /* distance between region B and region A */
//    int ab = me->a_start - me->b_end;
//    if (1 == me->b_inuse)
//        /* distance between region B and region A */
//        return me->a_start - me->b_end;
//    else
//        return me->size - me->a_end;
}

int bipbuf_size(const BipBuffer_t* me)
{
    return me->size;
}

/**
 * @return how much space we have assigned */
int bipbuf_used(const BipBuffer_t* me)
{
    return (me->a_end - me->a_start) + me->b_end;
}

/**
 * Initialise a bip buffer. Use memory provided by user.
 *
 * No malloc()s are performed.
 *
 * @param[in] array Pointer to the start of the array
 * @param[in] size The size of the array */
void bipbuf_init(BipBuffer_t* me, unsigned char *array, const unsigned int size)
{
    me->array = array;
    me->a_start = me->a_end = me->b_end = 0;
    me->size = size;
    me->b_inuse = 0;
}

/**
 * @return 1 if buffer is empty; 0 otherwise */
int bipbuf_is_empty(const BipBuffer_t* me)
{
    return me->a_start == me->a_end;
}

/* find out if we should turn on region B
 * ie. is the distance from A to buffer's end less than B to A? */
static void __check_for_switch_to_b(BipBuffer_t* me)
{
    if (me->size - me->a_end < me->a_start - me->b_end)
        me->b_inuse = 1;
}

/**
 * @param[in] data The data to be offered to the buffer
 * @param[in] size The size of the data to be offered
 * @return number of bytes offered */
int bipbuf_offer(BipBuffer_t* me, const unsigned char *data, const int size)
{
    /* not enough space */
    if (bipbuf_unused(me) < size)
        return 0;

    if (1 == me->b_inuse)
    {
        memcpy(me->array + me->b_end, data, size);
        me->b_end += size;
    }
    else
    {
        /* if partition A can't accept more data, switch to partition B */
        if (size > (me->size - me->a_end))
        {
            __check_for_switch_to_b(me);
            memcpy(me->array + me->b_end, data, size);
            me->b_end += size;
        }
        else
        {
            memcpy(me->array + me->a_end, data, size);
            me->a_end += size;
        }
    }

//    __check_for_switch_to_b(me);
    return size;
}

/**
 * Look at data. Don't move cursor
 *
 * @param[in] len The length of the data to be peeked
 * @return data on success, NULL if we can't peek at this much data */
unsigned char *bipbuf_peek(const BipBuffer_t* me, const unsigned int size)
{
    /* make sure we can actually peek at this data */
    if (me->size < me->a_start + size)
        return NULL;

    if (bipbuf_is_empty(me))
        return NULL;

    return (unsigned char*)me->array + me->a_start;
}

/**
 * Get pointer to data to read. Move the cursor on.
 *
 * @param[in] len The length of the data to be polled
 * @return pointer to data, NULL if we can't poll this much data */
unsigned char *bipbuf_poll(BipBuffer_t* me, const unsigned int size)
{
    if (bipbuf_is_empty(me))
        return NULL;

    /* make sure we can actually poll this data */
    if (me->size < me->a_start + size)
        return NULL;

    void *end = me->array + me->a_start;
    me->a_start += size;

    /* we seem to be empty.. */
    if (me->a_start == me->a_end)
    {
        /* replace a with region b */
        if (1 == me->b_inuse)
        {
            me->a_start = 0;
            me->a_end = me->b_end;
            me->b_end = me->b_inuse = 0;
        }
        else
            /* safely move cursor back to the start because we are empty */
            me->a_start = me->a_end = 0;
    }

//    __check_for_switch_to_b(me);
    return end;
}

/**
 * Reset a bip buffer.
 */
void bipbuf_reset(BipBuffer_t* me)
{
  me->a_start = me->a_end = me->b_end = 0;
  me->b_inuse = 0;
}
