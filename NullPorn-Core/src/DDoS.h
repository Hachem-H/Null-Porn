#pragma once

#include <pthread.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct AttackData_t
{
    int workerIndex;
    char* url;
} AttackData;

typedef struct DDoS_t
{
    char* url;
    
    pthread_t*  workers;
    AttackData* workersData;
    uint32_t    workersCount;
} DDoS;

void CreateAttack(DDoS* ddos, char* url, uint32_t workersCount);
void RunAttack(DDoS* ddos);
void StopAttack(DDoS* ddos);