#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

typedef enum __LOG_LEVEL_t
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
} __LOG_LEVEL;

static const char* __LogColors[] = { "\x1b[94m", "\x1b[36m",
                                     "\x1b[32m", "\x1b[33m", 
                                     "\x1b[31m", "\x1b[35m" };

static void __Log(const char* level, const char* file, const uint32_t line, const char* time)
{
#ifdef LOG_NOCOLORS
	printf("(%s) %s [%s]\x1b[0m \x1b[90m%s@%d:\x1b[0m ", time, level, client, level, file, line);
#else
    printf("(%s) [%s] %s@%d: ", time, level, file, line);
#endif
}

#define ERROR(...) { __Log("ERROR", __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }
#define FATAL(...) { __Log("FATAL", __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }

#ifndef NP_DIST
	#define TRACE(...) { __Log("TRACE", __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }
	#define DEBUG(...) { __Log("DEBUG", __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }
	#define INFO(...)  { __Log("INFO",  __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }
	#define WARN(...)  { __Log("WARN",  __FILENAME__, __LINE__, __TIME__); printf(__VA_ARGS__); putchar('\n'); }
#else
	#define TRACE(...)
	#define DEBUG(...)
	#define INFO(...)
	#define WARN(...)
	#define ERROR(...)
	#define FATAL(...)
#endif
