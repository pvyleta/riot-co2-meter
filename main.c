/*
 * Copyright (C) 2018 Petr Vyleta - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

/**
 * @ingroup     app
 * @{
 *
 * @file		main.c
 * @brief       Proprietary-app hello-world demo
 *
 * @author      Petr Vyleta
 *
 * @}
 */

#include <stdio.h>

extern void app(void);

int main(void) {
    printf("Hello world!\n");

    app();

    return 0;
}
