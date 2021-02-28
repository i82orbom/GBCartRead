#ifndef __IO_H__
#define __IO_H__

#include <Arduino.h>

#define DEBUG 0

uint8_t readData(uint16_t address);
void writeData(uint16_t address, uint8_t data);
void setAddress(uint16_t address);
void setupPins();
void enableWriteRAM();
void disableWriteRAM();
void resetCtrlPins();
void offCtrlPins();
void enableRead();
void enableWrite();
void disableWrite();
void disableRead();
void disableReadWrite();
void setDataInput();
void setDataOutput();
void csLOW();
void csHIGH();

#endif /* __IO_H__ */
