#include <api.h>
#include <sensor.h>

GlobalData gd; //shameless global data

int main(void) {
    pthread_t sensorThread;
    pthread_create(&sensorThread, NULL, sensorDataLogger, NULL);
    sleep(15);
    //printf("%s", makeJsonReply());
    srv(8080);
    return 0;
}