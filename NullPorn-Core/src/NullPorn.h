#pragma once

#include <pthread.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct WorkerData_t
{
    uint32_t workerIndex;
    char* url;
} WorkerData;

typedef struct AttackData_t
{
    uint32_t totalRequests;
    uint32_t successfulRequests;

    bool stop;
} AttackData;

typedef struct DDoS_t
{
    char* url;
    AttackData* data;

    pthread_t*  workers;
    WorkerData* workersData;
    uint32_t    workersCount;
} DDoS;

bool HTTPGetRequest(const char* url);

void CreateAttack(DDoS* ddos, char* url, uint32_t workersCount);
void RunAttack(DDoS* ddos);
void StopAttack(DDoS* ddos);