/**
 * @file upnp_pf_errcode.c
 * @brief UNnP interface error codes
 * @details define UNnP interface error codes to string message
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */
#include "upnp_pf_errcode.h"

#ifdef ERROR_CODES

/** Generate switch case for ::error_msg() */
#define ERROR_TEXT(ID, NAME, TEXT) \
    case ID:                       \
        return TEXT;

const char *error_msg(int code)
{
    switch (code)
    {
        ERROR_CODES(ERROR_TEXT)
    }

    return "Unknown error";
}
#undef ERROR_TEXT
#endif //ERROR_CODES
