#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "gbcart.h"
#include "io.h"

#define CMD_UNKNOWN -1
#define CMD_HEADER 0
#define CMD_READ_ROM 1
#define CMD_READ_RAM 2
#define CMD_WRITE_RAM 3

struct Cartridge readCartridgeHeader;

void readROM();
void readRAM();
void writeRAM();

int readCommand();

void setup()
{
  readCartridgeHeader.isRead = 0;

  setupPins();

  Serial.begin(115200);
}

void loop()
{
  while (Serial.available() <= 0)
  {
    delay(200);
  }

  switch (readCommand())
  {
  case CMD_HEADER:
    readCartridgeHeader = readHeader();

    Serial.println(readCartridgeHeader.gameTitle);
    Serial.println(readCartridgeHeader.cartridgeType);
    Serial.println(readCartridgeHeader.romSize);
    Serial.println(readCartridgeHeader.ramSize);
    Serial.println(readCartridgeHeader.logoCheck);
    break;
  case CMD_READ_ROM:
    readROM();
    break;
  case CMD_READ_RAM:
    readRAM();
    break;
  case CMD_WRITE_RAM:
    writeRAM();
    break;
  default:
    Serial.println("Unknown command received");
    break;
  }
}

void readROM()
{
  if (readCartridgeHeader.isRead == 0)
  {
    readCartridgeHeader = readHeader();
  }
  resetCtrlPins();
  uint16_t romAddress = 0;

  // Read number of banks and switch banks
  for (uint16_t bank = 1; bank < readCartridgeHeader.romBanks; bank++)
  {
    setDataOutput();
    if (readCartridgeHeader.cartridgeType >= 5)
    {                          // MBC2 and above
      if (DEBUG) {
        Serial.print("Changing bank to: ");
        Serial.println(bank);
      }
      writeData(0x2100, bank); // Set ROM bank
      
    }
    else
    {                                 // MBC1
      writeData(0x6000, 0);           // Set ROM Mode
      writeData(0x4000, bank >> 5);   // Set bits 5 & 6 (01100000) of ROM bank
      writeData(0x2000, bank & 0x1F); // Set bits 0 & 4 (00011111) of ROM bank
    }
    if (bank > 1)
    {
      romAddress = 0x4000;
    }
    setDataInput();
    // Read up to 7FFF per bank
    
    for (; romAddress <= 0x7FFF; romAddress += 64)
    {
      uint8_t data[64];
      for (int i = 0; i < 64; i++)
      {
        data[i] = readData(romAddress + i);
      }
      if (!DEBUG) {
        Serial.write(data, 64); // Send the 64 byte chunk
      }
    }
  }
}

void readRAM()
{
  resetCtrlPins();

  if (readCartridgeHeader.isRead == 0)
  {
    readCartridgeHeader = readHeader();
  }

  // MBC2 Fix (unknown why this fixes reading the ram, maybe has to read ROM before RAM?)
  readData(0x0134);

  // Does cartridge have RAM
  if (readCartridgeHeader.ramEndAddress > 0)
  {
    setDataOutput();

    if (readCartridgeHeader.cartridgeType <= 4)
    {                       // MBC1
      writeData(0x6000, 1); // Set RAM Mode
    }

    // Initialise MBC
    writeData(0x0000, 0x0A);

    // Switch RAM banks
    for (uint8_t bank = 0; bank < readCartridgeHeader.ramBanks; bank++)
    {
      setDataOutput();

      writeData(0x4000, bank);

      setDataInput();
      // Read RAM
      for (uint16_t ramAddress = 0xA000; ramAddress <= readCartridgeHeader.ramEndAddress; ramAddress += 64)
      {
        uint8_t data[64];
        for (uint8_t i = 0; i < 64; i++)
        {
          data[i] = readData(ramAddress + i);
        }

        Serial.write(data, 64); // Send the 64 byte chunk
      }
    }

    setDataOutput();

    // Disable RAM
    writeData(0x0000, 0x00);
  }
}

void writeRAM()
{
  if (readCartridgeHeader.isRead == 0)
  {
    readCartridgeHeader = readHeader();
  }
  resetCtrlPins();

  setDataOutput();
  // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
  readData(0x0134);

  setDataInput();
  // Does cartridge have RAM
  if (readCartridgeHeader.ramEndAddress > 0)
  {
    if (readCartridgeHeader.cartridgeType <= 4)   // MBC1
    {                     
      writeData(0x6000, 1); // Set RAM Mode
    }

    // Enable RAM
    writeData(0x0000, 0xA);

    // Switch RAM banks
    for (int bank = 0; bank < readCartridgeHeader.ramBanks; bank++)
    {

      writeData(0x4000, bank);      
      // Write RAM
      for (uint16_t ramAddress = 0xA000; ramAddress <= readCartridgeHeader.ramEndAddress; ramAddress++)
      {        
        // Wait for serial input
        while (Serial.available() <= 0)
          ;
        
        // Read input
        uint8_t readValue = (uint8_t)Serial.read();
        
        setDataOutput();
        csLOW();
        writeData(ramAddress, readValue);

        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        csHIGH();

      }
    }

    // Disable RAM
    writeData(0x0000, 0x00);
    Serial.flush(); // Flush any serial data that wasn't processed
  }
  resetCtrlPins();
}

int readCommand()
{
  char input[16];
  unsigned int n = 0;
  while (Serial.available() > 0)
  {
    input[n] = Serial.read();
    n++;
  }
  input[n] = '\0';

  if (strstr(input, "HEADER"))
  {
    return CMD_HEADER;
  }
  else if (strstr(input, "READROM"))
  {
    return CMD_READ_ROM;
  }
  else if (strstr(input, "READRAM"))
  {
    return CMD_READ_RAM;
  }
  else if (strstr(input, "WRITERAM")) {
    return CMD_WRITE_RAM;
  }
  return CMD_UNKNOWN;
}
