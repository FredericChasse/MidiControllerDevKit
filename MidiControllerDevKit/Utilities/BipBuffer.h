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
 */

#ifndef __BIPBUFFER_H__
#define __BIPBUFFER_H__

typedef struct
{
    unsigned long int size;

    /* region A */
    unsigned int a_start, a_end;

    /* region B */
    unsigned int b_end;

    /* is B inuse? */
    int b_inuse;

    unsigned char *array;
} BipBuffer_t;

/**
 * Initialise a bip buffer. Use memory provided by user.
 *
 * No malloc()s are performed.
 *
 * @param[in] array Pointer to the start of the array
 * @param[in] size The size of the array */
void bipbuf_init(BipBuffer_t* me, unsigned char *array,  const unsigned int size);

/**
 * @param[in] data The data to be offered to the buffer
 * @param[in] size The size of the data to be offered
 * @return number of bytes offered */
int bipbuf_offer(BipBuffer_t *me, const unsigned char *data, const int size);

/**
 * Look at data. Don't move cursor
 *
 * @param[in] len The length of the data to be peeked
 * @return data on success, NULL if we can't peek at this much data */
unsigned char *bipbuf_peek(const BipBuffer_t* me, const unsigned int len);

/**
 * Get pointer to data to read. Move the cursor on.
 *
 * @param[in] len The length of the data to be polled
 * @return pointer to data, NULL if we can't poll this much data */
unsigned char *bipbuf_poll(BipBuffer_t* me, const unsigned int size);

/**
 * @return 1 if buffer is empty; 0 otherwise */
int bipbuf_is_empty(const BipBuffer_t* me);

/**
 * @return how much space we have assigned */
int bipbuf_used(const BipBuffer_t* cb);

/**
 * @return bytes of unused space */
int bipbuf_unused(const BipBuffer_t* me);

/**
 * Reset a bip buffer.
 */
void bipbuf_reset(BipBuffer_t* me);

#endif /* __BIPBUFFER_H__ */
