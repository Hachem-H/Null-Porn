#include <ncurses.h>

int main()
{
    char* windowLabel = "NULL PORN";
    char* addURLLabel = "[Add URL]";
    char* addIpLabel  = "[Add IP]";
    char* attackLabel = "[ATTACK]";

    initscr();

    WINDOW* topMenu = newwin(3, COLS, 0, 0);
    WINDOW* content = newwin(LINES, COLS, 3, 0);
    refresh();

    box(topMenu, 0, 0);
    box(content, 0, 0);

    mvwprintw(topMenu,    0, (COLS-sizeof(windowLabel))/2, windowLabel);
    mvwprintw(content, 0, 1, "Attacked URLs/IPs");

    for (int i = 0; i < 50; i++)
        mvwprintw(content, i+1, 2, "[-] 127.0.0.%d", i+1);

    mvwprintw(topMenu, 1, 2,                          addURLLabel);
    mvwprintw(topMenu, 1, sizeof(addURLLabel)+4,      addIpLabel);
    mvwprintw(topMenu, 1, COLS-sizeof(attackLabel)-2, attackLabel);

    wrefresh(topMenu);
    wrefresh(content);
    
    getch();
    endwin();
}
