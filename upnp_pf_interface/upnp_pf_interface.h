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
