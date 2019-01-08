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

#include <stdbool.h>

#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10

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

    /* start drawing in a loop */
    puts("Drawing on screen.");

    while (true) {
        u8g2_FirstPage(&u8g2);
        u8g2_SetDrawColor(&u8g2, 1);

        u8log_WriteString(&u8log, "12345678901234567890123456789012\n");

        /* sleep a little */
        xtimer_sleep(1);
    }
}
