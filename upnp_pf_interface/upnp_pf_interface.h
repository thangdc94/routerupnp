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
typedef enum {
    SUPPORTED_PROTOCOLS(PROTOCOL_ENUM)
} SupportedProtocol_t;
#undef PROTOCOL_ENUM

/** Mapping input for AddPortMapping */
typedef struct
{
    char eport[6];             /**< external port */
    char iport[6];             /**< internal port */
    SupportedProtocol_t proto; /**< protocol will be mapped */
} MappingRule_t;

/**
 * @brief Get Protocol string from SupportedProtocol_t
 * @details Get Protocol string from SupportedProtocol_t. This function will use
 * define #SUPPORTED_PROTOCOLS(X)
 *
 * @param[in] proto protocol you want to get string
 * @return string of protocol
 */
extern const char *get_proto_str(SupportedProtocol_t proto);

/**
 * @brief Init for UPnP Interface
 * @details Discovery Device and setup some useful variables
 * for this API
 * 
 * @return 0 if OK and -1 if error
 */
int upnpPFInterface_init();

/**
 * @brief Add port forwarding rules
 * @details Add port forwarding rules on Router using UPnP
 * 
 * @param[in] rules array of Rule to add
 * @param[in] num_of_rules size of rule array
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_addPortMapping(MappingRule_t rules[], int num_of_rules);

/**
 * @brief Disable port forwarding rules
 * @details Disable port forwarding rules on Router using UPnP
 * 
 * @param[in] rules array of Rule to disable
 * @param[in] num_of_rules size of rule array
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_diablePortMapping(MappingRule_t rules[], int num_of_rules);

/**
 * @brief Update port forwarding rules
 * @details Remove old port mapping rule forwarding rules on Router using UPnP.
 * Check port conflict and add port mapping rule
 * 
 * @param[in] rules array of Rule to add
 * @param[in] num_of_rules size of rule array
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_updatePortMapping(MappingRule_t rules[], int num_of_rules);

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
