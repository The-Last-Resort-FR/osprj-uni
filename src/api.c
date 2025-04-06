/**
 * @file api.c
 * @author TLR
 * @brief Very basic api implementation
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <api.h>

/// @brief global data from main.c
extern GlobalData gd;

/**
 * @brief Request handler meant to be used as a thread
 * 
 * @param args Socket handle
 * @return void* 
 */
void* handleRequest(void* args) {
    int* new_socket = (int*)args;
    char buffer[MAX] = {0};
    read(*new_socket, buffer, MAX);
    
    printf("Request:\n%s\n", buffer);
    
    // Determine the route
    char *response;
    if (strncmp(buffer, "GET / ", 6) == 0) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWelcome to the C API server!";
    } else if (strncmp(buffer, "GET /on", 7) == 0) {
        gd.ledState = 1;
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurned ON";
    } else if (strncmp(buffer, "GET /off", 8) == 0) {
        gd.ledState = 0;
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurned OFF";
    } else if (strncmp(buffer, "GET /data", 9) == 0) {
        char* dataResponse = makeJsonReply();
        response = dataResponse;
    } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
    }
    write(*new_socket, response, strlen(response));
    free(response);

    close(*new_socket);
    free(new_socket);
    pthread_exit(0);
    return NULL;
}

/**
 * @brief Setup the socket and send each accepts to a handleRequest thread
 * 
 * @param port Port on which the socket will be bound to
 */
void srv(uint16_t port) {
    int sockfd, len;
    struct sockaddr_in servaddr, cli; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        syslog(LOG_ERR, "socket creation failed...\n"); 
        exit(0); 
    } 

    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        syslog(LOG_ERR, "socket bind failed...\n"); 
        exit(0); 
    } 

    if ((listen(sockfd, 5)) != 0) { 
        syslog(LOG_ERR, "Listen failed...\n"); 
        exit(0); 
    } 

    len = sizeof(cli); 

    pthread_t requestHandlerThread;

    while (!gd.shouldStop) {
        int* connfd = (int*)malloc(sizeof(int));
        *connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t * restrict)&len); 
        if (*connfd < 0) { 
            syslog(LOG_ERR, "server accept failed...\n"); 
            exit(0); 
        } 
        pthread_create(&requestHandlerThread, NULL, handleRequest, connfd);
    }

    close(sockfd);
}


/**
 * @brief Makes a json formated string from globalData gd
 * 
 * @return char* has to be freed by the caller
 */
char* makeJsonReply() {
    // printf("starting concate, allocating : %ld bytes\n", sizeof(char)*gd.data.size*100);
    char* reply = (char*)malloc(sizeof(char)*gd.data.size*100);
    char tmp[100];
    sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n[");
    // printf("first line\n");
    for (size_t i = 0; i < gd.data.size; i++)
    {
        if(i == 0) sprintf(tmp, "{\"time\": %lu, \"Temperature\": %.1f, \"humidity\": %.1f},", gd.data.dataArray[i]->timestamp, gd.data.dataArray[i]->temperature, gd.data.dataArray[i]->humidity);
        else if(i == gd.data.size-1) sprintf(tmp, "\n\r{\"time\": %lu, \"Temperature\": %.1f, \"humidity\": %.1f}", gd.data.dataArray[i]->timestamp, gd.data.dataArray[i]->temperature, gd.data.dataArray[i]->humidity);
        else sprintf(tmp, "\n\r{\"time\": %lu, \"Temperature\": %.1f, \"humidity\": %.1f},", gd.data.dataArray[i]->timestamp, gd.data.dataArray[i]->temperature, gd.data.dataArray[i]->humidity);
        strcat(reply, tmp);
    }
    strcat(reply, "]\n\r");
    return reply;
}