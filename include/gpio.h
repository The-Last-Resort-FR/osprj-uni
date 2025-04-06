/**
 * @file gpio.h
 * @author TLR
 * @brief Header of the gpio related functions
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#define GPIO_CHIP "/dev/gpiochip0"

void* setLedPin(void* args);
int initUART(const char *device, speed_t baudrate);
void uartSendString(const char *str);
void closeUART();