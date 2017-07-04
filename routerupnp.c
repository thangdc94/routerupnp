/**
 * @file routerupnp.c
 * @brief UPnP router config
 * @details Config port forwarding for router using UPnP Protocol
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */
#include "mq_interface.h"
#include "upnp_pf_interface.h"

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
    upnpPFInterface_init();
    upnpPFInterface_addPortMapping("8888", "8888", UDP);
    return 0;
}
