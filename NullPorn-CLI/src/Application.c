#include <sys/ioctl.h>
#include <newt.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void Dialog(newtComponent* form, const char* label, char* output)
{
    newtCenteredWindow(22, 3, label);

    char* buffer = "";
    newtComponent entry = newtEntry(1, 1, "", 20, (const char**)&buffer, NEWT_FLAG_SCROLL | NEWT_FLAG_RETURNEXIT);
    newtComponent button = newtCompactButton(8, 2, "Ok");
    newtFormAddComponents(*form, entry, button, NULL);
    newtRunForm(*form);
    strcpy(output, buffer);
    newtFormDestroy(*form);
}

void AddIpCallback(newtComponent component, void* data)
{
    char ipAddress[0x800];
    newtComponent ipForm = newtForm(NULL, NULL, 0);
    Dialog(&ipForm, "Add IP", ipAddress);
}

void AddUrlCallback(newtComponent component, void* data)
{
    char urlAddress[0x800];
    newtComponent urlForm = newtForm(NULL, NULL, 0);
    Dialog(&urlForm, "Add URL", urlAddress);
}

int main() 
{
    struct winsize windowSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize);

    newtInit();
    newtCls();
    
    newtCenteredWindow(windowSize.ws_col-5, windowSize.ws_row-5, "Null Porn");
    newtRefresh();

    newtComponent content   = newtForm(NULL, NULL, 0);

    newtComponent addIPButton  = newtButton(1, 1, "Add IP");
    newtComponent addURLButton = newtButton(15, 1, "Add URL");
    newtComponent attackButton = newtButton(windowSize.ws_col-17, 1, "ATTACK");

    newtComponent contentList = newtListbox(1, 6, windowSize.ws_row-11, NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER);
    newtListboxSetWidth(contentList, windowSize.ws_col-7);
    for (int i = 0; i < 50; i++)
    {
        char ip[15];
        sprintf(ip, "127.0.0.%d", i + 1);
        newtListboxAppendEntry(contentList, ip, (void*)(intptr_t)i);
    }
    
    newtFormAddComponents(content, addIPButton, addURLButton, attackButton, contentList, NULL);
    newtRunForm(content);

    newtFormDestroy(content);
    newtFinished();
}
