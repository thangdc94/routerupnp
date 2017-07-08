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

#include "mq_interface.h"
#include "upnp_pf_interface.h"
#include "upnp_pf_errcode.h"
#include "logutil.h"

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
        sleep(5);
    }
    int num_of_rules = 2;
    MappingRule_t rules[num_of_rules];
    rules[0] = (MappingRule_t){
        .eport = "8888",
        .iport = "8888",
        .proto = UDP,
    };

    rules[1] = (MappingRule_t){
        .eport = "9999",
        .iport = "9999",
        .proto = TCP,
    };

    mqInterface_create();
    char *msg_ptr = NULL;
    while (1)
    {
        mqInterface_receive(&msg_ptr);
        LOG(LOG_INFO, "Receive message %s", msg_ptr);
        free(msg_ptr);
        upnpPFInterface_updatePortMapping(rules, num_of_rules);
    }

    return 0;
}
