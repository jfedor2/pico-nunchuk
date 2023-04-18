#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "pico/stdlib.h"

#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C i2c1
#define I2C_ADDR 0x52
#define PIN_SDA 2
#define PIN_SCL 3

#ifdef MOUSE
typedef struct __attribute__((packed)) {
    uint8_t buttons;
    int8_t dx;
    int8_t dy;
    int16_t accel[3];
} hid_report_t;
#endif

#ifdef GAMEPAD
typedef struct __attribute__((packed)) {
    uint8_t buttons;
    uint8_t x;
    uint8_t y;
    int16_t accel[3];
} hid_report_t;
#endif

hid_report_t report;
hid_report_t prev_report;

void nunchuk_init() {
    i2c_init(I2C, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
    {
        uint8_t buf[] = { 0xF0, 0x55 };
        i2c_write_blocking(I2C, I2C_ADDR, buf, sizeof(buf), false);
    }
    sleep_ms(1);
    {
        uint8_t buf[] = { 0xFB, 0x00 };
        i2c_write_blocking(I2C, I2C_ADDR, buf, sizeof(buf), false);
    }
}

void nunchuk_read() {
    {
        uint8_t buf[] = { 0x00 };
        i2c_write_blocking(I2C, I2C_ADDR, buf, sizeof(buf), false);
    }
    {
        uint8_t buf[6];
        i2c_read_blocking(I2C, I2C_ADDR, buf, sizeof(buf), false);

        // my nunchuk reports 131 when neutral, but that might be unit-specific

#ifdef MOUSE
        report.dx = (buf[0] - 131) / 8;
        report.dy = -1 * (buf[1] - 131) / 8;
#endif
#ifdef GAMEPAD
        report.x = buf[0];
        report.y = 255 - buf[1];
#endif

        report.buttons = (!(buf[5] & 1) << 1) | (!(buf[5] & 2) << 0);

        report.accel[0] = ((buf[2] << 2) | ((buf[5] & 0b1100) >> 2)) - 511;
        report.accel[1] = ((buf[3] << 2) | ((buf[5] & 0b110000) >> 4)) - 511;
        report.accel[2] = ((buf[4] << 2) | ((buf[5] & 0b11000000) >> 6)) - 511;
    }
}

bool need_to_send() {
#ifdef MOUSE
    return (report.buttons != prev_report.buttons) ||
           (report.dx != 0) ||
           (report.dy != 0) ||
           (report.accel[0] != prev_report.accel[0]) ||
           (report.accel[1] != prev_report.accel[1]) ||
           (report.accel[2] != prev_report.accel[2]);
#endif
#ifdef GAMEPAD
    return (report.buttons != prev_report.buttons) ||
           (report.x != prev_report.x) ||
           (report.y != prev_report.y) ||
           (report.accel[0] != prev_report.accel[0]) ||
           (report.accel[1] != prev_report.accel[1]) ||
           (report.accel[2] != prev_report.accel[2]);
#endif
}

int main(void) {
    board_init();
    nunchuk_init();
    tusb_init();

    memset(&report, 0, sizeof(report));
    memset(&prev_report, 0, sizeof(prev_report));

    while (true) {
        tud_task();
        if (tud_hid_ready()) {
            nunchuk_read();
            if (need_to_send()) {
                tud_hid_report(0, &report, sizeof(report));
                prev_report = report;
            }
        }
    }

    return 0;
}
