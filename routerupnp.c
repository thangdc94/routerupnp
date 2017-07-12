/**
 * @file routerupnp.c
 * @brief UPnP router config
 * @details Config port forwarding for router using UPnP Protocol
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> /* to handle signal */
#include <cjson/cJSON.h>

#include "mq_interface.h"
#include "upnp_pf_interface.h"
#include "upnp_pf_errcode.h"
#include "logutil.h"
#include "portmappingcfg.h"

/**
 * Schedule timer period in second. If value is 0 no new alarm() is scheduled.
 * This should be the same as ::LEASE_DURATION so the rule will be stored on router
 * until device is removed from network.
 */
#define SCHEDULE_PERIOD LEASE_DURATION

/** Request message structure */
typedef struct _RequestMsg_t
{
    int pid;               /**< process id of the message */
    PortMappingCfg_t data; /**< data content of the message */
} RequestMsg_t;

static RequestMsg_t parse_request(const char *content)
{
    RequestMsg_t tmp;
    int i;
    cJSON *root = cJSON_Parse(content);
    cJSON *pid_json = cJSON_GetObjectItem(root, "pid");
    if (cJSON_IsNumber(pid_json))
    {
        tmp.pid = pid_json->valueint;
    }
    PortMappingCfg_t cfg;
    cJSON *cfg_json = cJSON_GetObjectItem(root, "data");
    if (cJSON_IsObject(cfg_json))
    {
        cJSON *enable_json = cJSON_GetObjectItem(cfg_json, "enable");
        if (cJSON_IsBool(enable_json))
        {
            cfg.is_enable = cJSON_IsTrue(enable_json);
        }
        cJSON *rules = cJSON_GetObjectItem(cfg_json, "rules");
        if (cJSON_IsArray(rules))
        {
            int arr_size = cJSON_GetArraySize(rules);
            cfg.numofrules = arr_size;
            MappingRule_t *map = malloc(arr_size * sizeof(MappingRule_t));
            cJSON *rule_item;
            for (i = 0; i < arr_size; i++)
            {
                rule_item = cJSON_GetArrayItem(rules, i);
                strcpy(map[i].eport, (cJSON_GetObjectItem(rule_item, "eport"))->valuestring);
                strcpy(map[i].iport, (cJSON_GetObjectItem(rule_item, "iport"))->valuestring);
                if (strcmp("UDP", (cJSON_GetObjectItem(rule_item, "proto"))->valuestring) == 0)
                {
                    map[i].proto = UDP;
                }
                else
                {
                    map[i].proto = TCP;
                }
            }
            cfg.rules = map;
        }
    }
    if (root != NULL)
    {
        cJSON_Delete(root);
    }
    tmp.data = cfg;
    return tmp;
}

static void update_rule()
{
    // read config file and update port mapping
    LOG(LOG_INFO, "Read config from config file and update port mapping");
    PortMappingCfg_t pm_cfg = PMCFG_getConfig();
    upnpPFInterface_updatePortMapping(pm_cfg.rules, pm_cfg.numofrules);
    free(pm_cfg.rules);
}

static void timer_handler(int signum)
{
    LOG(LOG_DBG, "Timer expired by signal: %s", strsignal(signum));
    update_rule();
    alarm(SCHEDULE_PERIOD); // new schedule
}

/**
 * @brief Main function
 * @details You know it's a main function
 * 
 * @param[in] argc Argument count. We don't use it
 * @param[in] argv Argument variables. We don't use it too 
 * 
 * @return Error code or 0 if OK
 */
int main(int argc, char const *argv[])
{
    while (SUCCESS != upnpPFInterface_init())
    {
        LOG(LOG_WARN, "upnpPFInterface_init() failed. Try again...");
        sleep(5);
    }

    update_rule();

    mqInterface_create();
    char *msg_ptr = NULL;

    /*
     * Implement timer using signal SIGALRM
     */
    signal(SIGALRM, timer_handler); // setup timer handler
    alarm(SCHEDULE_PERIOD);

    while (1)
    {
        if (mqInterface_receive(&msg_ptr) == 0)
        {
            LOG(LOG_INFO, "Receive message %s", msg_ptr);
            RequestMsg_t request = parse_request(msg_ptr);
            PortMappingCfg_t pm_cfg = request.data;

            free(msg_ptr);
            if (pm_cfg.is_enable)
            {
                if (upnpPFInterface_updatePortMapping(pm_cfg.rules, pm_cfg.numofrules) < 0)
                {
                    // handle error
                    mqInterface_send("Error", request.pid);
                }
                else
                {
                    alarm(SCHEDULE_PERIOD); // reset timer
                    PMCFG_saveConfig(&pm_cfg);
                    mqInterface_send("OK", request.pid);
                }
            }
            else
            {
                // remove rule and exit
                if (upnpPFInterface_diablePortMapping() < 0)
                {
                    // handle error
                    mqInterface_send("Error", request.pid);
                }
                else
                {
                    mqInterface_send("OK", request.pid);
                    PMCFG_saveConfig(&pm_cfg);
                    free(pm_cfg.rules);
                    upnpPFInterface_destroy();
                    exit(0);
                }
            }
            free(pm_cfg.rules);
        }
    }

    return 0;
}
