#include <api.h>
#include <sensor.h>
#include <gpio.h>
#include <signal.h>

#define GPIO5 5
#define SERVER_PORT 8080

/// @brief instance of the global data for this program
volatile GlobalData gd; //shameless global data

/**
 * @brief Signal handler for ctrl + c
 * 
 */
void signalhandler() {
    gd.shouldStop = true;
    closeUART();
    printf("Stop requested, shutting down in 30 sec...\n"); // makes sure all the workers had time to see shouldStop, mainly tied to AQUISITION_INTERVAL
    sleep(30);
}

/**
 * @brief Entry point
 * 
 * @return int 
 */
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