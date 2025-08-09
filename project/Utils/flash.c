#include "flash.h"

// addr: 页首地址
void erasePage(uint32_t addr) {
    FLASH_Unlock();					//解锁
    FLASH_ErasePage(addr);			//页擦除
    FLASH_Lock();					//加锁
}

// addr: 页首地址
// data: 存放在首地址的32位数据
void changeFlashFor32(uint32_t addr, uint32_t data) {
    FLASH_Unlock();							// 解锁
    FLASH_ProgramWord(addr, data);			// 32bits
    FLASH_Lock();							// 加锁
}

// addr: 页首地址
// data: 存放在首地址的16位数据
void changeFlashFor16(uint32_t addr, uint16_t data) {
    FLASH_Unlock();							// 解锁
    FLASH_ProgramHalfWord(addr, data);		// 16bits
    FLASH_Lock();							// 加锁
}

// 读取数据
void readFlash(uint32_t addr, char *buffer, int len) {
    for (int i = 0; i < len; i++) {
        buffer[i] = *(__IO uint8_t *)(addr + i);
    }
}

void read_flash(char *wifi) {
    int address = 0x0800F000;
    int status = 0;
    char buffer[40] = {0};

    status = *((__IO uint32_t *)(address));

    if (status != 0xFFFFFFFF) {
        // 有内容
        
        readFlash(address + 8, buffer, 40);
        PrintToUTools("read: %d,  %s \r\n", status,  buffer);
    } else {
        // Flash无内容, 存入数据
        char *str = wifi;
        strncpy(buffer, str, strlen(str));

        erasePage(address);
        changeFlashFor32(address, 14);
        changeFlashFor32(address + 4, strlen(buffer));

        for (int i = 0; i < sizeof(buffer); i += 4) {
            changeFlashFor32(address + 8 + i, *(int *)(buffer + i));
        }

        PrintToUTools("write \r\n");
    }
}

