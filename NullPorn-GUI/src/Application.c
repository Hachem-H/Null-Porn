#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include <NullPorn.h>
#include <stdlib.h>

#include "ApplicationState.h"
#include "Layout.h"

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);
   
    Application application;
    InitializeApp(&application);

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

    DeleteApp(&application);
}