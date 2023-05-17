#include <ncurses.h>

int main()
{
    char* temporaryIDs[4] = 
    {
        "127.0.0.1",
        "127.0.0.2",
        "127.0.0.3",
        "127.0.0.4",
    };
    
    char* windowLabel = "NULL PORN";
    char* addURLLabel = "[Add URL]";
    char* addIpLabel  = "[Add IP]";
    char* attackLabel = "[ATTACK]";

    initscr();

    WINDOW* topContainer    = newwin(3, COLS, 0, 0);
    WINDOW* middleContainer = newwin(LINES-6, COLS, 3, 0);
    WINDOW* bottomContainer = newwin(3, COLS, LINES-3, 0);
    refresh();

    box(topContainer,    0, 0);
    box(middleContainer, 0, 0);
    box(bottomContainer, 0, 0);

    mvwprintw(topContainer,    0, (COLS-sizeof(windowLabel))/2, windowLabel);
    mvwprintw(middleContainer, 0, 1, "Attacked URLs/IPs");

    for (int i = 0; i < 4; i++)
    {
        mvwprintw(middleContainer, i+1, 2, "(%d) %s", i+1, temporaryIDs[i]);
        mvwprintw(middleContainer, i+1, COLS-5, "[-]");
    }

    mvwprintw(bottomContainer, 1, (COLS-sizeof(attackLabel))/2, attackLabel);
    mvwprintw(topContainer, 1, 2, addURLLabel);
    mvwprintw(topContainer, 1, sizeof(addURLLabel)+4, addIpLabel);

    wrefresh(topContainer);
    wrefresh(middleContainer);
    wrefresh(bottomContainer);
    
    getch();
    endwin();
}
