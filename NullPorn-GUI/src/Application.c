#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include <NullPorn.h>
#include <stdlib.h>

#include "ApplicationState.h"

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);
   
    Application application;
    application.state                    = ApplicationState_Menu;
    application.attacks                  = NULL;      
    application.URLs                     = NULL;
    application.enableWorkersBufferEntry = false;
    application.workersCount             = 100;
    application.workersCountBuffer       = (char*)malloc(0x800);
    application.urlBuffer                = (char*)malloc(0x800);
    application.errorBuffer              = NULL;

    application.panelRect                = (Rectangle) { 25, 50+WINDOW_HEIGHT/12, WINDOW_WIDTH-50, WINDOW_HEIGHT-150 };
    application.panelContentRect         = (Rectangle) { 0, 0, application.panelRect.width-15, 0 };
    application.panelScroll              = (Vector2)   { 99, 0 };

    strcpy(application.workersCountBuffer, "100");
    strcpy(application.urlBuffer, "");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        UpdateApp(&application);
        
        switch (application.state)
        {
        case ApplicationState_Menu:        RenderMenu(&application);         break;
        case ApplicationState_Add:         RenderAdd(&application);          break;
        case ApplicationState_Error:       RenderError(&application);        break;
        case ApplicationState_Attack:      RenderAttack(&application);       break;
        case ApplicationState_AttackFinal: RenderAttackFinal(&application);  break;
        default: break;
        }

        EndDrawing();
    }

    if (application.attacks != NULL)
        free(application.attacks);
    free(application.workersCountBuffer);
    free(application.urlBuffer);
}