/*
 * Copyright (C) 2018 Petr Vyleta - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

/**
 * @ingroup     app
 * @{
 *
 * @file		app.c
 * @brief       Proprietary-app hello-world demo
 *
 * @author      Petr Vyleta
 *
 * @}
 */

#include <app.h>
#include <u8g2.h>
#include <u8x8_riotos.h>
#include <led.h>
#include <xtimer.h>
#include "i2cdetect.c"

#if MY_SHELL == 1
#include <shell.h>
#endif

#include <bmx280_params.h>
#include <bmx280.h>
#include <bmx280_internals.h>

#include <stdbool.h>

#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10

#define MAINLOOP_DELAY  (2 * 1000 * 1000u)      /* 2 seconds delay between printf's */

static int bme280(void);

static const shell_command_t shell_commands[] = {
    { "i2cdetect", "Scan I2C bus for devices", i2cdetect},
    { NULL, NULL, NULL }
};

void app(void) {
	LED_ON(0);
	LED_ON(1);
	LED_ON(2);

	u8g2_t u8g2;

    u8x8_riotos_t user_data =
    {
        .device_index = 0,
        .pin_cs = GPIO_UNDEF,
        .pin_dc = GPIO_PIN(0,20),
        .pin_reset = GPIO_PIN(0,25),
    };

    u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);
    u8g2_SetUserPtr(&u8g2, &user_data);
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(&u8g2, 0); // wake up display

	u8log_t u8log;
	uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

	u8g2_SetFont(&u8g2, u8g2_font_tom_thumb_4x6_mf);
	u8log_Init(&u8log, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8log_SetCallback(&u8log, u8log_u8g2_cb, &u8g2);
    u8g2_FirstPage(&u8g2);
    u8g2_SetDrawColor(&u8g2, 1);

    puts("Drawing on screen.");
    u8log_WriteString(&u8log, "12345678901234567890123456789012\n");

#if MY_SHELL == 1
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, sizeof(line_buf));
#endif

    if(0)bme280();
}

static int bme280(void)
{
    bmx280_t dev;
    int result;

    puts("BMX280 test application\n");

    printf("+------------Initializing------------+\n");
    result = bmx280_init(&dev, &bmx280_params[0]);
    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }

    if (result == -2) {
        printf("[Error] The sensor did not answer correctly at address 0x%02X\n", bmx280_params[0].i2c_addr);
        return 1;
    }

    printf("Initialization successful\n\n");

    printf("+------------Calibration Data------------+\n");
    printf("dig_T1: %u\n", dev.calibration.dig_T1);
    printf("dig_T2: %i\n", dev.calibration.dig_T2);
    printf("dig_T3: %i\n", dev.calibration.dig_T3);

    printf("dig_P1: %u\n", dev.calibration.dig_P1);
    printf("dig_P2: %i\n", dev.calibration.dig_P2);
    printf("dig_P3: %i\n", dev.calibration.dig_P3);
    printf("dig_P4: %i\n", dev.calibration.dig_P4);
    printf("dig_P5: %i\n", dev.calibration.dig_P5);
    printf("dig_P6: %i\n", dev.calibration.dig_P6);
    printf("dig_P7: %i\n", dev.calibration.dig_P7);
    printf("dig_P8: %i\n", dev.calibration.dig_P8);
    printf("dig_P9: %i\n", dev.calibration.dig_P9);

#if defined(MODULE_BME280)
    printf("dig_H1: %u\n", dev.calibration.dig_H1);
    printf("dig_H2: %i\n", dev.calibration.dig_H2);
    printf("dig_H3: %i\n", dev.calibration.dig_H3);
    printf("dig_H4: %i\n", dev.calibration.dig_H4);
    printf("dig_H5: %i\n", dev.calibration.dig_H5);
    printf("dig_H6: %i\n", dev.calibration.dig_H6);
#endif

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        int16_t temperature;
        uint32_t pressure;
#if defined(MODULE_BME280)
        uint16_t humidity;
#endif

        /* Get temperature in centi degrees Celsius */
        temperature = bmx280_read_temperature(&dev);
        bool negative = (temperature < 0);
        if (negative) {
            temperature = -temperature;
        }

        /* Get pressure in Pa */
        pressure = bmx280_read_pressure(&dev);

#if defined(MODULE_BME280)
        /* Get pressure in %rH */
        humidity = bme280_read_humidity(&dev);
#endif

        printf("Temperature [°C]:%c%d.%d\n"
               "Pressure [Pa]: %lu\n"
#if defined(MODULE_BME280)
               "Humidity [%%rH]: %u.%02u\n"
#endif
               "\n+-------------------------------------+\n",
               (negative) ? '-' : ' ',
               temperature / 100, (temperature % 100) / 10,
#if defined(MODULE_BME280)
               (unsigned long)pressure,
               (unsigned int)(humidity / 100), (unsigned int)(humidity % 100)
#else
               (unsigned long)pressure
#endif
               );

        xtimer_usleep(MAINLOOP_DELAY);
    }

    return 0;
}
