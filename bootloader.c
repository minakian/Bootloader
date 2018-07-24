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

#include "application.h"
#include "bootloader.h"

char cyacd_file[][] = 
{};

char transmit_buffer[32];
char receive_buffer[32];

int bootload(){
  getFlashSize();
}

int sendCommand(char address, int data_len, char * data){
  Serial.print("TX:,,");
  Wire.beginTransmission(address);
  for(int i = 0; i < data_len; i++){
      Wire.write(data[i]);
      Serial.print(String(data[i], HEX));
      Serial.print(",");
  }
  Wire.endTransmission();
  Serial.println();
}

int getResponse(char address, char bytes){
  int i = 0; 
  Serial.print("RX:,,");
  Wire.requestFrom(address, bytes);
  while(wire.available()){
    receive_buffer[i++] = Wire.read();
    Serial.print(String(data[i], HEX));
    Serial.print(",");
  }
}

int verifyStatus(){
  char status = 0;
  char start_pos = findStartLocation();
  status = receive_buffer[i+1];
//  Serial.println("Bootload Response - " + String(status, HEX));
  return status;
}

int findStartLocation(){
  char start_pos = 0;
  for(int i = 0; i < 32; i++){
    if(receive_buffer[i] == START_BYTE){
      start_pos = i;
      i = 32;
    }
  }
  return start_pos;
}

int verifyChecksum(){
  int checksum = 0;
  int start = findStartLocation();
  int length = receive_buffer[start + 3];
  length <<= 8;
  length |= receive_buffer[start + 2];
  for(int i = start; i < start + length + 4){
    checksum += receive_buffer[i];
  }
  int rx_checksum = receive_buffer[start + length + 6];
  rx_checksum <<= 8;
  rx_checksum |= receive_buffer[start + length + 5];
  if(rx_checksum != checksum){
    return -1;
  }
  return 0;


}

int getFlashSize(){
  char data[] = {START_BYTE, 0x32, 0x00, 0x00, 0xC7, 0xFF, END_BYTE}
  sendCommand(dev_address, 7, data);
  getResponse(dev_address, 32);
  int check = verifyChecksum();
  if(check){
    return -1;
  }
  int start = findStartLocation();
  int status = verifyStatus();
  if(status){
    return status;
  }
  if((receive_buffer[start + 4] != 0x29) || (receive_buffer[start + 5] != 0x00) || (receive_buffer[start + 6] != 0xFF) || (receive_buffer[start + 7] != 0x00)){
    return -2;
  }
  return 0; //TODO: Test for required result

}

int getAppStatus(){
  // This is not applicable to the current product
}

int eraseRow(){
  // TODO: Implement later, not currently used.
}

// This resets the bootloader, may be used in error.
int syncBootloader(){
  // TODO: Implement later
}

int setActiveApp(){
  // Not used in the current product.
}

int sendData(int data_len, char * data){
  int checksum = 0x01;
  char position = 3;
  transmit_buffer[0] = START_BYTE;
  transmit_buffer[1] = CMD_SEND_DATA;
  checksum += transmit_buffer[1];
  transmit_buffer[2] = data_len & 0xFF;
  checksum += transmit_buffer[2];
  transmit_buffer[3] = (data_len >> 8) & 0xFF;
  checksum += transmit_buffer[3];
  for(i = 0; i < data_len; i++){
    transmit_buffer[4 + i] = data[i];
    checksum += data[i];
    position = 5 + i;
  }
  checksum = (~checksum) + 1;
  transmit_buffer[position++] = (char) (checksum & 0xFF);
  transmit_buffer[position++] = (char) (checksum >> 8) & 0xFF;
  transmit_buffer[position] = 0x17;
  sendCommand(dev_address, position, transmit_buffer);
  getResponse(dev_address, 32);
  int check = verifyChecksum();
  if(check){
    return -1;
  }
  int start = findStartLocation();
  int status = verifyStatus();
  return status;
}

int enterBootloader(){
  char data[7] = {START_BYTE, 0x38, 0, 0, 0xC7, 0xFF, END_BYTE};
  sendCommand(dev_address, 7, data);
  getResponse(dev_address, 32);
  int check = verifyChecksum();
  if(check){
    return -1;
  }
  int start = findStartLocation();
  int status = verifyStatus();
  if(status){
    return status;
  }
  if((receive_buffer[start + 4] != 0x29) || (receive_buffer[start + 5] != 0x00) || (receive_buffer[start + 6] != 0xFF) || (receive_buffer[start + 7] != 0x00)){
    return -2;
  }
  int silicon_id = receive_buffer[start + 4];
  silicon_id <<= 8;
  silicon_id |= receive_buffer[start + 5];
  silicon_id <<= 8;
  silicon_id |= receive_buffer[start + 6];
  silicon_id <<= 8;
  silicon_id |= receive_buffer[start + 7];
  if(silicon_id != 0xA9110819){
    Serial.println("Device ID does not match... quitting bootload");
    return -1;
  }
  char silicon_rev = receive_buffer[start + 8];
  if(silicon_rev != 0){
    Serial.println("Silicon rev mismatch... continuing attempt to bootload")
  }
  char boot_ver_major = receive_buffer[start + 9];
  if(boot_ver_major != 0x32){
    Serial.println("Bootloader version (major) mismatch");
    return -2;
  }
  char boot_ver_minor = receive_buffer[start + 10];
  if(boot_ver_major != 0x01){
    Serial.println("Bootloader version (minor) mismatch");
    return -3;
  }
  char boot_ver_app = receive_buffer[start + 11];
  if(boot_ver_app != 0x01){
    Serial.println("Bootloader version (app) mismatch");
    return -2;
  }
  return 0;
}

int programRow(int array_id, int row_number, int data_len, char* data){
  int checksum = 0x01;
  char position = 3;
  transmit_buffer[0] = START_BYTE;
  transmit_buffer[1] = CMD_PROGRAM_ROW;
  checksum += transmit_buffer[1];
  transmit_buffer[2] = (char) (data_len & 0xFF);
  checksum += transmit_buffer[2];
  transmit_buffer[3] = (char) ((data_len >> 8) & 0xFF);
  checksum += transmit_buffer[3];
  for(int i = 0; i < data_len; i++){
    transmit_buffer[4 + i] = data[i];
    checksum += data[i];
  }
  checksum = (~checksum) + 1;
  transmit_buffer[position++] = (char) (checksum & 0xFF);
  transmit_buffer[position++] = (char) (checksum >> 8) & 0xFF;
  transmit_buffer[position] = 0x17;
  sendCommand(dev_address, position, transmit_buffer);
  getResponse(dev_address, 32);
  int check = verifyChecksum();
  if(check){
    return -1;
  }
  int start = findStartLocation();
  int status = verifyStatus();
  return status;
}

int verifyRow(char array_id, int row_number, char check){
  int checksum = START_BYTE + CMD_VERIFY_ROW + 0x03 + 0x00 + array_id + (row_number % 0xff) + (row_number >> 8);
  char data[] = {START_BYTE, CMD_VERIFY_ROW, 0x03, 0x00, array_id, (char) (row_number % 0xff), (char) (row_number >> 8), (char)(checksum & 0xff), (char)(checksum >> 8) & 0xFF), END_BYTE};
  sendCommand(dev_address, 10, data);
  getResponse(dev_address,32);
  int check = verifyChecksum();
  if(check){
    return -1;
  }
  int start = findStartLocation();
  int status = verifyStatus();
  if(status){
    return status;
  }
  if(check != receive_buffer[start + 4]){
    return -2;
  }
  return 0;
}

int exitBootloader(){
  char data[] = {START_BYTE, CMD_EXIT_BOOTLOADER, 0, 0, 0xC4, 0xFFFF, END_BYTE};
  sendCommand(dev_address, 9, data);
  return 0;
}
