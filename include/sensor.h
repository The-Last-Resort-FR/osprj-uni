/**
 * @file sensor.h
 * @author TLR
 * @brief Header of the sensor related functions
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

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
#include <syslog.h>

#define AQUISITION_INTERVAL 30
#define TEMP_UP_LIMIT 30
#define TEMP_DOWN_LIMIT 5
#define HUM_UP_LIMIT 95
#define HUM_DOWN_LIMIT 10

/**
 * @brief A single aquisition of the sensor
 * 
 */
typedef struct SensorData {
    time_t timestamp;
    float humidity;
    float temperature;
}SensorData;

/**
 * @brief The array of all the sensor's aquisitions
 * 
 */
typedef struct SensorDataArray {
    uint32_t size;
    uint32_t _size;
    SensorData** dataArray;
}SensorDataArray;

/**
 * @brief The global data structure
 * 
 */
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