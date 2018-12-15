/*
 * File: ring_buffer.c
 *
 * Written by duvallee.lee in 2018
 *
 */
#include "stdlib.h"
#include "main.h"

/* --------------------------------------------------------------------------
 * Name : initRingBuffer()
 *
 *
 * -------------------------------------------------------------------------- */
void initRingBuffer(RING_BUFFER* ring_buffer, byte* pbuffer, int size)
{
   ring_buffer->buffer                                   = pbuffer;
   ring_buffer->totoal_size                              = size;
   ring_buffer->data_size                                = 0;
   ring_buffer->head                                     = ring_buffer->buffer;
   ring_buffer->tail                                     = ring_buffer->buffer;
}

/* --------------------------------------------------------------------------
 * Name : getRingBufferSize()
 *
 *
 * -------------------------------------------------------------------------- */
int getRingBufferSize(RING_BUFFER* ring_buffer)
{
   return ring_buffer->totoal_size;
}

/* --------------------------------------------------------------------------
 * Name : getRingBufferDataSize()
 *
 *
 * -------------------------------------------------------------------------- */
int getRingBufferDataSize(RING_BUFFER* ring_buffer)
{
   return ring_buffer->data_size;
}

/* --------------------------------------------------------------------------
 * Name : getRingBufferFreeSize()
 *
 *
 * -------------------------------------------------------------------------- */
int getRingBufferFreeSize(RING_BUFFER* ring_buffer)
{
   return (ring_buffer->totoal_size - ring_buffer->data_size);
}

/* --------------------------------------------------------------------------
 * Name : writeRingBuffer()
 *
 *
 * -------------------------------------------------------------------------- */
int writeRingBuffer(RING_BUFFER* ring_buffer, byte* data, int len)
{
   int circular_ramin                                    = (int) ((ring_buffer->buffer + ring_buffer->totoal_size) - ring_buffer->tail);
   if ((ring_buffer->totoal_size - ring_buffer->data_size) < len)
   {
      return -1;
   }

   if (len > circular_ramin)
   {
      memcpy(ring_buffer->tail, data, circular_ramin);
      ring_buffer->data_size                             += circular_ramin;

      memcpy(ring_buffer->buffer, (data + circular_ramin), (len - circular_ramin));
      ring_buffer->tail                                  = ring_buffer->buffer + (len - circular_ramin);
      ring_buffer->data_size                             += (len - circular_ramin);
   }
   else
   {
      memcpy(ring_buffer->tail, data, len);
      ring_buffer->tail                                  += len;
      ring_buffer->data_size                             += len;
   }

   return len;
}

/* --------------------------------------------------------------------------
 * Name : readRingBuffer()
 *
 *
 * -------------------------------------------------------------------------- */
int readRingBuffer(RING_BUFFER* ring_buffer, byte* data, int len)
{
   int ramin                                                = 0;
   int circular_remain                                      = 0;
   if (ring_buffer->data_size == 0)
   {
      return -1;
   }

   if (ring_buffer->head < ring_buffer->tail)
   {
      if (((int) (ring_buffer->tail - ring_buffer->head)) > len)
      {
         memcpy(ring_buffer->head, data, len);
         ring_buffer->head                                  += len;
         ring_buffer->data_size                             -= len;
      }
      else
      {
         ramin                                              = ((int) (ring_buffer->tail - ring_buffer->head));
         memcpy(ring_buffer->head, data, ramin);
         ring_buffer->head                                  += ramin;
         ring_buffer->data_size                             -= ramin;
         len                                                = ramin;
      }
   }
   else
   {
      circular_remain                                       = ring_buffer->totoal_size - ((int) ((ring_buffer->buffer + ring_buffer->totoal_size) - ring_buffer->head));
      if (circular_remain > len)
      {
         memcpy(ring_buffer->head, data, len);
         ring_buffer->head                                  += len;
         ring_buffer->data_size                             -= len;
      }
      else
      {
         ramin                                              = (len - circular_remain);
         memcpy(ring_buffer->head, data, circular_remain);
         ring_buffer->data_size                             -= circular_remain;

         memcpy(ring_buffer->buffer, (data + circular_remain), ramin);
         ring_buffer->head                                  = (ring_buffer->buffer + ramin);
         ring_buffer->data_size                             -= ramin;
      }
   }

   return len;
}


