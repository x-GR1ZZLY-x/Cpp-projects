#include "Eeprom.h"
#include <algorithm>

Eeprom::Eeprom(SpiDriver& driver) : SpiDevice(driver){}

uint8_t Eeprom::makeOpcode(Command cmd, uint16_t address) {
    uint8_t opcode = static_cast<uint8_t>(cmd);
    if (address & 0x100) {
        opcode |= OPCODE_ADDR_BIT;
    }
    return opcode;
}

void Eeprom::writeEnable(){
    select();
    transfer(WREN);
    deselect();
}

uint8_t Eeprom::readStatus(){
    select();
    transfer(RDSR);
    uint8_t status = transfer(0xFF);
    deselect();
    return status;
}

bool Eeprom::waitWriteComplete(uint16_t timeoutMS) {
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::milliseconds(timeoutMS);
    while ((readStatus() & STATUS_WIP) != 0) {
        if (std::chrono::steady_clock::now() >= end) {
            return false;
        }
        spi.delayUS(10);
    }
    return true;
}

bool Eeprom::writeByte(uint16_t address, uint8_t value){
    if (address >= MEMORY_SIZE){
        return false;
    }

    writeEnable();

    select();
    transfer(makeOpcode(WRITE, address));
    transfer(address & 0xFF);
    transfer(value);
    deselect();

    return waitWriteComplete();
}

bool Eeprom::readByte(uint16_t address, uint8_t& value){
    if (address >= MEMORY_SIZE){
        return false;
    }

    select();
    transfer(makeOpcode(READ, address));
    transfer(address & 0xFF);
    value = transfer(0xFF);
    deselect();

    return true;
}

bool Eeprom::writeBuffer(uint16_t address, const uint8_t* data, uint16_t length){
    if(!data || length == 0){
        return false;
    }
    if (address >= MEMORY_SIZE || length > (MEMORY_SIZE - address)){
        return false;
    }
    uint16_t offset = 0;
    while (offset < length){
        uint16_t currentAddress = address + offset;
        uint16_t pageOffset = currentAddress % PAGE_SIZE;
        uint16_t bytesInPage = PAGE_SIZE - pageOffset;
        uint16_t bytesToWrite = std::min<uint16_t>(length - offset, bytesInPage);

        writeEnable();

        select();
        transfer(makeOpcode(WRITE, currentAddress));
        transfer(currentAddress & 0xFF);

        for (uint16_t i = 0; i < bytesToWrite; ++i){
            transfer(data[offset + i]);
        }
        deselect();
        
        if(!waitWriteComplete()){
            return false;
        }

        offset += bytesToWrite;
    }
    return true;
}

bool Eeprom::readBuffer(uint16_t address, uint8_t* data, uint16_t length){
    if (!data || length == 0) {
        return false;
    }
    if (address >= MEMORY_SIZE || length > (MEMORY_SIZE - address)) {
        return false;
    }

    select();

    transfer(makeOpcode(READ, address));
    transfer(address & 0xFF);
    for (uint16_t i = 0; i < length; ++i) {
        data[i] = transfer(0xFF);
    }
    deselect();

    return true;
}

bool Eeprom::writeBit(uint16_t address, uint8_t bit, bool value){
    if (address >= MEMORY_SIZE || bit > 7) {
        return false;
    }

    uint8_t byte;
    if (!readByte(address, byte)) {
        return false;
    }

    if (value) {
        byte |= (1U << bit);
    } else {
        byte &= ~(1U << bit);
    }

    return writeByte(address, byte);
}

bool Eeprom::readBit(uint16_t address, uint8_t bit, bool& value){
    if (address >= MEMORY_SIZE || bit > 7) {
        return false;
    }

    uint8_t byte;
    if (!readByte(address, byte)) {
        return false;
    }

    value = (byte >> bit) & 1U;
    return true;
}