/**
 * @file sensor.c
 * @author TLR
 * @brief Implementation of the sensor related functions
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <sensor.h>
#include <errno.h>
#include <string.h>
#include <gpio.h>

/// @brief Internal I2C file descriptor
static int i2cFile = 0;

/// @brief The global data from main.c
extern GlobalData gd;

/**
 * @brief Adds a full aquisition into a SensorDataArray in a sort of vector style
 * 
 * @param _timestamp
 * @param _humidity 
 * @param _temperature 
 */
void add(time_t _timestamp, float _humidity, float _temperature) {
	if(gd.data.size >= gd.data._size) {
		SensorData** sd = (SensorData**)malloc(sizeof(SensorData*) * (gd.data.size + 10));
		for(uint32_t i = 0; i < gd.data.size; i++) {
			sd[i] = gd.data.dataArray[i];
		}
		free(gd.data.dataArray);
		gd.data.dataArray = sd;
		gd.data._size = gd.data.size + 10;
	}
	gd.data.dataArray[gd.data.size] = (SensorData*)malloc(sizeof(SensorData));
	gd.data.dataArray[gd.data.size]->timestamp = _timestamp;
	gd.data.dataArray[gd.data.size]->humidity = _humidity;
	gd.data.dataArray[gd.data.size]->temperature = _temperature;
	gd.data.size++;
	char uartLog[100];
	sprintf(uartLog, "sensor data recorded {\n\ttime: %ld\n\thumidity: %.1f\n\ttemperature: %.1f\n}\n", _timestamp, _humidity, _temperature);
	uartSendString(uartLog);
	if(_temperature > TEMP_UP_LIMIT || _temperature < TEMP_DOWN_LIMIT || _humidity > HUM_UP_LIMIT ||  _humidity < HUM_DOWN_LIMIT) gd.ledState = 1;
}

/**
 * @brief Gets a SensorData* to the requested index from the global data
 * 
 * @param index 
 * @return SensorData* 
 */
SensorData* get(uint32_t index) {
	if(index >= gd.data.size) {
		return NULL;
	}
	return gd.data.dataArray[index];
}

/**
 * @brief Free all the ressources taken by the global data SensorDataArray
 * 
 * @param _gd 
 */
void clean(GlobalData _gd) {
	for (size_t i = 0; i < _gd.data.size; i++)
	{
		free(_gd.data.dataArray[i]);
	}
	free(_gd.data.dataArray);
}

/**
 * @brief Opens the I2C bus and set the slave address
 * 
 * @param bus String to the I2C bus
 */
void initSensor(const char* bus) {
	if ((i2cFile = open(bus, O_RDWR)) < 0) {
		syslog(LOG_ERR, "failed to open i2c bus\n");
	}

	int slaveAddr = 0x40;

	if (ioctl(i2cFile, I2C_SLAVE, slaveAddr) < 0) {
		syslog(LOG_ERR, "failed to communicate with slave\n");
	}
}

/**
 * @brief Reads the temperature from the sensor
 * 
 * @return float 
 */
float readTemp() {
    uint8_t cmd = 0xE3;
	if (write(i2cFile, &cmd, sizeof(cmd)) != sizeof(cmd)) {
		syslog(LOG_ERR, "failed to send command\n");
		return 0;
	}

	uint8_t expectedLength = 2;
	uint16_t answer = 0;
	if (read(i2cFile, &answer, expectedLength) != expectedLength) {
		syslog(LOG_ERR, "failed reading the data\n");
		return 0;
	}
	else {
		// magic
		uint8_t* byte = (uint8_t*)&answer;
		uint16_t result = byte[0] << 8 | byte[1];
		return DataToTemp(result);
	}
}

/**
 * @brief Reads the humidity from the sensor
 * 
 * @return float 
 */
float readHum() {
    uint8_t cmd = 0xE5;
	if (write(i2cFile, &cmd, sizeof(cmd)) != sizeof(cmd)) {
		syslog(LOG_ERR, "failed to send command\n");
		return 0;
	}

	uint8_t expectedLength = 2;
	uint16_t answer = 0;
	if (read(i2cFile, &answer, expectedLength) != expectedLength) {
		syslog(LOG_ERR, "failed reading the data: %s\n", strerror(errno));
		return 0;
	}
	else {
		// magic
		uint8_t* byte = (uint8_t*)&answer;
		uint16_t result = byte[0] << 8 | byte[1];
		return DataToHum(result);
	} 
}

/**
 * @brief Helper function to convert the raw temperature data to a float
 * 
 * @param d 
 * @return float 
 */
float DataToTemp(uint16_t d) {
    return d*175.72f / 65536 - 46.85f;
}

/**
 * @brief Helper function to convert the raw humidity data to a float
 * 
 * @param d 
 * @return float 
 */
float DataToHum(uint16_t d) {
    return ((125.0f*d) / 65536) - 6;
}

/**
 * @brief Closes the I2C bus
 * 
 */
void deinitSensor() {
	close(i2cFile);
}

/**
 * @brief Worker that periodically add a measurement to the global data SensorDataArray
 * 
 * @param args unused
 * @return void* args
 */
void* sensorDataLogger(void* args) {
	initSensor("/dev/i2c-1");
	while (!gd.shouldStop)
	{
		add(time(NULL), readHum(), readTemp());
		sleep(AQUISITION_INTERVAL);
	}
	deinitSensor();
	return args;
}