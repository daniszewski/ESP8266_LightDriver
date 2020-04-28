#ifndef I2C_H
#define I2C_H

#include "Global.h"

class I2CClass {
public:
    void begin();
    void handle();
    void HD44780_add(uint8_t address, uint8_t width, uint8_t height);
    void HD44780_font(uint8_t address, uint8_t location, uint8_t r0, uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7);
    void HD44780_cursor(uint8_t address, uint8_t x, uint8_t y);
    void HD44780_print(uint8_t address, String text);
    void HD44780_printCustom(uint8_t address, String text);
    void HD44780_backlight(uint8_t address, uint8_t isOn);
    void HD44780_printbig(uint8_t address, String text);
    void HD44780_bigchar(uint8_t address, uint8_t c, String seq);
private:

};

extern I2CClass I2C;

#endif
