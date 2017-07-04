/**
 * @file upnp_pf_interface.h
 * @brief UNnP interface functions prototypes
 * @details UNnP interface function prototypes use in this project
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __UPNP_PF_INTERFACE_H
#define __UPNP_PF_INTERFACE_H

/** UPnP Supported Protocols  */
typedef enum _SupportedProtocol_t {
    UDP,
    TCP
} SupportedProtocol_t;

/**
 * @brief Init for UPnP Interface
 * @details Discovery Device and setup some useful variables
 * for this API
 */
void upnpPFInterface_init();

/**
 * @brief Add a port forwarding rule
 * @details Add a port forwarding rule on Router using UPnP
 * 
 * @param eport external port
 * @param iport internal port
 * @param protocol protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_addPortMapping(const char *eport, const char *iport, SupportedProtocol_t protocol);

/**
 * @brief Disable port forwarding rule 
 * @details Disable a port forwarding rule on Router using UPnP
 * 
 * @param eport external port
 * @param iport internal port
 * @param protocol protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_diablePortMapping(const char *eport, const char *iport, SupportedProtocol_t protocol);

/**
 * @brief Remove a port forwarding rule
 * @details Remove a port forwarding rule on Router using UPnP
 * 
 * @param eport external port
 * @param protocol protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_removePortMapping(const char *eport, SupportedProtocol_t protcol);

#endif //__UPNP_PF_INTERFACE_H
