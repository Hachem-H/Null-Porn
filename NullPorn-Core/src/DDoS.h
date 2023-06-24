#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct DDoS_t
{
    char* url;
    bool* stop;
    int workersCount;
} DDoS;