#pragma once

#include <gpiod.h>
#include <linux/i2c-dev.h>	
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define AQUISITION_INTERVAL 1

typedef struct SensorData {
    time_t timestamp;
    float humidity;
    float temperature;
}SensorData;

typedef struct SensorDataArray {
    uint32_t size;
    uint32_t _size;
    SensorData** dataArray;
}SensorDataArray;

typedef struct GlobalData {
    uint8_t ledState;
    uint8_t shouldStop;
    struct SensorDataArray data;
}GlobalData;

void add(time_t time, float _humidity, float _temperature);
void clean(GlobalData _gd);


void initSensor(const char* bus);
float readTemp();
float readHum();
float DataToTemp(uint16_t d);
float DataToHum(uint16_t d);
void deinitSensor();
void* sensorDataLogger(void* args);