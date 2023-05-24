#define RAYGUI_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#include <stb_ds.h>

#define WINDOW_WIDTH  1720
#define WINDOW_HEIGHT 967

static char** GlobalIPs  = NULL;
static char** GlobalURLs = NULL;

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);
   
    int numEntries = 0;
    int numURLs = 0;
    int numIPs = 0;

    Rectangle panelRect        = { 25, 50+WINDOW_HEIGHT/12, WINDOW_WIDTH-50, WINDOW_HEIGHT-150 };
    Rectangle panelContentRect = { 0, 0, panelRect.width-15, numEntries * 1.2 * 30 + 100 };
    Vector2   panelScroll      = { 99, 0 };

    bool showAddIP  = false;
    bool showAddURL = false;

    char urlBuffer[0x800] = {0};
    char ipBuffer[0x800] = {0};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!showAddIP && !showAddURL)
        {
            Rectangle content = GuiScrollPanel(panelRect, NULL, panelContentRect, &panelScroll);
            BeginScissorMode(content.x, content.y, content.width, content.height);
            for (int i = 0; i < numIPs; i++)
            {
                GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
                GuiLabel((Rectangle) { panelRect.x + 15, panelScroll.y + panelRect.y+15+i*35, content.width - 100, 30 }, GlobalIPs[i]);
                GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, panelScroll.y+panelRect.y+15+i*35, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove"));
            }
            for (int i = 0; i < numURLs; i++)
            {
                GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
                GuiLabel((Rectangle) { panelRect.x + 15, panelScroll.y + panelRect.y+15+i*35, content.width - 100, 30 }, GlobalURLs[i]);
                GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, panelScroll.y+panelRect.y+15+i*35, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove"));
            }
            EndScissorMode();
            
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
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
            {
                memset(ipBuffer, 0, 0x800);
                showAddIP = false;
            }

            if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
            {
                numIPs++;
                arrput(GlobalIPs, strdup(ipBuffer));
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
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            if (GuiButton((Rectangle) { WINDOW_WIDTH/4, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Cancel"))
            {
                memset(urlBuffer, 0, 0x800);
                showAddURL = false;
            }

            if (GuiButton((Rectangle) { WINDOW_WIDTH/2+50, WINDOW_HEIGHT/2+200, WINDOW_WIDTH/4-50, 40 }, "Okay"))
            {
                numURLs++;
                arrput(GlobalURLs, strdup(urlBuffer));
                memset(urlBuffer, 0, 0x800);
                showAddURL = false;
            }
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


