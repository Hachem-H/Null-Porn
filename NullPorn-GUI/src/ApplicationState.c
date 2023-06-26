#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#include <raygui.h>

#include <pthread.h>
#include <regex.h>
#include <ctype.h>

#include "ApplicationState.h"

static bool ValidURL(const char* url)
{
    regex_t regex;
    int ret;
    const char* pattern = "^(https?|ftp)://[^\\s/$.?#].[^\\s]*$";

    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) 
        return false;

    ret = regexec(&regex, url, 0, NULL, 0);
    regfree(&regex);
    return !ret;
}

static bool ValidNumber(const char *text)
{
    for (size_t i = 0; i < strlen(text); i++)
        if (!isdigit(text[i]))
            return false;
    return true;
}

void UpdateApp(Application* application)
{
    application->panelContentRect.height = arrlen(application->URLs)*36+100;
}

void RenderMenu(Application* application)
{
    Rectangle content = GuiScrollPanel(application->panelRect, NULL, application->panelContentRect, &application->panelScroll);
    BeginScissorMode(content.x, content.y, content.width, content.height);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    for (int i = 0; i < arrlen(application->URLs); i++)
    {
        GuiLabel((Rectangle) { application->panelRect.x + 15, application->panelScroll.y+application->panelRect.y+15 + i*35, content.width - 100, 30 }, application->URLs[i]);
        if(GuiButton((Rectangle) { application->panelRect.width-75-WINDOW_HEIGHT/12, application->panelScroll.y+application->panelRect.y+15 + i*35, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove")))
        {
            arrdel(application->URLs, i);
            break;
        }
    }
    
    EndScissorMode();

    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    if (GuiTextBox((Rectangle) { 100+WINDOW_HEIGHT/12, 25, WINDOW_HEIGHT/12, WINDOW_HEIGHT/12 }, application->workersCountBuffer, 0x800, application->enableWorkersBufferEntry))
        if (ValidNumber(application->workersCountBuffer))
        {
            application->workersCount = atoi(application->workersCountBuffer);
            application->enableWorkersBufferEntry = !application->enableWorkersBufferEntry;
        }

    if (GuiButton((Rectangle) { 25, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD HOST"))
        application->state = ApplicationState_Add;
    if (GuiButton((Rectangle) { WINDOW_WIDTH-25-WINDOW_WIDTH/12, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ATTACK"))
        application->state = ApplicationState_Attack;
}

void RenderAdd(Application* application)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ADD HOST");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiTextBox((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, application->urlBuffer, 0x800, true);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    
    if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
    {
        memset(application->urlBuffer, 0, 0x800);
        application->state = ApplicationState_Menu;
    }
    
    if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
    {
        if (ValidURL(application->urlBuffer))
            arrput(application->URLs, strdup(application->urlBuffer));
        else 
        {
            application->errorBuffer = "Invalid HOST";
            application->state = ApplicationState_Error;
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
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ERROR");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, application->errorBuffer);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);

    if (GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
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
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ATTACK STARTED");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, "Press stop to end the attack.");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    
    if(GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Stop"))
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
    int totalRequests = 0;
        
    for (int i = 0; i < arrlen(application->URLs); i++)
    {
        successfulRequests += application->attacks[i].data->successfulRequests;
        totalRequests += application->attacks[i].data->totalRequests;
    }
        
    snprintf(totalBuffer, sizeof(totalBuffer), "Total Requests: %d", totalRequests);
    snprintf(successfulBuffer, sizeof(successfulBuffer), "Successful Requests: %d", successfulRequests);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, totalBuffer);
    GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, successfulBuffer);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
    if(GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Close"))
        application->state = ApplicationState_Menu;
}