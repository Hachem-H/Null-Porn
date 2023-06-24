#include <NullPorn.h>

int main()
{
    DDoS attack;
    CreateAttack(&attack, "http://127.0.0.1:8080", 5);
    RunAttack(&attack);
    sleep(1);
    StopAttack(&attack);

    printf("Total Requests: %d\n", attack.data->totalRequests);
    printf("Successful Requests: %d\n", attack.data->successfulRequests);
}