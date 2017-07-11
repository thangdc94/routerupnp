/**
 * @file portmappingcfg.h
 * @brief Port Mapping config API
 * @details Function prototype to get and save port mapping rules to config file
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __PORT_MAPPING_CFG_H_
#define __PORT_MAPPING_CFG_H_

#include "mappingrule.h"

/** Default External port value for Config */
#define PM_DEFAUT_EPORT "0"

/** Default Internal port value for Config */
#define PM_DEFAUT_IPORT "0"

/** Port mapping Config */
typedef struct _PortMappingCfg_t
{
    int is_enable;       /**< enable port mapping config for router over UPnP or not */
    int numofrules;      /**< number of rules */
    MappingRule_t *rules; /**< Rules you want to add to router */
} PortMappingCfg_t;

/**
 * @brief Get Port mapping config
 * @details Get Port mapping config from storage and create a new one with default
 * value if it doesn't exist.
 * @warning you need to free() ::PortMappingCfg_t::rules if not NULL after use it
 *
 * @return port mapping config
 */
PortMappingCfg_t PMCFG_getConfig();

/**
 * @brief Save Port mapping config
 * @details Save Port mapping config to storage and create a new one with default
 * value if it doesn't exist.
 *
 * @param[in] pm_cfg Port mapping config you want to save to storage
 * @return 0 if OK and < 0 if failed
 */
int PMCFG_saveConfig(PortMappingCfg_t *pm_cfg);

#endif //__PORT_MAPPING_CFG_H_
