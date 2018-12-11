/*
 * File: ring_buffer.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif 

// --------------------------------------------------------------------------
typedef unsigned char byte;

typedef struct
{
   byte* buffer;
   int totoal_size;
   int data_size;
   byte* head;
   byte* tail;
} RING_BUFFER;

void initRingBuffer(RING_BUFFER* ring_buffer, byte* pbuffer, int size);
int getRingBufferSize(RING_BUFFER* ring_buffer);
int getRingBufferDataSize(RING_BUFFER* ring_buffer);
int getRingBufferFreeSize(RING_BUFFER* ring_buffer);
int writeRingBuffer(RING_BUFFER* ring_buffer, byte* data, int len);
int readRingBuffer(RING_BUFFER* ring_buffer, byte* data, int len);

#ifdef __cplusplus
}
#endif


#endif   // __UART_DEBUG_H__


