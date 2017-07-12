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

#include "mappingrule.h"

/** 
 * Expired Duration for Port Mapping in seconds. 
 * Port Mapping Rule will be remove after amounts of this time.
 * It's will be helpful if device removed from network and never comeback,
 * rule will be removed automatically after an amount of time.
 * Value 0 means never expire.
 */
#define LEASE_DURATION 86400 // 1 day

/**
 * @brief Init for UPnP Interface
 * @details Discovery Device and setup some useful variables
 * for this API. 
 * @warning Need to call ::upnpPFInterface_destroy()
 * 
 * @return 0 if OK and -1 if error
 */
int upnpPFInterface_init();

/**
 * @brief Destroy UPnP Interface
 * @details free memory of ::upnpPFInterface_init() to prevent memory leak
 * 
 * @return 0 if OK and -1 if error
 */
int upnpPFInterface_destroy();

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
 * @details Disable port forwarding rules on Router using UPnP by removing them
 * 
 * @return 0 if OK or error code if failed
 */
int upnpPFInterface_diablePortMapping();

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
