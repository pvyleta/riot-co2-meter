/*
 * Copyright (C) 2018 Petr Vyleta - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

/**
 * @ingroup     newlib_syscalls_u8log
 * @{
 *
 * @file		newlib_syscalls_u8log.c
 * @brief       Proprietary u8log stdout implementation
 *
 * @author      Petr Vyleta
 *
 * @}
 */

#include <u8g2.h>
#include <u8g2.h>
#include <u8x8_riotos.h>
#include <periph/gpio.h>
#include <stdbool.h>

#define U8LOG_WIDTH 32
#define U8LOG_HEIGHT 10

static u8g2_t u8g2;
static u8log_t u8log;
static bool is_initialized = false;

static u8x8_riotos_t user_data = {
    .device_index = 0,
    .pin_cs = GPIO_UNDEF,
    .pin_dc = GPIO_PIN(0,20),
    .pin_reset = GPIO_PIN(0,25),
};

void stdio_init(void) {
    u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_riotos_hw_spi, u8x8_gpio_and_delay_riotos);
    u8g2_SetUserPtr(&u8g2, &user_data);
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
    u8g2_SetFont(&u8g2, u8g2_font_tom_thumb_4x6_mf);
    u8log_Init(&u8log, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8log_SetCallback(&u8log, u8log_u8g2_cb, &u8g2);

    u8g2_FirstPage(&u8g2);
    u8g2_SetDrawColor(&u8g2, 1);

    is_initialized = true;
}

ssize_t stdio_read(void* buffer, size_t count) {
    (void) buffer;
    (void) count;
    return 0;
}

ssize_t stdio_write(const void* buffer, size_t len) {
    if (!is_initialized)
    {
        return 0;
    }

    const char *buffer_char = (const char *) buffer;
    for (size_t i = 0; i < len; i++)
    {
      u8log_WriteChar(&u8log, buffer_char[i]);
    }

    return len;
}
