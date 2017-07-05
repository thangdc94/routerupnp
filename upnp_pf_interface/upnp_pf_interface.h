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

/** Using X Macro technique for enum to string */
#define SUPPORTED_PROTOCOLS(X) \
    X(UDP, "UDP")              \
    X(TCP, "TCP")

/** Generate enum */
#define PROTOCOL_ENUM(NAME, TEXT) NAME,

/** UPnP Supported Protocols  */
typedef enum _SupportedProtocol_t {
    SUPPORTED_PROTOCOLS(PROTOCOL_ENUM)
} SupportedProtocol_t;

/** Get String of typedef SupportedProtocol_t */
extern const char *proto_str[];

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
 * @param[in] eport external port
 * @param[in] iport internal port
 * @param[in] protocol protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_addPortMapping(const char *eport, const char *iport, SupportedProtocol_t protocol);

/**
 * @brief Disable port forwarding rule 
 * @details Disable a port forwarding rule on Router using UPnP
 * 
 * @param[in] eport external port
 * @param[in] iport internal port
 * @param[in] proto protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_diablePortMapping(const char *eport, const char *iport, SupportedProtocol_t proto);

/**
 * @brief Remove a port forwarding rule
 * @details Remove a port forwarding rule on Router using UPnP
 * 
 * @param[in] eport external port
 * @param[in] proto protocol will be mapped
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_removePortMapping(const char *eport, SupportedProtocol_t proto);

#endif //__UPNP_PF_INTERFACE_H
