/*
 * Copyright 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drivers/dbgserial.h"
#include "board/board_esp32c3.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// ESP-IDF includes
#include "driver/uart.h"
#include "driver/gpio.h"

#define UART_NUM UART_NUM_0
#define BUF_SIZE 1024

static bool uart_initialized = false;

void dbgserial_init(void) {
    if (uart_initialized) {
        return;
    }

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Install UART driver
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_initialized = true;
}

void dbgserial_print(const char* str) {
    if (!uart_initialized) {
        dbgserial_init();
    }
    if (str) {
        uart_write_bytes(UART_NUM, str, strlen(str));
    }
}

void dbgserial_putstr(const char* str) {
    dbgserial_print(str);
    dbgserial_newline();
}

void dbgserial_newline(void) {
    dbgserial_print("\r\n");
}

void dbgserial_print_hex(uint32_t value) {
    char hex_str[11];
    snprintf(hex_str, sizeof(hex_str), "0x%08lx", value);
    dbgserial_print(hex_str);
}

void dbgserial_putstr_fmt(char* buffer, unsigned int buffer_size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, buffer_size, fmt, args);
    va_end(args);
    dbgserial_print(buffer);
}

