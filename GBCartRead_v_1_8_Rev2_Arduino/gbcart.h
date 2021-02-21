#ifndef GBCART_H_
#define GBCART_H_

struct Cartridge
{
  char* gameTitle;
  uint16_t cartridgeType;
  uint16_t romSize;
  uint16_t romBanks;
  uint16_t ramSize;
  uint16_t ramBanks;
  uint16_t ramEndAddress;
  uint8_t logoCheck;
  uint8_t isRead;
};

uint16_t readCartridgeType();
uint16_t readRomSize();
uint16_t readRamSize();
uint8_t nintendoLogoCheck();
uint8_t romBanks(uint8_t readRomSize);
uint8_t ramBanks(uint8_t cartridgeType, uint8_t readRamSize);
uint16_t ramEndAddress(uint8_t cartridgeType, uint8_t readRamSize);
char* getGameTitle();
struct Cartridge readHeader();

#endif /* GBCART_H_ */
