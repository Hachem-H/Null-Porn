#define RAYGUI_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include <stb_ds.h>
#include <DDoS.h>

#include <pthread.h>
#include <regex.h>

#define WINDOW_WIDTH  1720
#define WINDOW_HEIGHT 967

static char** GlobalIPs  = NULL;
static char** GlobalURLs = NULL;

void* FloodThread(void* arg)
{
    char* ipAddress = (char*)arg;
    Flood(ipAddress);
    return NULL;
}

void* DNSLookupThread(void* arg)
{
    char* url = (char*)arg;
    char* ipAddress = DNSLookup(url);
    if (ipAddress != NULL) 
        Flood(ipAddress);
    return NULL;
}

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

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);
   
    Rectangle panelRect        = { 25, 50+WINDOW_HEIGHT/12, WINDOW_WIDTH-50, WINDOW_HEIGHT-150 };
    Rectangle panelContentRect = { 0, 0, panelRect.width-15, 0 };
    Vector2   panelScroll      = { 99, 0 };

    bool showError = false;
    bool showAddIP  = false;
    bool showAddURL = false;

    char urlBuffer[0x800] = {0};
    char ipBuffer[0x800] = {0};
    char* errorType;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int numEntries = arrlen(GlobalIPs)+arrlen(GlobalURLs);
        panelContentRect.height = numEntries*1.2*30+100;

        if (!showAddIP && !showAddURL && !showError)
        {
            Rectangle content = GuiScrollPanel(panelRect, NULL, panelContentRect, &panelScroll);
            BeginScissorMode(content.x, content.y, content.width, content.height);
            int baseY = panelScroll.y+panelRect.y+15;
            GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
            for (int i = 0; i < arrlen(GlobalIPs); i++)
            {
                int lineY = baseY + i*35;
                GuiLabel((Rectangle) { panelRect.x + 15, lineY, content.width - 100, 30 }, GlobalIPs[i]);
                if(GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, lineY, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove")))
                {
                    arrdel(GlobalIPs, i);
                    goto DoneButtons;
                }
            }
            for (int i = 0; i < arrlen(GlobalURLs); i++)
            {
                int lineY = baseY + arrlen(GlobalIPs)*35 + i*35;
                GuiLabel((Rectangle) { panelRect.x + 15, lineY, content.width - 100, 30 }, GlobalURLs[i]);
                if(GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, lineY, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove")))
                {
                    arrdel(GlobalURLs, i);
                    goto DoneButtons;
                }
            }

            DoneButtons: EndScissorMode();
            
            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

            if (GuiButton((Rectangle) { 25, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD IP"))
                showAddIP = true;
            if (GuiButton((Rectangle) { 50+WINDOW_WIDTH/12, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD URL"))
                showAddURL = true;

            GuiButton((Rectangle) { WINDOW_WIDTH-25-WINDOW_WIDTH/12, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ATTACK");
        }

        if (showAddIP)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ADD IP");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
            GuiTextBox((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, ipBuffer, 0x800, true);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
            {
                memset(ipBuffer, 0, 0x800);
                showAddIP = false;
            }

            if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
            {
                if (ValidIP(ipBuffer))
                    memset(ipBuffer, 0, 0x800);
                else 
                {
                    errorType = "Invalid IP";
                    showError = true;
                }

                memset(ipBuffer, 0, 0x800);
                showAddIP = false;
            }
        }

        if (showAddURL)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiLabel((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2-100, WINDOW_WIDTH/2, 80 }, "ADD URL");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 48);
            GuiTextBox((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+100, WINDOW_WIDTH/2, 50 }, urlBuffer, 0x800, true);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
            {
                memset(urlBuffer, 0, 0x800);
                showAddURL = false;
            }

            if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
            {
                if (ValidURL(urlBuffer))
                    arrput(GlobalURLs, strdup(urlBuffer));
                else
                {
                    errorType = "Invalid URL";
                    showError = true;
                }

                memset(urlBuffer, 0, 0x800);
                showAddURL = false;
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
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


