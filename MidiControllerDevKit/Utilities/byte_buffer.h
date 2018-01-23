/*! \file byte_buffer.h
 *  The circular buffers (byte and word).
 *  \copyright 2011 Carré Technologies inc.
 *  \author Jean-Francois Roy and Charles Robillard.
 *  \version 2.0 Version with byte_buffer_t and word_buffer_t.
 *  \version 1.0 Initial version
 *  \note The usable amount of bytes if capacity-1 since both 
 *  rd_ptr and wr_ptr can't overlap when full.
 */
#ifndef _BYTE_BUFFER_H
#define _BYTE_BUFFER_H

#include "stddef.h"
#include "stdint.h"

/******************************************************************/
/*** General macro definitions ***/
/*! Getting the effective usable capacity */
#define buffer_usableCap(b)       ( (b)->capacity - 1 )
/*! Tell if the buffer is empty */
#define buffer_empty(b)           ( (b)->wr_ptr == (b)->rd_ptr )
/*! Tell if the buffer is full */
#define buffer_full(b)            ( ((b)->count == (b)->capacity) ? ((b)->rd_ptr == 0) : (((b)->wr_ptr + 1) == (b)->rd_ptr) )
/*! Tell the number of free place in the buffer */
#define buffer_space_available(b) ( buffer_usableCap(b) - buffer_data_available((byte_buffer_t*)b) )
/*! Reset  any buffer */
#define buffer_reset(b)           ( (b)->rd_ptr = (b)->wr_ptr = (b)->error_flag  = (b)->max = (b)->count = 0 )
/*! Add fast method, use safely */
#define buffer_add_fast(b, el)    { (b)->buffer[(b)->wr_ptr] = el; buffer_incrWr(b); (b)->count++; }
/*! Get the buffer error flag */
#define buffer_error_get(b)       ( (b)->error_flag )
/*! Reset the buffer error flag */
#define buffer_error_clr(b)       ( (b)->error_flag = 0 )

#define buffer_addloop_fast(buffer, els, len) {     \
    size_t __i = 0;                                 \
    do {                                            \
      buffer_add_fast(buffer, els[__i]);            \
    } while (++__i < len);                          \
  }

/*** Utilities ***/
/*! Check if wr and rd pointer are pointing to the same index. */
#define buffer_rd_eq_wr(b)    ( (b)->wr_ptr == (b)->rd_ptr ? 1 : 0 )
/*! Increasing the wr pointer. */
#define buffer_incrWr(b)      ( (b)->wr_ptr = ((b)->wr_ptr >= buffer_usableCap(b)) ? 0 : (b)->wr_ptr + 1 )
/*! Increasing the rd pointer. */
#define buffer_incrRd(b)      ( (b)->rd_ptr = ((b)->rd_ptr >= buffer_usableCap(b)) ? 0 : (b)->rd_ptr + 1 )
/*! Increasing the wr pointer of N values. */
#define buffer_incrWr_N(b,N)  {                                       \
  size_t addr = (b)->wr_ptr + N;                                      \
  addr = (addr > buffer_usableCap(b) ? addr - (b)->capacity : addr);  \
  (b)->wr_ptr = addr;                                                 \
  }
/*! Increasing the rd pointer of N values. */
#define buffer_incrRd_N(b,N)  {                                       \
  size_t addr = (b)->rd_ptr + N;                                      \
  addr = (addr > buffer_usableCap(b) ? addr - (b)->capacity : addr);  \
  (b)->rd_ptr = addr;                                                 \
  }

/******************************************************************/
/*** Byte buffer ***/
typedef struct {
  uint8_t *buffer;      /*!< The buffer array. >!*/
  //Indexes
  size_t rd_ptr;        /*!< The read pointer. >!*/
  size_t wr_ptr;        /*!< The write pointer. >!*/
  //n elements      
  size_t capacity;      /*!< The capacity of the buffer array in bytes. >!*/
  //for statistics    
  size_t max;           /*!< The maximum number of bytes that was in the buffer. >!*/
  uint16_t error_flag;  /*!< A flag that states the number of add error since last clr. >!*/
  size_t count;         /*!< The actual number of bytes in the circular buffer. >!*/
} byte_buffer_t; /*!< Structure of a circular buffer for a byte. >!*/

//// Init / reset
/*! Function that will initialize a byte_buffer_t structure.
  * \param buffer A ptr to the buffer to initialize.
  * \param ptr The buffer to use to store the bytes.
  * \param capacity The length of the buffer array ptr passed as the second argument.
  */
void byte_buffer_init(byte_buffer_t * buffer, uint8_t *ptr, size_t capacity);

/*! Macro that will reset the ptrs and statistics of the given buffer. */
#define byte_buffer_reset(b)  buffer_reset(b)

//// Adding
/*! Function that will add a single element to the buffer.
  * \param buffer A pointer to the buffer to add to.
  * \param byte The byte to add.
  * \return The number of bytes added (0 if failed, 1 on success).
  */
int16_t byte_buffer_add(byte_buffer_t * buffer, uint8_t byte);

/*! Function that will add multiple elements to the buffer.
  * \param buffer A pointer to the buffer to add to.
  * \param bytes The bytes to add.
  * \param len The number of bytes to add.
  * \return The number of bytes added (0 if failed, len on success).
  * \note This function will not add partial data to the buffer, either
  * all of the array is added or nothing.
  */
int16_t byte_buffer_addmultiple(byte_buffer_t *buffer, uint8_t *bytes, size_t len);

/*! Macro to add multiple elements using a loop instead of the memcpy feature.
  * \param buffer A pointer to the buffer to add to.
  * \param bytes The bytes to add.
  * \param len The number of bytes to add.
  * \note This methods should be use for small value of len.
  */
#define byte_buffer_addloop(buffer, els, len) {     \
    size_t __i = 0;                                 \
    do {                                            \
      byte_buffer_add(buffer, els[__i]);            \
    } while (++__i < len);                          \
  }

////Get / peek
/*! Function that will get a single element from the buffer.
  * \param buffer A pointer to the buffer to get from.
  * \param destByte A pointer to a byte where to store the response.
  * \return The number of bytes returned (0 if failed, 1 on success).
  */
int16_t byte_buffer_get(byte_buffer_t * buffer, uint8_t *destByte);

/*! Function that will get multiple elements from the buffer.
  * \param buffer A pointer to the buffer to get from.
  * \param destBytes A pointer to an array of bytes where to store the response.
  * \param len The number of bytes to get.
  * \return The number of bytes returned (0 if failed, len on success).
  * \note This function will not get partial data from the buffer, either
  * all of the array is returned or nothing.
  */
int16_t byte_buffer_getmultiple(byte_buffer_t * buffer, uint8_t *destBytes, size_t len);

/*! Function that will peek a single element from the buffer.
  * \param buffer A pointer to the buffer to peek from.
  * \param destByte A pointer to a byte where to store the response.
  * \param delta The index from the current rd_ptr to peek at.
  * \return The number of bytes peeked (0 if failed, 1 on success).
  */
int16_t byte_buffer_peek(byte_buffer_t *buffer, uint8_t *destByte, size_t delta);

/*! return the number of samples in the buffer
 * \param buffer A pointer to the target buffer.
 */
uint32_t buffer_data_available(byte_buffer_t* b);


// Signed byte buffer
typedef byte_buffer_t sbyte_buffer_t;

// Macros for sbyte_buffer_t
#define sbyte_buffer_init(b, ptr, capacity)           byte_buffer_init(b, (uint8_t*)ptr, capacity)
#define sbyte_buffer_reset(b)                         byte_buffer_reset(b)
#define sbyte_buffer_add(b, byte)                     byte_buffer_add(b, (uint8_t)byte)
#define sbyte_buffer_addmultiple(b, bytes, len)       byte_buffer_addmultiple(b, (uint8_t*)bytes, len)
#define sbyte_buffer_addloop(buffer, els, len) {  \
    size_t __i = 0;                               \
    do {                                          \
      sbyte_buffer_add(buffer, els[__i]);         \
    } while (++__i < len);                        \
  }
#define sbyte_buffer_get(b, destByte)                 byte_buffer_get(b, (uint8_t*)destByte)
#define sbyte_buffer_getmultiple(b, destBytes, len)   byte_buffer_getmultiple(b, (uint8_t*)destBytes, len)
#define sbyte_buffer_peek(b, destByte, delta)         byte_buffer_peek(b, (uint8_t*)destByte, delta)


//******************************************************************
// Word buffer

typedef struct {
  uint16_t *buffer;     /*!< The buffer array. >!*/
  //Indexes
  size_t rd_ptr;        /*!< The read pointer. >!*/
  size_t wr_ptr;        /*!< The write pointer. >!*/
  //n elements      
  size_t capacity;      /*!< The capacity of the buffer array in word. >!*/
  //for statistics    
  size_t max;           /*!< The maximum number of words that was in the buffer. >!*/
  uint16_t error_flag;  /*!< A flag that states the number of add error since last clr. >!*/
  size_t count;         /*!< The actual number of words in the circular buffer. >!*/
} word_buffer_t; /*!< Structure of a circular buffer for a word. >!*/

//// Init / reset
/*! Function that will initialize a word_buffer_t structure.
  * \param buffer A ptr to the buffer to initialize.
  * \param ptr The buffer to use to store the words.
  * \param capacity The length in word of the buffer array ptr passed as the second argument.
  */
void word_buffer_init(word_buffer_t * buffer, uint16_t *ptr, size_t capacity);

/*! Macro that will reset the ptrs and statistics of the given buffer. */
#define word_buffer_reset(b)  buffer_reset(b);

//// Adding
/*! Function that will add a single element to the buffer.
  * \param buffer A pointer to the buffer to add to.
  * \param word The word to add.
  * \return The number of words added (0 if failed, 1 on success).
  */
int16_t word_buffer_add(word_buffer_t * buffer, uint16_t word);

/*! Function that will add multiple elements to the buffer.
  * \param buffer A pointer to the buffer to add to.
  * \param words The words to add.
  * \param len The number of words to add.
  * \return The number of words added (0 if failed, len on success).
  * \note This function will not add partial data to the buffer, either
  * all of the array is added or nothing.
  */
int16_t word_buffer_addmultiple(word_buffer_t *buffer, uint16_t *words, size_t len);

/*! Macro to add multiple elements using a loop instead of the memcpy feature.
  * \param buffer A pointer to the buffer to add to.
  * \param bytes The bytes to add.
  * \param len The number of bytes to add.
  * \note This methods should be use for small value of len.
  */
#define word_buffer_addloop(buffer, els, len) {     \
    size_t __i = 0;                                 \
    do {                                            \
      word_buffer_add(buffer, els[__i]);            \
    } while (++__i < len);                          \
  }

////Get / peek
/*! Function that will get a single element from the buffer.
  * \param buffer A pointer to the buffer to get from.
  * \param destWord A pointer to a word where to store the response.
  * \return The number of words returned (0 if failed, 1 on success).
  */
int16_t word_buffer_get(word_buffer_t * buffer, uint16_t *destWord);

/*! Function that will get multiple elements from the buffer.
  * \param buffer A pointer to the buffer to get from.
  * \param destWords A pointer to an array of words where to store the response.
  * \param len The number of words to get.
  * \return The number of words returned (0 if failed, len on success).
  * \note This function will not get partial data from the buffer, either
  * all of the array is returned or nothing.
  */
int16_t word_buffer_getmultiple(word_buffer_t * buffer, uint16_t *destWords, size_t len);

/*! Function that will peek a single element from the buffer.
  * \param buffer A pointer to the buffer to peek from.
  * \param destWord A pointer to a word where to store the response.
  * \param delta The index from the current rd_ptr to peek at.
  * \return The number of words peeked (0 if failed, 1 on success).
  */
int16_t word_buffer_peek(word_buffer_t *buffer, uint16_t *destWord, size_t delta);

// Signed word buffer.
typedef word_buffer_t sword_buffer_t; 

// Macros for sword_buffer_t
#define sword_buffer_init(b, ptr, capacity)           word_buffer_init(b, (uint16_t*)ptr, capacity)
#define sword_buffer_reset(b)                         word_buffer_reset(b)
#define sword_buffer_add(b, word)                     word_buffer_add(b, (uint16_t)word)
#define sword_buffer_addmultiple(b, words, len)       word_buffer_addmultiple(b, (uint16_t*)words, len)
#define sword_buffer_addloop(buffer, els, len) {  \
    size_t __i = 0;                               \
    do {                                          \
      sword_buffer_add(buffer, els[__i]);         \
    } while (++__i < len);                        \
  }
#define sword_buffer_get(b, destWord)                 word_buffer_get(b, (uint16_t*)destWord)
#define sword_buffer_getmultiple(b, destWords, len)   word_buffer_getmultiple(b, (uint16_t*)destWords, len)
#define sword_buffer_peek(b, destWord, delta)         word_buffer_peek(b, (uint16_t*)destWord, delta)

#endif
