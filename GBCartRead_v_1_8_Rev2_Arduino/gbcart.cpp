#include "io.h"
#include "gbcart.h"

/**
 * Reads the rom size from 0x148
 * 0 - 32K 
 * 1 - 64K
 * 2 - 128K
 * etc...
 **/
uint16_t readRomSize()
{
    return readData(0x148);
}

/**
 * Reads the ram size from 0x149
 * 0 - None
 * 1 - 2K
 * 2 - 8K
 * 3 - 32KB (4 banks of 8Kbytes each)
 * etc...
 **/
uint16_t readRamSize()
{
    return readData(0x149);
}

/**
 * 0x0 : rom only
 * 0x1 : mbc1
 * 0x2 : mbc1 + ram
 * 0x3 : mbc1 + ram + battery
 * 0x4 : mbc2
 * 0x5 : mbc2 + battery
 * 0x6 : rom + ram
 * 0x8 : rom + ram + battery
 * 0xB : MMM01
 * 0xC : MMM01 + RAM
 * 0xD : MMM01 + RAM + battery
 * 0xF : MBC3 + timer + battery
 * 0x10: MBC3 + timer + ram + battery
 * 0x11: MBC3
 * 0x12: MBC3 + RAM
 * 0x13: MBC3 + RAM + battery
 * 0x15: MBC4
 * 0x16: MBC4 + RAM
 * 0x17: MBC4 + RAM + battery
 * 0x19: MBC5
 * 0x1A: MBC5 + RAM
 * 0x1B: MBC5 + RAM + battery
 * 0x1C: MBC5 + Rumble
 * 0x1D: MBC5 + Rumble + RAM
 * 0x1E: MBC5 + Rumble + RAM + Battery
 * 0xFC: Pocket camera
 * 0xFD: Bandai Tama5
 * 0xFE: HuC3
 * 0xFF: HuC1 + RAM + battery
 **/
uint16_t readCartridgeType()
{
    return readData(0x147);
}

uint8_t nintendoLogo[] = {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
                          0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
                          0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

uint8_t nintendoLogoCheck()
{
    uint8_t x = 0;
    for (uint16_t romAddress = 0x0104; romAddress <= 0x133; romAddress++)
    {
        if (nintendoLogo[x] != readData(romAddress))
        {
            return 0;
            break;
        }
        x++;
    }
    return 1;
}

uint8_t romBanks(uint8_t readRomSize)
{
    uint8_t romBanks = 2;
    if (readRomSize >= 1)
    { // Calculate rom size
        romBanks = 2 << readRomSize;
    }
    return romBanks;
}

uint8_t ramBanks(uint8_t cartridgeType, uint8_t readRamSize)
{
    if (cartridgeType == 6)
    {
        return 1;
    }
    if (readRamSize == 2)
    {
        return 1;
    }
    if (readRamSize == 3)
    {
        return 4;
    }
    if (readRamSize == 4)
    {
        return 16;
    }
    if (readRamSize == 5)
    {
        return 8;
    }

    return 0; // Default 0K RAM
}

uint16_t ramEndAddress(uint8_t cartridgeType, uint8_t readRamSize)
{
    if (cartridgeType == 6)
    {
        return 0xA1FF;
    } // MBC2 512bytes (nibbles)
    if (readRamSize == 1)
    {
        return 0xA7FF;
    } // 2K RAM
    if (readRamSize > 1)
    {
        return 0xBFFF;
    } // 8K RAM

    return 0;
}

char *getGameTitle()
{
    char *gameTitle = (char *)malloc(sizeof(char) * 17);
    for (uint16_t romAddress = 0x0134; romAddress <= 0x143; romAddress++)
    {
        char headerChar = (char)readData(romAddress);
        gameTitle[(romAddress - 0x0134)] = headerChar;
    }
    gameTitle[16] = '\0';

    return gameTitle;
}

struct Cartridge readHeader()
{
    // Read cartridge title and check for non-printable text
    struct Cartridge cart;

    cart.gameTitle = getGameTitle();

    // Nintendo Logo Check
    cart.logoCheck = nintendoLogoCheck();

    cart.cartridgeType = readCartridgeType();
    cart.romSize = readRomSize();
    cart.ramSize = readRamSize();

    // ROM banks
    cart.romBanks = romBanks(cart.romSize);

    // RAM banks
    cart.ramBanks = ramBanks(cart.cartridgeType, cart.ramSize);

    // RAM end address
    cart.ramEndAddress = ramEndAddress(cart.cartridgeType, cart.ramSize);

    return cart;
}