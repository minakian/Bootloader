#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

//#include "application.h"

#define CMD_VERIFY_CHECKSUM 0x31
#define CMD_GET_FLASH_SIZE  0x32
#define CMD_GET_APP_STATUS  0x33
#define CMD_ERASE_ROW       0x34
#define CMD_SYNC_BOOTLOAD   0x35
#define CMD_SET_ACTIVE_APP  0x36
#define CMD_SEND_DATA       0x37
#define CMD_ENTER_BOOTLOAD  0x38
#define CMD_PROGRAM_ROW     0x39
#define CMD_VERIFY_ROW      0x3A
#define CMD_EXIT_BOOTLOADER 0x3B

#define STATUS_SUCCESS      0x00
#define STATUS_ERR_VERIFY   0x02
#define STATUS_ERR_LENGTH   0x03
#define STATUS_ERR_DATA     0x04
#define STATUS_ERR_CMD      0x05
#define STATUS_ERR_DEVICE   0x06
#define STATUS_ERR_VERSION  0x07
#define STATUS_ERR_CHECK    0x08
#define STATUS_ERR_ARRAY    0x09
#define STATUS_ERR_ROW      0x0A
#define STATUS_ERR_APP      0x0C
#define STATUS_ERR_ACTIVE   0x0D
#define STATUS_ERR_UNK      0x0F

#define START_BYTE  0x01
#define END_BYTE    0x17

const char dev_address = 0x08;

#define CYACD_ROWS        21
#define CYACD_ROW_LENGTH  134

int bootload();

int parseAndFlashRow(int row);

int sendCommand(char address, int data_len, char * data);

int getResponse(char address, char bytes);

int verifyStatus();

int findStartLocation();

int verifyChecksum();

int getFlashSize();

int getAppStatus();

int eraseRow();

int syncBootloader();

int setActiveApp();

int sendData(int data_len, char * data);

int enterBootloader();

int programRow(int array_id, int row_number, int data_len, char* data);

int verifyRow(char array_id, int row_number, char check);

int exitBootloader();

#endif