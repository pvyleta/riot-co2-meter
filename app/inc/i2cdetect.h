/*
 * Copyright (C) 2018 Petr Vyleta - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

/**
 * @ingroup     app
 * @{
 *
 * @file		i2cdetect.h
 * @brief       RIOT-OS version of i2cdetect
 *
 * @author      Petr Vyleta
 *
 * @}
 */

#ifndef APP_I2CDETECT_H
#define APP_I2CDETECT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Performs scan of selected I2C bus. Type --help for help
 */
int i2cdetect(int argc, char **argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // APP_I2CDETECT_H
