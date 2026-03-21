#pragma once

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * HD44780 1602A LCD driven via a PCF8574 I2C backpack (QA Pass style).
 *
 * Default I2C address: 0x27 (A0=A1=A2=1).
 * If your board uses 0x3F, pass that instead.
 *
 * PCF8574 bit mapping (standard wiring):
 *   P7 P6 P5 P4  P3       P2  P1  P0
 *   D7 D6 D5 D4  BL(+5V)  EN  RW  RS
 */
class LCD_I2C {
public:
    static constexpr uint8_t DEFAULT_ADDR = 0x27;

    explicit LCD_I2C(uint8_t addr = DEFAULT_ADDR, const char* dev = "/dev/i2c-1")
        : _addr(addr)
    {
        _fd = open(dev, O_RDWR);
        if (_fd < 0) return;
        if (ioctl(_fd, I2C_SLAVE, _addr) < 0) { close(_fd); _fd = -1; return; }
        init();
    }

    ~LCD_I2C() { if (_fd >= 0) close(_fd); }

    bool ok() const { return _fd >= 0; }

    void clear()    { cmd(0x01); usleep(2000); }
    void home()     { cmd(0x02); usleep(2000); }

    /** Move cursor: row 0 or 1, col 0..15 */
    void setCursor(uint8_t col, uint8_t row)
    {
        static const uint8_t row_offsets[] = {0x00, 0x40};
        cmd(0x80 | (col + row_offsets[row & 1]));
    }

    void print(const char* str)
    {
        while (*str) write_char(static_cast<uint8_t>(*str++));
    }

    void setBacklight(bool on) { _backlight = on ? BL : 0; }

private:
    static constexpr uint8_t EN = 0x04;
    static constexpr uint8_t RW = 0x02;
    static constexpr uint8_t RS = 0x01;
    static constexpr uint8_t BL = 0x08;

    int     _fd       = -1;
    uint8_t _addr;
    uint8_t _backlight = BL;   // backlight on by default

    void send_byte(uint8_t data)
    {
        uint8_t buf = data | _backlight;
        write(_fd, &buf, 1);
    }

    void pulse_enable(uint8_t data)
    {
        send_byte(data | EN);
        usleep(1);
        send_byte(data & ~EN);
        usleep(50);
    }

    void write4(uint8_t nibble, uint8_t mode)
    {
        uint8_t data = (nibble & 0xF0) | mode;
        pulse_enable(data);
    }

    void send(uint8_t value, uint8_t mode)
    {
        write4(value & 0xF0, mode);
        write4((value << 4) & 0xF0, mode);
    }

    void cmd(uint8_t value)        { send(value, 0);  }
    void write_char(uint8_t value) { send(value, RS); }

    void init()
    {
        usleep(50000);              // wait for LCD power-up
        // initialise in 4-bit mode (HD44780 init sequence)
        write4(0x30, 0); usleep(4500);
        write4(0x30, 0); usleep(4500);
        write4(0x30, 0); usleep(150);
        write4(0x20, 0);            // switch to 4-bit

        cmd(0x28);  // 4-bit, 2 lines, 5x8 font
        cmd(0x0C);  // display on, cursor off, blink off
        cmd(0x06);  // entry mode: increment, no shift
        clear();
    }
};
