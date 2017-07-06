/**
 * @file upnp_pf_errcode.h
 * @brief UNnP interface error codes
 * @details define UNnP interface error codes
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __UPNP_PF_ERRCODE_H_
#define __UPNP_PF_ERRCODE_H_

/** Using X Macro technique for enum to string */
#define ERROR_CODES(X)                                 \
    X(0, SUCCESS, "No error at all!")                  \
    X(10, ERR_RETRY, "Max retry on error is reached")  \
    X(20, ERR_ROLLBACK, "Rollback after error failed") \
    X(40, ERR_TIMEOUT, "Connection timed out")         \
    X(50, ERR_NO_INIT, "Initial parameter is not set") \
    X(60, ERR_PORT_CONFLICT, "Port has been already in used")

/** Generate enum */
#define ERROR_ENUM(ID, NAME, TEXT) NAME = ID,
enum
{
    ERROR_CODES(ERROR_ENUM)
};
#undef ERROR_ENUM

/**
 * @brief Get error message
 * @details Get error message. This function will use define #ERROR_CODES(X)
 *
 * @param[in] code the error code
 * @return string of error code
 */
extern const char *error_msg(int code);

#endif //__UPNP_PF_ERRCODE_H_
