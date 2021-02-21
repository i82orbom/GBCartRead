#include "io.h"
#include <Arduino.h>

int DATA_PINS[8] = {23, 22, 25, 24, 27, 26, 29, 28};                                  // D7...D0
int ADDR_PINS[16] = {46, 47, 44, 45, 42, 43, 40, 41, 38, 39, 36, 37, 34, 35, 32, 33}; // A0...A16

int READ_PIN = 50;
int WRITE_PIN = 53;
int CS1_PIN = 51;

#define ARRAY_COUNT(arr) (signed int)(sizeof(arr) / sizeof(arr[0]))

void setAddress(uint16_t address)
{
    uint16_t mask = 0x1;
    for (int pin = 0; pin < ARRAY_COUNT(ADDR_PINS); pin++)
    {
        digitalWrite(ADDR_PINS[pin], HIGH && (address & mask));
        mask <<= 1;
    }
}

uint8_t readData(uint16_t address)
{
    setAddress(address);
    delayMicroseconds(50);

    uint8_t data = 0;
    for (int pin = 0; pin < ARRAY_COUNT(DATA_PINS); pin++)
    {
        int bit = digitalRead(DATA_PINS[pin]) ? 1 : 0;
        data = (data << 1) + bit;
    }
    return data;
}

void writeData(uint16_t address, uint8_t data)
{
    setAddress(address);

    // Set pins as outputs
    uint16_t mask = 0x1;

    for (int pin = 0; pin < ARRAY_COUNT(DATA_PINS); pin++)
    {
        pinMode(DATA_PINS[pin], OUTPUT);

        digitalWrite(ADDR_PINS[pin], HIGH && (data & mask));
        mask <<= 1;
    }

    digitalWrite(READ_PIN, HIGH); // Read disable
    digitalWrite(WRITE_PIN, LOW); // Write enable

    asm volatile("nop");

    digitalWrite(READ_PIN, LOW);   // Read enable
    digitalWrite(WRITE_PIN, HIGH); // Write disable

    // Restore pins as inputs
    for (int pin = 0; pin < ARRAY_COUNT(DATA_PINS); pin++)
    {
        pinMode(DATA_PINS[pin], INPUT);
    }
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

    digitalWrite(READ_PIN, LOW);   // Read enable
    digitalWrite(WRITE_PIN, HIGH); // Write disable
    digitalWrite(CS1_PIN, HIGH);   // READ ROM
}

void enableWriteRAM()
{
    digitalWrite(CS1_PIN, LOW); // WRITE RAM
}

void disableWriteRAM()
{
    digitalWrite(CS1_PIN, LOW); // READ ROM
}