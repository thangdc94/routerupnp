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

/** Using X Macro pattern for enum to string */
#define SUPPORTED_PROTOCOLS(X) \
    X(UDP, "UDP")              \
    X(TCP, "TCP")

/** Generate enum */
#define PROTOCOL_ENUM(NAME, TEXT) NAME,

/** Generate switch case for enum to string */
#define PROTOCOL_TEXT(NAME, TEXT) \
    case NAME:                    \
        return TEXT;

/** UPnP Supported Protocols  */
typedef enum _SupportedProtocol_t {
    SUPPORTED_PROTOCOLS(PROTOCOL_ENUM)
} SupportedProtocol_t;

/**
 * @brief Get String of typedef SupportedProtocol_t
 * @details Get String of typedef SupportedProtocol_t
 * 
 * @param[in] proto protocol name
 * @return protocol string
 */
static inline const char *get_proto_str(SupportedProtocol_t proto)
{
    switch (proto)
    {
        SUPPORTED_PROTOCOLS(PROTOCOL_TEXT)
    }
    return "Unknown";
}

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
