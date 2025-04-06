#pragma once

#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>
#include <sensor.h>

#define MAX 1024

void* handleRequest(void* args);
void srv(uint16_t port);
char* makeJsonReply();