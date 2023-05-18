#include <ncurses.h>

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{
    char* windowLabel = "NULL PORN";
    char* addURLLabel = "[Add URL]";
    char* addIpLabel = "[Add IP]";
    char* attackLabel = "[ATTACK]";

    initscr();
    cbreak();
    keypad(stdscr, true);
    noecho();

    WINDOW* menu = newwin(3, COLS, 0, 0);
    WINDOW* content = newwin(LINES - 3, COLS, 3, 0);
    refresh();

    scrollok(menu, false);
    scrollok(content, true);

    mvwprintw(menu, 0, (COLS - strlen(windowLabel)) / 2, windowLabel);
    mvwprintw(content, 0, 1, "Attacked URLs/IPs");

    int numIds = 150;
    char* temporaryIds[numIds];
    for (int i = 0; i < numIds; i++)
    {
        temporaryIds[i] = malloc(12 * sizeof(char));
        sprintf(temporaryIds[i], "127.0.0.%d", i + 1);
    }

    int contentOffset = 0;
    int visibleLines = LINES - 4;
    int numVisibleIds = (numIds < visibleLines) ? numIds : visibleLines;

    for (int i = 0; i < numVisibleIds; i++)
        mvwprintw(content, i + 1, 2, "[-] %s", temporaryIds[i]);

    mvwprintw(menu, 1, 2, addURLLabel);
    mvwprintw(menu, 1, strlen(addURLLabel) + 4, addIpLabel);
    mvwprintw(menu, 1, COLS - strlen(attackLabel) - 2, attackLabel);

    int cursorState = 0;
    int cursorX = 1;
    int cursorY = 1;
    int key;

    box(menu, 0, 0);
    box(content, 0, 0);

    wmove(menu, cursorY, cursorX);
    wrefresh(content);
    wrefresh(menu);

    while ((key = getch()) != KEY_F(1))
    {

        switch (key)
        {
            case KEY_LEFT:
                if (cursorX > 1)
                    cursorX--;
                break;
            case KEY_RIGHT:
                if (cursorX < COLS - 2)
                    cursorX++;
                break;

            case KEY_UP:
            {
                if (cursorY > 1)
                    cursorY--;
                else if (cursorState % 2 != 0)
                {
                    if (contentOffset > 0)
                    {
                        contentOffset--;
                        werase(content);
                        box(content, 0, 0);
                        for (int i = 0; i < numVisibleIds; i++)
                            mvwprintw(content, i + 1, 2, "[-] %s", temporaryIds[i + contentOffset]);
                        wrefresh(content);
                    }
                }
            }
            break;

            case KEY_DOWN:
            {
                if (cursorState % 2 == 0)
                {
                    if (cursorY < 1)
                        cursorY++;
                }
                else
                {
                    if (cursorY < numVisibleIds-1)
                        cursorY++;
                    else
                    {
                        if (contentOffset + visibleLines - 1 < numIds)
                        {
                            contentOffset++;
                            werase(content);
                            box(content, 0, 0);
                            for (int i = 0; i < numVisibleIds-1; i++)
                                mvwprintw(content, i + 1, 2, "[-] %s", temporaryIds[i + contentOffset]);
                            wrefresh(content);
                        }
                    }
                }
            }
            break;

            case '\t':
            {
                cursorState++;
                cursorX = 1;
                cursorY = 1;
            }
            break;

            default:
                break;
        }

        box(menu, 0, 0);
        box(content, 0, 0);

        if (cursorState % 2 == 0)
        {
            wmove(menu, cursorY, cursorX);
            wrefresh(menu);
        }
        else
        {
            wmove(content, cursorY, cursorX);
            wrefresh(content);
        }
    }

    for (int i = 0; i < numIds; i++)
        free(temporaryIds[i]); 
    endwin();
}
