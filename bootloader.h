#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H

int bootload();

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