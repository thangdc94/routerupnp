/**
 * @file util.c
 * @brief Utility functions
 * @details Utility functions use in this project
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <string.h>

#include "util.h"

void strfmt(char **strout, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    *strout = malloc(n + 1);
    va_start(args, fmt);
    vsprintf(*strout, fmt, args);
    va_end(args);
}