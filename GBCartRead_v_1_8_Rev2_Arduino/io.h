#ifndef __IO_H__
#define __IO_H__

#include <Arduino.h>

uint8_t readData(uint16_t address);
void writeData(uint16_t address, uint8_t data);
void setAddress(uint16_t address);
void setupPins();
void enableWriteRAM();
void disableWriteRAM();

#endif /* __IO_H__ */
