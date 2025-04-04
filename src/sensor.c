#include <sensor.h>
#include <errno.h>
#include <string.h>

static int i2cFile = 0;

void initSensor(const char* bus) {
	if ((i2cFile = open(bus, O_RDWR)) < 0) {
		printf("failed to open i2c bus\n");
	}

	int slaveAddr = 0x40;

	if (ioctl(i2cFile, I2C_SLAVE, slaveAddr) < 0) {
		printf("failed to communicate with slave\n");
	}
}

float readTemp() {
    uint8_t cmd = 0xE3;
	if (write(i2cFile, &cmd, sizeof(cmd)) != sizeof(cmd)) {
		printf("failed to send command\n");
		return 0;
	}

	uint8_t expectedLength = 2;
	uint16_t answer = 0;
	if (read(i2cFile, &answer, expectedLength) != expectedLength) {
		printf("failed reading the data\n");
		return 0;
	}
	else {
		// magic
		uint8_t* byte = (uint8_t*)&answer;
		uint16_t result = byte[0] << 8 | byte[1];
		return DataToTemp(result);
	}
}

float readHum() {
    uint8_t cmd = 0xE5;
	if (write(i2cFile, &cmd, sizeof(cmd)) != sizeof(cmd)) {
		printf("failed to send command\n");
		return 0;
	}

	uint8_t expectedLength = 2;
	uint16_t answer = 0;
	if (read(i2cFile, &answer, expectedLength) != expectedLength) {
		printf("failed reading the data: %s\n", strerror(errno));
		return 0;
	}
	else {
		// magic
		uint8_t* byte = (uint8_t*)&answer;
		uint16_t result = byte[0] << 8 | byte[1];
        printf("dbg %d\n", answer);
        printf("dbg %d\n", result);
		return DataToHum(result);
	} 
}

float DataToTemp(uint16_t d) {
    return d*175.72f / 65536 - 46.85f;
}

float DataToHum(uint16_t d) {
    return ((125.0f*d) / 65536) - 6;
}

void deinitSensor() {

}