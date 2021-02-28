#include "io.h"
#include <Arduino.h>

// int DATA_PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

int DATA_PINS[8] = {23, 22, 25, 24, 27, 26, 29, 28};  // D7....D0
int ADDR_PINS[16] = {44, 45, 42, 43, 40, 41, 38, 39, 36, 37, 34, 35, 32, 33, 30, 31}; // A0...A16

int READ_PIN = 50;
int WRITE_PIN = 51;
int CS1_PIN = 53;

void enableRead() {
    digitalWrite(READ_PIN, LOW);
}

void disableRead() {
    digitalWrite(READ_PIN, HIGH);
}

void enableWrite() {
    digitalWrite(WRITE_PIN, LOW);
}

void disableWrite() {
    digitalWrite(WRITE_PIN, HIGH);
}

void csLOW() {
    digitalWrite(CS1_PIN, LOW);
}

void csHIGH() {
    digitalWrite(CS1_PIN, HIGH);
}

void setDataOutput() {
     for (int pin = 0; pin < 8; pin++){
        pinMode(DATA_PINS[pin], OUTPUT);
    }
}

void setDataInput() {
     for (int pin = 0; pin < 8; pin++){
        pinMode(DATA_PINS[pin], INPUT);
    }
}

void setAddress(uint16_t address)
{
    for (uint16_t pin = 0; pin < 16; pin++)
    {
        uint16_t val = HIGH && (address & (0x1 << pin));
        digitalWrite(ADDR_PINS[pin], val);
    }   
}

uint8_t readData(uint16_t address)
{
    setAddress(address);
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    enableRead();
    csLOW();

    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    uint8_t data = 0;
    for (int pin = 0; pin < 8; pin++)
    {   
        uint8_t bit = digitalRead(DATA_PINS[pin]) ? HIGH : LOW;
        data = (data << 1) | bit;
    }

    disableRead();
    csHIGH();

    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    return data;
}

void writeData(uint16_t address, uint8_t data)
{
    setAddress(address);
  
    // Write data
    for (int i = 0; i < 8; i++){
        uint8_t val = bitRead(data, 7-i);
      
        // From D7 to D0
        digitalWrite(DATA_PINS[i], val);
    }

    enableWrite();
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
    disableWrite();
}

void setupPins()
{
    // Setup address pins
    for (int i = 0; i < 16; i++)
    {
        pinMode(ADDR_PINS[i], OUTPUT);
    }
    // Setup data pins
    for (int i = 0; i < 8; i++)
    {
        pinMode(DATA_PINS[i], INPUT);
    }
    pinMode(READ_PIN, OUTPUT);
    pinMode(WRITE_PIN, OUTPUT);
    pinMode(CS1_PIN, OUTPUT);

    resetCtrlPins();
}

void enableWriteRAM()
{
    digitalWrite(CS1_PIN, LOW); // Select RAM
}

void disableWriteRAM()
{
    digitalWrite(CS1_PIN, HIGH); // Select ROM
}

// Turn READ_PIN, WRITE_PIN and CS1_PIN to high so they are deselected (reset state)
void resetCtrlPins() {
    digitalWrite(READ_PIN, HIGH);
    digitalWrite(WRITE_PIN, HIGH);
    digitalWrite(CS1_PIN, HIGH);
}

void offCtrlPins() {
    digitalWrite(READ_PIN, LOW);
    digitalWrite(WRITE_PIN, LOW);
    digitalWrite(CS1_PIN, LOW);
}