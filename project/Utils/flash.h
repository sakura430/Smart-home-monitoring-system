#ifndef __FLASH_H
#define __FLASH_H
#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "USART1.h"
void erasePage(uint32_t addr);
void changeFlashFor32(uint32_t addr, uint32_t data);
void changeFlashFor16(uint32_t addr, uint16_t data);
void readFlash(uint32_t addr, char *buffer, int len);
#endif


