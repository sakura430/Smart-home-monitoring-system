#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Delay.h"
void SPI1_Init(void);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
void W25Q64_CS_Enable(void);
void W25Q64_CS_Disable(void);
uint8_t W25Q64_Wait_Busy(void);
void W25Q64_Write(uint32_t WriteAddr, uint8_t Data);
void W25Q64_Erase_Sector(uint32_t SectorAddr);
uint8_t W25Q64_Read_Byte(uint32_t ReadAddr);
void W25Q64_Write_String(uint32_t WriteAddr, char *str, int len);



#endif


