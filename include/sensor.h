#pragma once

#include <gpiod.h>
#include <linux/i2c-dev.h>	
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

void initSensor(const char* bus);
float readTemp();
float readHum();
float DataToTemp(uint16_t d);
float DataToHum(uint16_t d);
void deinitSensor();