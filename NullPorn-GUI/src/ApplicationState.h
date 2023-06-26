#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <NullPorn.h>

#define WINDOW_WIDTH  1720
#define WINDOW_HEIGHT 967

typedef enum ApplicationState_t
{
    ApplicationState_Menu,
    ApplicationState_Error,
    ApplicationState_Add,
    ApplicationState_Attack,
    ApplicationState_AttackFinal,
} ApplicationState;

typedef struct Application_t
{
    ApplicationState state;

    char** URLs;
    uint32_t workersCount;
    bool enableWorkersBufferEntry;
    char* workersCountBuffer;

    char* urlBuffer;
    char* errorBuffer;

    DDoS* attacks;

    Rectangle panelRect;
    Rectangle panelContentRect;
    Vector2 panelScroll;
} Application;

void UpdateApp(Application* application);

void RenderMenu(Application* application);
void RenderError(Application* application);
void RenderAdd(Application* application);
void RenderAttack(Application* application);
void RenderAttackFinal(Application* application);
