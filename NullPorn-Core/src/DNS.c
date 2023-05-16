#include "DDoS.h"

char* DNSLookup(const char* domain)
{
    int addressInfo;
    int socketFD;
    char ip[32];

    struct addrinfo hints;
    struct addrinfo* results;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((addressInfo = getaddrinfo(domain, "domain", &hints, &results)) != 0)
        fprintf(stderr, "[ERR] %s\n", gai_strerror(addressInfo));

    struct sockaddr_in* socketAddressInput = (struct sockaddr_in*) results->ai_addr;
    strcpy(ip, inet_ntoa(socketAddressInput->sin_addr));
    printf("[IP] %s : %s\n", domain, ip);	

    freeaddrinfo(results); 

    char* buffer = malloc(strlen(ip));
    strcpy(buffer, ip);
    return buffer;
}
