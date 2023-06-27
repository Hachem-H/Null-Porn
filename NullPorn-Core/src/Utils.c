#include "NullPorn.h"

#include <string.h>
#include <regex.h>
#include <ctype.h>

bool ValidURL(const char* url)
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

bool ValidNumber(const char* text)
{
    for (size_t i = 0; i < strlen(text); i++)
        if (!isdigit(text[i]))
            return false;
    return true;
}