#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#include <raygui.h>

#include <pthread.h>
#include <stdlib.h>

#include "ApplicationState.h"
#include "Layout.h"

void InitializeApp(Application* application)
{
    application->state                    = ApplicationState_Menu;
    application->attacks                  = NULL;      
    application->URLs                     = NULL;
    application->enableWorkersBufferEntry = false;
    application->workersCount             = 100;
    application->workersCountBuffer       = (char*)malloc(0x800);
    application->urlBuffer                = (char*)malloc(0x800);
    application->errorBuffer              = NULL;

    application->panelRect                = (Rectangle) { __PANEL_RECT    };
    application->panelContentRect         = (Rectangle) { __PANEL_CONTENT };
    application->panelScroll              = (Vector2)   { __PANEL_SCROLL  };

    strcpy(application->workersCountBuffer, "100");
    strcpy(application->urlBuffer, "");
}

void UpdateApp(Application* application)
{
    application->panelContentRect.height = arrlen(application->URLs)*36+100;
}

void DeleteApp(Application* application)
{
    if (application->attacks != NULL)
        free(application->attacks);
    free(application->workersCountBuffer);
    free(application->urlBuffer);
}

void RenderMenu(Application* application)
{
    Rectangle content = GuiScrollPanel(application->panelRect, NULL, application->panelContentRect, &application->panelScroll);
    BeginScissorMode(content.x, content.y, content.width, content.height);

    GuiSetStyle(DEFAULT, TEXT_SIZE,      21);
    GuiSetStyle(LABEL,   TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    for (int i = 0; i < arrlen(application->URLs); i++)
    {
        GuiLabel(    (Rectangle) { __MENU_URLS_RECT   }, application->URLs[i]);
        if(GuiButton((Rectangle) { __MENU_REMOVE_URLS }, GuiIconText(ICON_BIN, "Remove")))
        {
            arrdel(application->URLs, i);
            break;
        }
    }
    
    EndScissorMode();

    GuiSetStyle(DEFAULT, TEXT_SIZE,      28);
    GuiSetStyle(BUTTON,  TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    if (GuiTextBox((Rectangle) { __MENU_WORKERS_RECT }, application->workersCountBuffer, 0x800, application->enableWorkersBufferEntry))
        if (ValidNumber(application->workersCountBuffer))
        {
            application->workersCount             = atoi(application->workersCountBuffer);
            application->enableWorkersBufferEntry = !application->enableWorkersBufferEntry;
        }

    if (GuiButton((Rectangle) { __MENU_ADD_RECT    }, "ADD HOST"))
        application->state = ApplicationState_Add;
    if (GuiButton((Rectangle) { __MENU_ATTACK_RECT }, "ATTACK"))
        application->state = ApplicationState_Attack;
}

void RenderAdd(Application* application)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE,      80);
    GuiSetStyle(LABEL,   TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { __ADD_ADD }, "ADD HOST");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox((Rectangle) { __ADD_ENTRY }, application->urlBuffer, 0x800, true);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    
    if (GuiButton((Rectangle) { __ADD_CANCEL }, "Cancel"))
    {
        memset(application->urlBuffer, 0, 0x800);
        application->state = ApplicationState_Menu;
    }
    
    if (GuiButton((Rectangle) { __ADD_OKAY }, "Okay"))
    {
        if (ValidURL(application->urlBuffer))
            arrput(application->URLs, strdup(application->urlBuffer));
        else 
        {
            application->errorBuffer = "Invalid HOST";
            application->state       = ApplicationState_Error;
            memset(application->urlBuffer, 0, 0x800);
            return;
        }

        memset(application->urlBuffer, 0, 0x800);
        application->state = ApplicationState_Menu;
    }
}

void RenderError(Application* application)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { __ERROR_LABEL }, "ERROR");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel((Rectangle) { __ERROR_ENTRY }, application->errorBuffer);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);

    if (GuiButton((Rectangle) { __ERROR_BUTTON }, "Okay"))
        application->state = ApplicationState_Menu;
}

void RenderAttack(Application* application)
{
    application->attacks = (DDoS*) malloc(sizeof(DDoS)*arrlen(application->URLs));

    for (int i = 0; i < arrlen(application->URLs); i++)
    {
        CreateAttack(&application->attacks[i], application->URLs[i], application->workersCount);
        RunAttack(&application->attacks[i]);
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { __ATTACK_LABEL1 }, "ATTACK STARTED");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel((Rectangle) { __ATTACK_LABEL2 }, "Press stop to end the attack.");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    
    if(GuiButton((Rectangle) { __ATTACK_STOP }, "Stop"))
    {
        for (int i = 0; i < arrlen(application->URLs); i++)
            StopAttack(&application->attacks[i]);
        application->state = ApplicationState_AttackFinal;
    }
}

void RenderAttackFinal(Application* application)
{
    char totalBuffer[256];
    char successfulBuffer[256];

    int successfulRequests = 0;
    int totalRequests      = 0;
        
    for (int i = 0; i < arrlen(application->URLs); i++)
    {
        successfulRequests += application->attacks[i].data->successfulRequests;
        totalRequests      += application->attacks[i].data->totalRequests;
    }
        
    snprintf(totalBuffer,      sizeof(totalBuffer),      "Total Requests: %d",      totalRequests);
    snprintf(successfulBuffer, sizeof(successfulBuffer), "Successful Requests: %d", successfulRequests);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { __FINAL_TOTAL      }, totalBuffer);
    GuiLabel((Rectangle) { __FINAL_SUCCESSFUL }, successfulBuffer);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    if(GuiButton((Rectangle) { __FINAL_BUTTON }, "Close"))
        application->state = ApplicationState_Menu;
}