//******************************************************************
// PROPRIETARY DATA NOTICE
// Copyright @ Carré Technologies inc.
// All Right Reserved. No part of this file shall be used,
// communicated, reproduced or copied in any form or by any means
// without the prior written permission of Carré Technologies inc. 
//******************************************************************
//
// Description:
// Déclaration des buffers avec pointeurs lecture et écriture
// 
//******************************************************************
#include "byte_buffer.h"
#include "string.h"
#include "utilities.h"

/* BYTE BUFFER */
void byte_buffer_init(byte_buffer_t * buffer, uint8_t *ptr, size_t capacity) {
  buffer->buffer = ptr;
  buffer->capacity = capacity;
  byte_buffer_reset(buffer);
}

//Return 1 when worked ok
int16_t byte_buffer_add(byte_buffer_t * buffer, uint8_t byte) {
  
  if(buffer_full(buffer)) { 
    //Flag!
    buffer->error_flag++;
    return 0; //Buffer is full, cannot add!
  }

  //Adding element
  buffer->buffer[buffer->wr_ptr] = byte;
  buffer_incrWr(buffer);

  //Statistics
  buffer->count++;

  return 1;
}

int16_t byte_buffer_addmultiple(byte_buffer_t * buffer, uint8_t *bytes, size_t len){
   size_t end;
   size_t left = len;

  //Validating that we have room
  if (left <= buffer_space_available(buffer)) {
    //Copying
    end = (buffer->rd_ptr > buffer->wr_ptr ? buffer->rd_ptr - buffer->wr_ptr : buffer->capacity - buffer->wr_ptr);
    end = MIN(end, left);
    memcpy(buffer->buffer + buffer->wr_ptr, bytes, end);
    left -= end;
    if(left) {
      bytes += end;
      memcpy(buffer->buffer, bytes, left);
    }
    
    //Ptr
    buffer_incrWr_N(buffer,len);
    //Statistics
    buffer->count += len;

    //Worked
    return (int16_t)len;
  } else { 
    return 0;
  }
}

int16_t byte_buffer_get(byte_buffer_t *buffer, uint8_t *destByte) {
  
  //Getting
  if (!buffer_empty(buffer)) {
    //Taking a byte.
    *destByte = buffer->buffer[buffer->rd_ptr];
    buffer_incrRd(buffer);
    //Updating here because normally less time critical
    buffer->max = MAX(buffer->max, buffer->count);
    //Statistics
    buffer->count--;
    return 1;
  } else {
    return 0;
  }
}

int16_t byte_buffer_peek(byte_buffer_t *buffer, uint8_t *destByte, size_t delta ) {
  size_t n;
  size_t rd_ptr;
  //Validating it exists
  n = buffer_data_available(buffer);
  if(n == 0 || delta >= n) return -1;

  //Getting ptr address
  rd_ptr = (buffer->rd_ptr + delta); 
  if(rd_ptr > buffer_usableCap(buffer)) { rd_ptr -= buffer->capacity; }
  
  *destByte = buffer->buffer[rd_ptr];
  return 0;
}


// TODO Not tested. Use with caution.
int16_t byte_buffer_getmultiple(byte_buffer_t * buffer, uint8_t *destBytes, size_t len) {
  size_t end;
  size_t left = len;
  uint8_t *ptr = destBytes;
  if(len <= buffer_data_available(buffer)) {
    //Finding the number of elements at the end
    end = (buffer->rd_ptr < buffer->wr_ptr ? buffer->wr_ptr - buffer->rd_ptr : buffer->capacity - buffer->rd_ptr);
    end = MIN(end, left);
    memcpy(ptr, buffer->buffer + buffer->rd_ptr, end);
  
    left -= end;
    if (left) {
      ptr += end;
      memcpy(ptr, buffer->buffer, left);
    }
    
    //Ptr
    buffer_incrRd_N(buffer,len);
    //Updating here because normally less time critical
    buffer->max = MAX(buffer->max, buffer->count);
    //Statistics
    buffer->count -= len;
    //Worked
    return (int16_t)len;
  } else { return 0; }
}

/*! return the number of samples in the buffer */
uint32_t buffer_data_available(byte_buffer_t* b)  {
    uint32_t n;
    size_t wr_ptr = (b)->wr_ptr;
    if (wr_ptr >= (b)->rd_ptr) {
        n = wr_ptr - (b)->rd_ptr;
    } else {
        n = wr_ptr - (b)->rd_ptr + (b)->capacity;
    }
    return n;
}
//******************************************************************
//word BUFFER
void word_buffer_init(word_buffer_t * buffer, uint16_t *ptr, size_t capacity) {
  buffer->buffer = ptr;
  buffer->capacity = capacity;
  word_buffer_reset(buffer);
}

//Adding
int16_t word_buffer_add(word_buffer_t * buffer, uint16_t word) {
  if(buffer_full(buffer)) { 
    //Flag!
    buffer->error_flag++;
    return 0; //Buffer is full, cannot add!
  }

  //Adding element
  buffer->buffer[buffer->wr_ptr] = word;
  buffer_incrWr(buffer);

  //Statistics
  buffer->count++;

  return 1;
}

int16_t word_buffer_addmultiple(word_buffer_t *buffer, uint16_t *words, size_t len) {
   size_t end;
   size_t left = len;

  //Validating that we have room
  if (left <= buffer_space_available(buffer)) {
    //Copying
    end = (buffer->rd_ptr > buffer->wr_ptr ? buffer->rd_ptr - buffer->wr_ptr : buffer->capacity - buffer->wr_ptr);
    end = MIN(end, left);
    memcpy(buffer->buffer + buffer->wr_ptr, words, (end << 1));
    left -= end;
    if(left) {
      words += end;
      memcpy(buffer->buffer, words, (left << 1));
    }
    
    //Ptr
    buffer_incrWr_N(buffer,len);
    //Statistics
    buffer->count += len;
    //Worked
    return (int16_t)len;
  } else { 
    return 0;
  }
}

//Get / peek
int16_t word_buffer_get(word_buffer_t * buffer, uint16_t *destWord) {
  //Getting
  if (!buffer_empty(buffer)) {
    //Taking a byte.
    *destWord = buffer->buffer[buffer->rd_ptr];
    buffer_incrRd(buffer);
    //Updating here because normally less time critical
    buffer->max = MAX(buffer->max, buffer->count);
    //Statistics
    buffer->count--;
    return 1;
  } else {
    return 0;
  }
}

int16_t word_buffer_getmultiple(word_buffer_t * buffer, uint16_t *destWords, size_t len) {
  size_t end;
  size_t left = len;
  uint16_t *ptr = destWords;
  if(len <= buffer_data_available((byte_buffer_t*)buffer)) {
    //Finding the number of elements at the end
    end = (buffer->rd_ptr < buffer->wr_ptr ? buffer->wr_ptr - buffer->rd_ptr : buffer->capacity - buffer->rd_ptr);
    memcpy(ptr, buffer->buffer + buffer->rd_ptr, (end << 1));
  
    left -= end;
    if (left) {
      ptr += end;
      memcpy(ptr, buffer->buffer, (left << 1));
    }
    
    //Ptr
    buffer_incrRd_N(buffer,len);
    //Updating here because normally less time critical
    buffer->max = MAX(buffer->max, buffer->count);
    //Statistics
    buffer->count -= len;
    //Worked
    return (int16_t)len;
  } else { return 0; }
}

int16_t word_buffer_peek(word_buffer_t *buffer, uint16_t *destWord, size_t delta) {
  size_t n;
  size_t rd_ptr;
  //Validating it exists
  n = buffer_data_available((byte_buffer_t*)buffer);
  if(n == 0 || delta >= n) return -1;

  //Getting ptr address
  rd_ptr = (buffer->rd_ptr + delta); 
  if(rd_ptr > buffer_usableCap(buffer)) { rd_ptr -= buffer->capacity; }
  
  *destWord = buffer->buffer[rd_ptr];
  return 0;
}

