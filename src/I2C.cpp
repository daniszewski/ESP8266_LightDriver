#include "I2C.h"
#include <queue>
#include "LiquidCrystal_I2C.h"

std::deque<LiquidCrystal_I2C> i2clist;

LiquidCrystal_I2C* getI2C(uint8_t address) {
    for (std::size_t i=0; i<i2clist.size(); ++i) {
        if (i2clist[i].getAddr() == address) return &i2clist[i];
    }
    return nullptr;
}

void I2CClass::begin() {
}

void I2CClass::handle() {
}

void I2CClass::HD44780_add(uint8_t address, uint8_t width, uint8_t height) {
    if (getI2C(address)) return;
    LiquidCrystal_I2C lcd(address, width, height);
    lcd.init();
    lcd.setCursor(0, 0);
    i2clist.push_back(lcd);
}

void I2CClass::HD44780_font(uint8_t address, uint8_t location, uint8_t r0, uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7) {
    auto instance = getI2C(address);
    if (!instance) return;
    uint8_t arr[8] = { r0, r1, r2, r3, r4, r5, r6, r7 }; 
    instance->createChar(location, arr);
}

void I2CClass::HD44780_cursor(uint8_t address, uint8_t x, uint8_t y) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->setCursor(x, y);
}

void I2CClass::HD44780_print(uint8_t address, String text) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->print(text);
}

void I2CClass::HD44780_printCustom(uint8_t address, String text) {
    auto instance = getI2C(address);
    if (!instance) return;
    auto arr = text.c_str();
    for(unsigned int i=0; i<text.length(); ++i) {
        instance->print((char)(arr[i]-'0'));
    }
}

void I2CClass::HD44780_backlight(uint8_t address, uint8_t isOn) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->setBacklight(isOn);
}

void I2CClass::HD44780_printbig(uint8_t address, String text) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->printBig(text.c_str());
}

void I2CClass::HD44780_bigchar(uint8_t address, uint8_t c, String seq) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->setBigChar(c, seq.c_str());
}

void I2CClass::HD44780_clear(uint8_t address) {
    auto instance = getI2C(address);
    if (!instance) return;
    instance->clear();
}

I2CClass I2C;
