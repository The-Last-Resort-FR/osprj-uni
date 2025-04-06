#pragma once

#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define GPIO_CHIP "/dev/gpiochip0"

void* setLedPin(void* args);
int initUART(const char *device, speed_t baudrate);
void uartSendString(const char *str);
void closeUART();