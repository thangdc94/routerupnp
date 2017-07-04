/**
 * @file netutil.h
 * @brief Network Utility functions prototypes
 * @details Network Utility function prototypes use in this project
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __NET_UTIL_H_
#define __NET_UTIL_H_

/**
 * @brief Get MAC Address associated with Interface name
 * @details Get MAC Address associated with Interface name
 * 
 * @param[in] iface interface name
 * @return MAC Address
 * @warning You need to free() MAC Address after use it
 */
char *getmac_from_ifname(char *iface);

/**
 * @brief Get MAC Address associated with IP Address
 * @details Get MAC Address associated with IP Address
 * 
 * @param[in] ip IP Address
 * @return MAC Address
 * @warning You need to free() MAC Address after use it
 */
char *getmac_from_ip(const char *ip);

#endif //__NET_UTIL_H_
