/*
 *  File: qspi_flash.h
 *
 * Written by duvallee.lee in 2019
 *
 */

#ifndef __QSPI_FLASH_H__
#define __QSPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// QSPI Error Codes
#define QSPI_OK                                          (0)
#define QSPI_ERROR                                       (-1)
#define QSPI_BUSY                                        (-2)
#define QSPI_NOT_SUPPORTED                               (-3)
#define QSPI_SUSPENDED                                   (-4)

// QSPI Info
typedef struct
{
   uint32_t FlashSize;                                                           // !< Size of the flash */
   uint32_t EraseSectorSize;                                                     // !< Size of sectors for the erase operation */
   uint32_t EraseSectorsNumber;                                                  // !< Number of sectors for the erase operation */
   uint32_t ProgPageSize;                                                        // !< Size of pages for the program operation */
   uint32_t ProgPagesNumber;                                                     // !< Number of pages for the program operation */
} QSPI_Info;


// -----------------------------------------------------------------------------
int qspi_flash_init();
int qspi_flash_deinit();
uint8_t qspi_flash_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t qspi_flash_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t qspi_flash_Erase_Block(uint32_t BlockAddress);
uint8_t qspi_flash_Erase_Chip(void);
uint8_t qspi_flash_GetStatus(void);
uint8_t qspi_flash_GetInfo(QSPI_Info* pInfo);
uint8_t qspi_flash_EnableMemoryMappedMode(void);

#ifdef __cplusplus
}
#endif

#endif   // __QSPI_FLASH_H__

