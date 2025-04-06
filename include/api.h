/**
 * @file api.h
 * @author TLR
 * @brief Header of the basic api
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

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
#include <syslog.h>

#define MAX 2048

void* handleRequest(void* args);
void srv(uint16_t port);
char* makeJsonReply();