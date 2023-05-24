#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>

#define WINDOW_WIDTH  1720
#define WINDOW_HEIGHT 967

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Null-Porn");
    SetTargetFPS(60);

    int num = 60;

    Rectangle panelRect        = { 25, 50+WINDOW_HEIGHT/12, WINDOW_WIDTH-50, WINDOW_HEIGHT-150 };
    Rectangle panelContentRect = { 0, 0, panelRect.width-15, num * 25 + 100 };
    Vector2   panelScroll      = { 99, 0 };

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            Rectangle content = GuiScrollPanel(panelRect, NULL, panelContentRect, &panelScroll);
            BeginScissorMode(content.x, content.y, content.width, content.height);
            for (int i = 0; i < num; i++)
            {
                char ipAddress[16];
                snprintf(ipAddress, sizeof(ipAddress), "127.0.0.%d", i + 1);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 21);
                GuiLabel((Rectangle) { panelRect.x + 15, panelScroll.y + panelRect.y + 15 + i * 35, content.width - 100, 30 }, ipAddress);
                GuiButton((Rectangle) { panelRect.width-75-WINDOW_HEIGHT/12, panelScroll.y+panelRect.y+15+i*35, WINDOW_WIDTH/12, 30}, GuiIconText(ICON_BIN, "Remove"));
            }
            EndScissorMode();
            
            GuiSetStyle(DEFAULT, TEXT_SIZE, 28);
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
            GuiButton((Rectangle) { 25,                              25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD IP");
            GuiButton((Rectangle) { 50+WINDOW_WIDTH/12,              25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ADD URL");
            GuiButton((Rectangle) { WINDOW_WIDTH-25-WINDOW_WIDTH/12, 25, WINDOW_WIDTH/12, WINDOW_HEIGHT/12 }, "ATTACK");
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


