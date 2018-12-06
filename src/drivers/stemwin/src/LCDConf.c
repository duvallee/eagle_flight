/**
  ******************************************************************************
  * @file    lcdconf.c
  * @author  MCD Application Team
  * @brief   This file implements the configuration for the GUI library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
#include "main.h"

#undef  LCD_SWAP_XY
#undef  LCD_MIRROR_Y
#undef  LCD_SWAP_RB

#define LCD_SWAP_XY                                      1
#define LCD_MIRROR_Y                                     1

#define LCD_SWAP_RB                                      1

#define XSIZE_PHYS                                       LCD_WIDTH
#define YSIZE_PHYS                                       LCD_HEIGHT

#define NUM_BUFFERS                                      3              // Number of multiple buffers to be used
#define NUM_VSCREENS                                     1              // Number of virtual screens to be used

#define BK_COLOR                                         GUI_DARKBLUE

#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS                                   1

#define COLOR_CONVERSION_0                               GUICC_M8888I
#define DISPLAY_DRIVER_0                                 GUIDRV_LIN_32


#if (GUI_NUM_LAYERS > 1)
#define COLOR_CONVERSION_1                               GUICC_M1555I
#define DISPLAY_DRIVER_1                                 GUIDRV_LIN_16
#endif

#ifndef XSIZE_PHYS
#error Physical X size of display is not defined!
#endif

#ifndef YSIZE_PHYS
#error Physical Y size of display is not defined!
#endif

#ifndef NUM_VSCREENS
#define NUM_VSCREENS                                     1
#else

#if (NUM_VSCREENS <= 0)
#error At least one screeen needs to be defined!
#endif

#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
#error Virtual screens and multiple buffers are not allowed!
#endif

#define LCD_LAYER0_FRAME_BUFFER                          ((int) LCD_FRAMEBUFFER_LAYER_0)
#define LCD_LAYER1_FRAME_BUFFER                          ((int) LCD_FRAMEBUFFER_LAYER_1)

// --------------------------------------------------------------------------
static LTDC_HandleTypeDef* g_ltdc_handle;
static DMA2D_HandleTypeDef* g_dma2d_handle;

static LCD_LayerPropTypedef g_layer_prop[GUI_NUM_LAYERS];

static const LCD_API_COLOR_CONV * g_apColorConvAPI[]     =
{
   COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
   COLOR_CONVERSION_1,
#endif
};

/* --------------------------------------------------------------------------
 * Name : GetBufferSize()
 *
 *
 * -------------------------------------------------------------------------- */
static U32 GetBufferSize(U32 LayerIndex) 
{
   U32 BufferSize;

   BufferSize                                            = g_layer_prop[LayerIndex].xSize * g_layer_prop[LayerIndex].ySize * g_layer_prop[LayerIndex].BytesPerPixel;
   return BufferSize;
}

/* --------------------------------------------------------------------------
 * Name : LCD_LL_GetPixelformat()
 *
 *
 * -------------------------------------------------------------------------- */
static inline U32 LCD_LL_GetPixelformat(U32 LayerIndex)
{
   if (LayerIndex == 0)
   {
      return LTDC_PIXEL_FORMAT_ARGB8888;
   } 
   else
   {
      return LTDC_PIXEL_FORMAT_ARGB1555;
   } 
}

/* --------------------------------------------------------------------------
 * Name : LCD_LL_LayerInit()
 *
 *
 * -------------------------------------------------------------------------- */
static void LCD_LL_LayerInit(U32 LayerIndex) 
{
   LTDC_LayerCfgTypeDef layer_cfg;
  
   if (LayerIndex < GUI_NUM_LAYERS) 
   { 
      // Layer configuration
      layer_cfg.WindowX0                                 = 0;
      layer_cfg.WindowX1                                 = XSIZE_PHYS;
      layer_cfg.WindowY0                                 = 0;
      layer_cfg.WindowY1                                 = YSIZE_PHYS; 
      layer_cfg.PixelFormat                              = LCD_LL_GetPixelformat(LayerIndex);
      layer_cfg.FBStartAdress                            = ((uint32_t) 0xC0000000);
      layer_cfg.Alpha                                    = 255;
      layer_cfg.Alpha0                                   = 0;
      layer_cfg.Backcolor.Blue                           = 0;
      layer_cfg.Backcolor.Green                          = 0;
      layer_cfg.Backcolor.Red                            = 0;
      layer_cfg.BlendingFactor1                          = LTDC_BLENDING_FACTOR1_PAxCA;
      layer_cfg.BlendingFactor2                          = LTDC_BLENDING_FACTOR2_PAxCA;
      layer_cfg.ImageWidth                               = XSIZE_PHYS;
      layer_cfg.ImageHeight                              = YSIZE_PHYS;
      HAL_LTDC_ConfigLayer(g_ltdc_handle, &layer_cfg, LayerIndex);  

      // Enable LUT on demand
      if (LCD_GetBitsPerPixelEx(LayerIndex) <= 8) 
      {
         // Enable usage of LUT for all modes with <= 8bpp
         HAL_LTDC_EnableCLUT(g_ltdc_handle, LayerIndex);
      } 
   } 
}


/* --------------------------------------------------------------------------
 * Name : DMA2D_CopyBuffer()
 *
 *
 * -------------------------------------------------------------------------- */
static void DMA2D_CopyBuffer(U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst)
{
   U32 PixelFormat;

   PixelFormat                                           = LCD_LL_GetPixelformat(LayerIndex);
   DMA2D->CR                                             = 0x00000000UL | (1 << 9);  

   // Set up pointers
   DMA2D->FGMAR                                          = (U32) pSrc;
   DMA2D->OMAR                                           = (U32) pDst;
   DMA2D->FGOR                                           = OffLineSrc;
   DMA2D->OOR                                            = OffLineDst;

   // Set up pixel format
   DMA2D->FGPFCCR                                        = PixelFormat;

   // Set up size
   DMA2D->NLR                                            = (U32)(xSize << 16) | (U16) ySize;
   DMA2D->CR                                             |= DMA2D_CR_START;

   // Wait until transfer is done
   while (DMA2D->CR & DMA2D_CR_START) 
   {
   }
}

/* --------------------------------------------------------------------------
 * Name : DMA2D_FillBuffer()
 *
 * @param  LayerIndex : Layer Index
 * @param  pDst:        pointer to destination
 * @param  xSize:       X size
 * @param  ySize:       Y size
 * @param  OffLine:     offset after each line
 * @param  ColorIndex:  color to be used.           
 *
 * -------------------------------------------------------------------------- */
static void DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex) 
{
   U32 PixelFormat;

   PixelFormat                                           = LCD_LL_GetPixelformat(LayerIndex);

   // Set up mode
   DMA2D->CR                                             = 0x00030000UL | (1 << 9);
   DMA2D->OCOLR                                          = ColorIndex;

   // Set up pointers
   DMA2D->OMAR                                           = (U32) pDst;
   // Set up offsets
   DMA2D->OOR                                            = OffLine;

   // Set up pixel format
   DMA2D->OPFCCR                                         = PixelFormat;

   // Set up size
   DMA2D->NLR                                            = (U32)(xSize << 16) | (U16)ySize;
   DMA2D->CR                                             |= DMA2D_CR_START; 

   // Wait until transfer is done
   while (DMA2D->CR & DMA2D_CR_START) 
   {
   }
}


/* --------------------------------------------------------------------------
 * Name : CUSTOM_CopyBuffer()
 *
 *
 * -------------------------------------------------------------------------- */
static void CUSTOM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst)
{
   U32 BufferSize, AddrSrc, AddrDst;

   BufferSize                                            = GetBufferSize(LayerIndex);
   AddrSrc                                               = g_layer_prop[LayerIndex].address + BufferSize * IndexSrc;
   AddrDst                                               = g_layer_prop[LayerIndex].address + BufferSize * IndexDst;
   DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, g_layer_prop[LayerIndex].xSize, g_layer_prop[LayerIndex].ySize, 0, 0);
   g_layer_prop[LayerIndex].buffer_index                 = IndexDst;
}

/* --------------------------------------------------------------------------
 * Name : CUSTOM_CopyBuffer()
 *
 * @param  LayerIndex : Layer Index
 * @param  x0:          X0 position
 * @param  y0:          Y0 position
 * @param  x1:          X1 position
 * @param  y1:          Y1 position
 * @param  xSize:       X size.
 * @param  ySize:       Y size.
 *
 * -------------------------------------------------------------------------- */
static void CUSTOM_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) 
{
   U32 AddrSrc, AddrDst;  

   AddrSrc                                               = g_layer_prop[LayerIndex].address + (y0 * g_layer_prop[LayerIndex].xSize + x0) * g_layer_prop[LayerIndex].BytesPerPixel;
   AddrDst                                               = g_layer_prop[LayerIndex].address + (y1 * g_layer_prop[LayerIndex].xSize + x1) * g_layer_prop[LayerIndex].BytesPerPixel;
   DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, g_layer_prop[LayerIndex].xSize - xSize, g_layer_prop[LayerIndex].xSize - xSize);
}

/* --------------------------------------------------------------------------
 * Name : CUSTOM_CopyBuffer()
 *
 * @param  LayerIndex : Layer Index
 * @param  x0:          X0 position
 * @param  y0:          Y0 position
 * @param  x1:          X1 position
 * @param  y1:          Y1 position
 * @param  PixelIndex:  Pixel index.
 *
 * -------------------------------------------------------------------------- */
static void CUSTOM_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) 
{
   U32 BufferSize, AddrDst;
   int xSize, ySize;


   if (GUI_GetDrawMode() == GUI_DM_XOR) 
   {		
      LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
      LCD_FillRect(x0, y0, x1, y1);
      LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_FillRect);
   } 
   else 
   {
      xSize                                              = x1 - x0 + 1;
      ySize                                              = y1 - y0 + 1;
      BufferSize                                         = GetBufferSize(LayerIndex);
      AddrDst                                            = g_layer_prop[LayerIndex].address + BufferSize * g_layer_prop[LayerIndex].buffer_index + (y0 * g_layer_prop[LayerIndex].xSize + x0) * g_layer_prop[LayerIndex].BytesPerPixel;
      DMA2D_FillBuffer(LayerIndex, (void *)AddrDst, xSize, ySize, g_layer_prop[LayerIndex].xSize - xSize, PixelIndex);
   }
}

/* --------------------------------------------------------------------------
 * Name : CUSTOM_CopyBuffer()
 *
 * @brief  Draw indirect color bitmap
 * @param  pSrc: pointer to the source
 * @param  pDst: pointer to the destination
 * @param  OffSrc: offset source
 * @param  OffDst: offset destination
 * @param  PixelFormatDst: pixel format for destination
 * @param  xSize: X size
 * @param  ySize: Y size
 *
 * -------------------------------------------------------------------------- */
static void CUSTOM_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
   U32 BufferSize, AddrDst;
   int OffLineSrc, OffLineDst;

   BufferSize                                            = GetBufferSize(LayerIndex);
   AddrDst                                               = g_layer_prop[LayerIndex].address + BufferSize * g_layer_prop[LayerIndex].buffer_index + (y * g_layer_prop[LayerIndex].xSize + x) * g_layer_prop[LayerIndex].BytesPerPixel;
   OffLineSrc                                            = (BytesPerLine / 4) - xSize;
   OffLineDst                                            = g_layer_prop[LayerIndex].xSize - xSize;
   DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}


/* --------------------------------------------------------------------------
 * Name : lcd_init_for_stemwin()
 *
 *
 * -------------------------------------------------------------------------- */
void lcd_init_for_stemwin(void* ltdc_handle, void* dma2d_handle)
{
   g_ltdc_handle                                         = (LTDC_HandleTypeDef*) ltdc_handle;
   g_dma2d_handle                                        = (DMA2D_HandleTypeDef*) g_dma2d_handle;
}

/* --------------------------------------------------------------------------
 * Name : LCD_X_Config()
 *
 *
 * -------------------------------------------------------------------------- */
void LCD_X_Config(void) 
{
   U32 i;

   // At first initialize use of multiple buffers on demand
#if (NUM_BUFFERS > 1)
    for (i = 0; i < GUI_NUM_LAYERS; i++) 
    {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
#endif

   // Set display driver and color conversion for 1st layer
   GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  
   // Set size of 1st layer
   if (LCD_GetSwapXYEx(0))
   {
      LCD_SetSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
   }
   else
   {
      LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
   }

#if (GUI_NUM_LAYERS > 1)
   // Set display driver and color conversion for 2nd layer
   GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);

   // Set size of 2nd layer
   if (LCD_GetSwapXYEx(1))
   {
      LCD_SetSizeEx(1, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(1, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
   }
   else
   {
      LCD_SetSizeEx(1, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(1, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
   }
#endif
    
    // Initialize GUI Layer structure
    g_layer_prop[0].address                              = LCD_LAYER0_FRAME_BUFFER;
    
#if (GUI_NUM_LAYERS > 1)
    g_layer_prop[1].address                              = LCD_LAYER1_FRAME_BUFFER; 
#endif
       
   // Setting up VRam address and custom functions for CopyBuffer-, CopyRect- and FillRect operations
   for (i = 0; i < GUI_NUM_LAYERS; i++) 
   {
      g_layer_prop[i].pColorConvAPI                      = (LCD_API_COLOR_CONV *) g_apColorConvAPI[i];
      g_layer_prop[i].pending_buffer                     = -1;

      // Set VRAM address
      LCD_SetVRAMAddrEx(i, (void *) (g_layer_prop[i].address));

      // Remember color depth for further operations
      g_layer_prop[i].BytesPerPixel                        = LCD_GetBitsPerPixelEx(i) >> 3;

      // Set custom functions for several operations
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void)) CUSTOM_CopyBuffer);
      LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,   (void(*)(void)) CUSTOM_CopyRect);
      LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void)) CUSTOM_FillRect);

      // Set up drawing routine for 32bpp bitmap using DMA2D
      if (LCD_LL_GetPixelformat(i) == LTDC_PIXEL_FORMAT_ARGB8888)
      {
         LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_32BPP, (void(*)(void)) CUSTOM_DrawBitmap32bpp);      // Set up drawing routine for 32bpp bitmap using DMA2D. Makes only sense with ARGB8888
      }    
   }
}

/* --------------------------------------------------------------------------
 * Name : LCD_X_DisplayDriver()
 *
 * @brief  This function is called by the display driver for several purposes.
 *         To support the according task the routine needs to be adapted to
 *         the display controller. Please note that the commands marked with
 *         'optional' are not cogently required and should only be adapted if
 *         the display controller supports these features
 * @param  LayerIndex: Index of layer to be configured 
 * @param  Cmd       :Please refer to the details in the switch statement below
 * @param  pData     :Pointer to a LCD_X_DATA structure
 * @retval Status (-1 : Error,  0 : Ok)
 *
 * -------------------------------------------------------------------------- */
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) 
{
   int ret                                               = -1;
   U32 addr;
   int xPos, yPos;
   U32 Color;
    
   switch (Cmd)
   {
      case LCD_X_INITCONTROLLER : 
         LCD_LL_LayerInit(LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_SETORG :
         addr                                            = g_layer_prop[LayerIndex].address + ((LCD_X_SETORG_INFO *)pData)->yPos * g_layer_prop[LayerIndex].xSize * g_layer_prop[LayerIndex].BytesPerPixel;
         HAL_LTDC_SetAddress(g_ltdc_handle, addr, LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_SHOWBUFFER :
         g_layer_prop[LayerIndex].pending_buffer         = ((LCD_X_SHOWBUFFER_INFO *) pData)->Index;
         ret                                             = 0;
         break;

      case LCD_X_SETLUTENTRY :
         HAL_LTDC_ConfigCLUT(g_ltdc_handle, (uint32_t *)&(((LCD_X_SETLUTENTRY_INFO *)pData)->Color), 1, LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_ON :
         __HAL_LTDC_ENABLE(g_ltdc_handle);
         ret                                             = 0;
         break;

      case LCD_X_OFF :
         __HAL_LTDC_DISABLE(g_ltdc_handle);
         ret                                             = 0;
         break;

      case LCD_X_SETVIS :
         if (((LCD_X_SETVIS_INFO *) pData)->OnOff  == ENABLE )
         {
            __HAL_LTDC_LAYER_ENABLE(g_ltdc_handle, LayerIndex); 
         }
         else
         {
            __HAL_LTDC_LAYER_DISABLE(g_ltdc_handle, LayerIndex); 
         }
         __HAL_LTDC_RELOAD_CONFIG(g_ltdc_handle); 
         ret                                             = 0;
         break;

      case LCD_X_SETPOS :
         HAL_LTDC_SetWindowPosition(g_ltdc_handle, ((LCD_X_SETPOS_INFO *) pData)->xPos,  ((LCD_X_SETPOS_INFO *) pData)->yPos, LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_SETSIZE :
         GUI_GetLayerPosEx(LayerIndex, &xPos, &yPos);
         g_layer_prop[LayerIndex].xSize                  = ((LCD_X_SETSIZE_INFO *) pData)->xSize;
         g_layer_prop[LayerIndex].ySize                  = ((LCD_X_SETSIZE_INFO *) pData)->ySize;
         HAL_LTDC_SetWindowPosition(g_ltdc_handle, xPos, yPos, LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_SETALPHA :
         HAL_LTDC_SetAlpha(g_ltdc_handle, ((LCD_X_SETALPHA_INFO *) pData)->Alpha, LayerIndex);
         ret                                             = 0;
         break;

      case LCD_X_SETCHROMAMODE :
         if (((LCD_X_SETCHROMAMODE_INFO *) pData)->ChromaMode != 0)
         {
            HAL_LTDC_EnableColorKeying(g_ltdc_handle, LayerIndex);
         }
         else
         {
            HAL_LTDC_DisableColorKeying(g_ltdc_handle, LayerIndex);      
         }
         ret                                             = 0;
         break;

      case LCD_X_SETCHROMA :
         Color                                           = ((((LCD_X_SETCHROMA_INFO *) pData)->ChromaMin & 0xFF0000) >> 16) |       \
                                                           (((LCD_X_SETCHROMA_INFO *) pData)->ChromaMin & 0x00FF00) |               \
                                                           ((((LCD_X_SETCHROMA_INFO *) pData)->ChromaMin & 0x0000FF) << 16);

         HAL_LTDC_ConfigColorKeying(g_ltdc_handle, Color, LayerIndex);
         ret                                             = 0;
         break;

      default :
         ret                                             = -1;
         break;
   }
   return ret;
}

 