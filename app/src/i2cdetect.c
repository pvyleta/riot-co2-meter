/*
 * Copyright (C) 2018 Petr Vyleta - All Rights Reserved
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

/**
 * @ingroup     app
 * @{
 *
 * @file		i2cdetect.c
 * @brief       RIOT-OS version of i2cdetect
 *
 * @author      Petr Vyleta
 *
 * @}
 */
#include <periph/i2c.h>

#include <assert.h>
#include <errno.h>

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Helper macro to count elements in array
#define COUNTOF(_array) ( sizeof(_array) / sizeof(_array[0]) )

/**
 * Handler for a given option.
 * @param argc number of total options left (including the selected one)
 * @param argv the array of options
 * @return -errno for failure, the number of consumed args on success (including the selected one)
 */
typedef int(*option_handler_t)(int argc, char **argv);

// All possible options - order must be kept as this is used as indexing to option_list
typedef enum
{
    OPTION_HELP = 0,
    OPTION_DEVICE,
    OPTION_LOWER,
    OPTION_UPPER,
    OPTION_READ,
    OPTION_WRITE,
} option_index_t;

// All possible options
static const char OPTION_NAME_HELP[] = "--help";
static const char OPTION_NAME_DEVICE[] = "-d";
static const char OPTION_NAME_LOWER[] = "-l";
static const char OPTION_NAME_UPPER[] = "-u";
static const char OPTION_NAME_READ[] = "-r";
static const char OPTION_NAME_WRITE[] = "-w";

// Handlers for the options
static int option_help(int argc, char **argv);
static int option_device(int argc, char **argv);
static int option_lower(int argc, char **argv);
static int option_upper(int argc, char **argv);
static int option_read(int argc, char **argv);
static int option_write(int argc, char **argv);

// Helpers
static int set_option(int *selected_option, const char *input_option);
static void set_options_default(void);
static void i2cdetect_scan(void);

static int selected_device;
static int selected_lower;
static int selected_upper;
static bool selected_write;
static bool selected_read;

// Structure holding necessary info about one option
typedef struct
{
    size_t option_length; // Length of string option_name
    const char *option_name; // String to invoke the option
    const char *option_help; // Help text for the option
    const char *option_default; // Default value for the option, null if option_has_value == false
    bool option_has_value; // If value follows the option or not
    option_handler_t option_handler;
} option_t;

static const option_t option_list[] =
{
    {
        .option_length = sizeof(OPTION_NAME_HELP),
        .option_name = OPTION_NAME_HELP,
        .option_help = "Print this help",
        .option_default = NULL,
        .option_has_value = false,
        .option_handler = option_help
    },
    {
        .option_length = sizeof(OPTION_NAME_DEVICE),
        .option_name = OPTION_NAME_DEVICE,
        .option_help = "Index of the I2C bus",
        .option_default = "0",
        .option_has_value = true,
        .option_handler = option_device
    },
    {
        .option_length = sizeof(OPTION_NAME_LOWER),
        .option_name = OPTION_NAME_LOWER,
        .option_help = "Lowest address to scan",
        .option_default = "0x02",
        .option_has_value = true,
        .option_handler = option_lower
    },
    {
        .option_length = sizeof(OPTION_NAME_UPPER),
        .option_name = OPTION_NAME_UPPER,
        .option_help = "Highest address to scan",
        .option_default = "0x77",
        .option_has_value = true,
        .option_handler = option_upper
    },
    {
        .option_length = sizeof(OPTION_NAME_READ),
        .option_name = OPTION_NAME_READ,
        .option_help = "Use only read operation",
        .option_default = NULL,
        .option_has_value = false,
        .option_handler = option_read
    },
    {
        .option_length = sizeof(OPTION_NAME_WRITE),
        .option_name = OPTION_NAME_WRITE,
        .option_help = "Use only write operation",
        .option_default = NULL,
        .option_has_value = false,
        .option_handler = option_write
    }
};

static int option_help(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    for (size_t i = 0; i < COUNTOF(option_list); i++)
    {
        const option_t *const opt_ptr = &option_list[i];

        printf("%s\t%s%s%s\n",
                opt_ptr->option_name,
                opt_ptr->option_help,
                opt_ptr->option_has_value ? " Default: " : "",
                opt_ptr->option_has_value ? opt_ptr->option_default : "");
    }

    return 1;
}

static int option_device(int argc, char **argv)
{
    assert(argc > 1);
    assert(NULL != argv);
    assert(NULL != argv[1]);

    return 0 == set_option(&selected_device, argv[1]) ? 2 : -1;
}

static int option_lower(int argc, char **argv)
{
    assert(argc > 1);
    assert(NULL != argv);
    assert(NULL != argv[1]);

    return 0 == set_option(&selected_lower, argv[1]) ? 2 : -1;
}

static int option_upper(int argc, char **argv)
{
    assert(argc > 1);
    assert(NULL != argv);
    assert(NULL != argv[1]);

    return 0 == set_option(&selected_upper, argv[1]) ? 2 : -1;
}

static int option_read(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    selected_read = true;

    return 1;
}

static int option_write(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    selected_write = true;

    return 1;
}

static int set_option(int *selected_option, const char *input_option)
{
    assert(NULL != selected_option);
    assert(NULL != input_option);

    errno = 0; // To detect the errors of strtoll

    const long temp_option = strtoll(input_option, NULL, 0);

    if (0 == errno)
    {
        *selected_option = temp_option;
    }

    return -errno;
}

static void set_options_default(void)
{
    int ret_val = 0;

    ret_val = set_option(&selected_device, option_list[OPTION_DEVICE].option_default);
    assert(0 == ret_val);

    ret_val = set_option(&selected_lower, option_list[OPTION_LOWER].option_default);
    assert(0 == ret_val);

    ret_val = set_option(&selected_upper, option_list[OPTION_UPPER].option_default);
    assert(0 == ret_val);

    selected_read = false;
    selected_write = false;
}

static void i2cdetect_scan(void)
{
    /* Assuming device is correct, it should return 1 (nr bytes) */
    i2c_acquire(I2C_DEV(selected_device));
    puts("WARNING! This program may disrupt your I2C bus and worse!\n");
    puts("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

    const int first_row = selected_lower / 16;
    const int last_row  = selected_upper / 16;

    for (int row = first_row; row <= last_row; row ++)
    {
        printf("%02x:", row * 16);

        const int first_column = (row == first_row) ? selected_lower % 16 : 0;
        const int last_column  = (row == last_row)  ? selected_upper % 16 : 15;

        for (int column = 0; column <= 16; column++)
        {
          if (column < first_column)
          {
              printf("   ");
          } else if (column <= last_column)
          {
              const int address = row * 16 + column;
              uint16_t read_data = 0;
              const int result = i2c_read_reg(I2C_DEV(selected_device), address, 0, &read_data, 0);

              if (0 == result)
              {
                  printf(" %02x", address);
              }
              else
              {
                  printf(" --");
              }
          }
        }

        printf("\n");
    }

    i2c_release(I2C_DEV(selected_device));
}

int i2cdetect(int argc, char **argv)
{
    // TODO Reject duplicit options
    // TODO Input sanitization

    set_options_default();

    // Iterate over all user-options
    for (int arg_index = 1; arg_index < argc;)
    {
        const char *const option = argv[arg_index];
        bool is_option_known = false;

        // Iterate over all known options
        for (size_t option_index = 0; option_index < COUNTOF(option_list); option_index++)
        {
            const option_t *const opt_ptr = &option_list[option_index];
            if (0 == strncmp(option, opt_ptr->option_name, opt_ptr->option_length))
            {
                is_option_known = true;
                const size_t args_remaining = argc - arg_index;

                if ((opt_ptr->option_has_value) && (args_remaining <= 1))
                {
                    printf("ERROR: Option %s needs a parameter", opt_ptr->option_name);
                }

                ssize_t args_consumed = opt_ptr->option_handler(args_remaining, &argv[arg_index]);
                if (-1 == args_consumed)
                {
                    option_help(0, NULL);
                    return -1;
                }

                arg_index += args_consumed;
                break;
            }
        }

        if (!is_option_known)
        {
            printf("ERROR: Unknown option '%s'\n", option);
            option_help(0, NULL);
            return -1;
        }
    }

    // All options processed, all checks passed, we can execute the scan
    i2cdetect_scan();

    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
