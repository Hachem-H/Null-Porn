#include "DDoS.h"

static bool stop = false;

static void* WorkerThread(void* data)
{
    AttackData* attackData = (AttackData*) data;

    while (!stop);
        printf("[%d]: Attacking.\n", attackData->workerIndex);
    printf("[%d]: Done.\n", attackData->workerIndex);
}

void CreateAttack(DDoS* ddos, char* url, uint32_t workersCount)
{
    ddos->url          = url;
    ddos->workersCount = workersCount;

    ddos->workers = (pthread_t*) malloc(sizeof(pthread_t)*workersCount);
    ddos->workersData = (AttackData*) malloc(sizeof(AttackData)*workersCount);
}

void RunAttack(DDoS* ddos)
{
    for (int i = 0; i < ddos->workersCount; i++)
    {
        ddos->workersData[i].url = ddos->url;
        ddos->workersData[i].workerIndex = i;

        pthread_create(&ddos->workers[i], NULL, WorkerThread, (void*) &ddos->workersData[i]);
    }
}

void StopAttack(DDoS* ddos)
{
    stop = true;

    for (int i = 0; i < ddos->workersCount; i++)
        pthread_join(ddos->workers[i], NULL);

    free(ddos->workers);
    free(ddos->workersData);
}