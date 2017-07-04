/**
 * @file util.h
 * @brief Utility functions prototypes
 * @details Utility function prototypes use in this project
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __MY_UTIL_H_
#define __MY_UTIL_H_

/**
 * @brief Merge strings based on its format
 * @details Concatenate strings based on format
 * 
 * @param[out] strout string pointer to save output
 * @warning You need to free() @p strout
 * @param[in] fmt format string
 */
void strfmt(char **strout, const char *fmt, ...);

#endif //__MY_UTIL_H_
