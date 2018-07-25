#include "application.h"
#include "bootloader.h"

SYSTEM_MODE(SEMI_AUTOMATIC);


void setup(){
  pinMode(D3, OUTPUT);
  digitalWrite(D3, 1);
  Wire.begin();
  Serial.begin(115200);
  while(Serial.read() != '\r'){

  }
  bootload();

  Particle.connect();
}

void loop(){

  
}
