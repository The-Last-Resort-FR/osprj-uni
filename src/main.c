#include <api.h>
#include <sensor.h>
#include <gpio.h>
#include <signal.h>

#define GPIO5 5
#define SERVER_PORT 8080

volatile GlobalData gd; //shameless global data

void signalhandler() {
    gd.shouldStop = true;
    closeUART();
    sleep(5);
}

int main(void) {
    gd.ledState = 0;
    gd.shouldStop = 0;
    uint32_t pin = GPIO5;
    pthread_t sensorThread;
    pthread_t ledThread;

    signal(SIGINT, signalhandler);

    if(!initUART("/dev/serial0", B115200)) {
        printf("couldn't open serial port");
        return -1;
    }

    pthread_create(&sensorThread, NULL, sensorDataLogger, NULL);
    pthread_create(&ledThread, NULL, setLedPin, &pin);
    srv(SERVER_PORT);
    return 0;
}