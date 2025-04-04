#include <api.h>


int state_on = 0; // Global state

void* handle_request(void* args) {
    int* new_socket = (int*)args;
    char buffer[MAX] = {0};
    read(*new_socket, buffer, MAX);
    
    printf("Request:\n%s\n", buffer);
    
    // Determine the route
    const char *response;
    if (strncmp(buffer, "GET / ", 6) == 0) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nWelcome to the C API server!";
    } else if (strncmp(buffer, "GET /on", 7) == 0) {
        state_on = 1;
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurned ON";
    } else if (strncmp(buffer, "GET /off", 8) == 0) {
        state_on = 0;
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTurned OFF";
    } else if (strncmp(buffer, "GET /data", 9) == 0) {
        char data_response[100];
        sprintf(data_response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\": \"%s\", \"value\": 42}", state_on ? "on" : "off");
        response = data_response;
    } else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
    }

    write(*new_socket, response, strlen(response));

    close(*new_socket);
    free(new_socket);
    pthread_exit(0);
}

void func(int connfd) { 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(connfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(connfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
    close(connfd);
} 

void srv(uint16_t port) {
    int sockfd, len;
    struct sockaddr_in servaddr, cli; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 

    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 

    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 

    len = sizeof(cli); 

    pthread_t requestHandlerThread;

    while (1) {
        int* connfd = (int*)malloc(sizeof(int));
        *connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t * restrict)&len); 
        if (*connfd < 0) { 
            printf("server accept failed...\n"); 
            exit(0); 
        } 
        pthread_create(&requestHandlerThread, NULL, handle_request, connfd);
    }

    close(sockfd);
}