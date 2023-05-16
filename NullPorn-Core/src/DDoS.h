#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define HTTP_PORT 80

typedef struct PseudoHeader_t
{
    uint32_t sourceAddress;
    uint32_t destinationAddress;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t tcpLength;

    struct tcphdr tcpHeader;
} PseudoHeader;

int RandomPort();
int RandomForIP();

char* RandomIP();
int ValidIP(char* ip);

void InterruptHandler(int signal);
uint16_t Checksum(uint16_t* pointer, int numBytes);

void  Flood(void* ip);
char* DNSLookup(const char* domain);
