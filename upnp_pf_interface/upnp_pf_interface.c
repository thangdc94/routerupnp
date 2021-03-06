/**
 * @file upnp_pf_interface.c
 * @brief Implement of Port Mapping Config using UPnP protocol
 * @details Implement of Port Mapping Config using UPnP protocol
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdlib.h>
#include <string.h>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

#include "llist.h"

#include "logutil.h"
#include "upnp_pf_interface.h"
#include "upnp_pf_errcode.h"
#include "netutil/netutil.h"

#ifdef LOG_LEVEL
#undef LOG_LEVEL
#define LOG_LEVEL LOG_DBG
#endif //LOG_LEVEL

/** Get string of input */
#define VAL(x) #x

/** Convert int to string */
#define STRINGIFY(x) VAL(x)

/** lease duration in string */
#define LEASE_DURATION_STR STRINGIFY(LEASE_DURATION)

/** Max number of times operation will be retried when error occured */
#define MAX_RETRY_ON_ERR 5

static struct UPNPUrls g_urls;
static struct IGDdatas g_data;
static char g_lanaddr[64]; /* my ip address on the LAN */
static char g_desc[13];

/* Function Prototypes */

int upnpPFInterface_init()
{
    struct UPNPDev *devlist = 0;
    int error = 0;
    int ipv6 = 0; // not use IPv6
    int i;
    char *desc;

    unsigned char ttl = 2; /* defaulting to 2 */
    LOG(LOG_INFO, "UPnP Discovering ...");

    // discovery device in network
    if ((devlist = upnpDiscover(2000, NULL /* multicastif */,
                                NULL /* minissdpdsock */,
                                UPNP_LOCAL_PORT_ANY, ipv6, ttl, &error)))
    {
        struct UPNPDev *device;

        LOG(LOG_INFO, "List of UPNP devices found on the network :");
        for (device = devlist; device; device = device->pNext)
        {
            printf("\n");
            LOG(LOG_INFO, "[desc]: %s | [st]: %s", device->descURL, device->st);
            if (UPNP_GetIGDFromUrl(device->descURL, &g_urls, &g_data, g_lanaddr, sizeof(g_lanaddr)) || (i = UPNP_GetValidIGD(devlist, &g_urls, &g_data, g_lanaddr, sizeof(g_lanaddr))))
            {
                switch (i)
                {
                case 1:
                    LOG(LOG_DBG, "Found valid IGD : %s", g_urls.controlURL);
                    break;
                case 2:
                    LOG(LOG_DBG, "Found a (not connected?) IGD : %s", g_urls.controlURL);
                    LOG(LOG_DBG, "Trying to continue anyway");
                    break;
                case 3:
                    LOG(LOG_DBG, "UPnP device found. Is it an IGD ? : %s", g_urls.controlURL);
                    LOG(LOG_DBG, "Trying to continue anyway");
                    break;
                default:
                    LOG(LOG_DBG, "Found device (igd ?) : %s", g_urls.controlURL);
                    LOG(LOG_DBG, "Trying to continue anyway");
                }

                if ((strcmp(g_lanaddr, "") == 0) || (strcmp(g_data.first.servicetype, "") == 0))
                {
                    LOG(LOG_WARN, "Not a valid UPnP Internet Gateway Device");
                    continue;
                }
                else
                {
                    LOG(LOG_DBG, "Local LAN ip address : %s", g_lanaddr);
                    
                    // use MAC Address as Description
                    desc = getmac_from_ip(g_lanaddr);
                    strcpy(g_desc, desc);
                    free(desc);
                    break;
                }
            }
            else
            {
                LOG(LOG_WARN, "Not a valid UPnP Internet Gateway Device");
                continue;
            }
        }

        // free device list
        freeUPNPDevlist(devlist);
        devlist = 0;
        if ((strcmp(g_lanaddr, "") == 0) || (strcmp(g_data.first.servicetype, "") == 0))
        {
            LOG(LOG_ERR, "No valid UPnP Internet Gateway Device found");
            return -1;
        }
    }
    else
    {
        LOG(LOG_ERR, "No IGD UPnP Device found on the network");
        return -1;
    }
    return SUCCESS;
}

static bool remove_rule(void *data)
{
    MappingRule_t *node = (MappingRule_t *)data;
    upnpPFInterface_removePortMapping(node->eport, node->proto);
    return TRUE;
}

int upnpPFInterface_addPortMapping(MappingRule_t rules[], int num_of_rules)
{
    const char *str_proto;
    int i, r;
    for (i = 0; i < num_of_rules; i++)
    {
        str_proto = get_proto_str(rules[i].proto);
        r = UPNP_AddPortMapping(g_urls.controlURL, g_data.first.servicetype,
                                rules[i].eport, rules[i].iport, g_lanaddr, g_desc,
                                str_proto, 0, LEASE_DURATION_STR);
        if (r != UPNPCOMMAND_SUCCESS)
        {
            LOG(LOG_ERR, "AddPortMapping(%s, %s, %s, %s) failed with code %d (%s)",
                rules[i].eport, rules[i].iport, g_lanaddr, str_proto, r, strupnperror(r));
            return -r;
        }
        LOG(LOG_INFO, "AddPortMapping(%s, %s, %s, %s) success", rules[i].eport, rules[i].iport, g_lanaddr, str_proto);
    }
    return SUCCESS;
}

int upnpPFInterface_diablePortMapping()
{
    int r = 0;
    int retry_count = 0;

    int i = 0;
    char index[6];      /* Port Mapping index */
    char intClient[40]; /* Internal Client IP */
    char intPort[6];    /* Internal Port Mapping */
    char extPort[6];    /* External Port Mapping */
    char protocol[4];   /* Protocol */
    char desc[80];      /* Port Mapping Description */
    char duration[16];  /* Expired Duration of this entry map */

    LOG(LOG_DBG, " i protocol exPort->inAddr:inPort description leaseTime");
    do
    {
        snprintf(index, 6, "%d", i);
        duration[0] = '\0';
        desc[0] = '\0';
        extPort[0] = '\0';
        intPort[0] = '\0';
        intClient[0] = '\0';
        r = UPNP_GetGenericPortMappingEntry(g_urls.controlURL,
                                            g_data.first.servicetype,
                                            index,
                                            extPort, intClient, intPort,
                                            protocol, desc, NULL /*enabled*/,
                                            NULL /* Remote Host */, duration);

        if (r == 0)
        {
            LOG(LOG_DBG, "%2d %s %5s->%s:%-5s '%s' %s",
                i, protocol, extPort, intClient, intPort,
                desc, duration);

            // check old port mapping rule
            if (strcmp(desc, g_desc) == 0)
            {
                // remove rule

                if (strcmp(protocol, "UDP") == 0)
                {
                    upnpPFInterface_removePortMapping(extPort, UDP);
                }
                else
                {
                    upnpPFInterface_removePortMapping(extPort, TCP);
                }
            }
        }
        else
        {
            // try again with the same index entry if r != 713 (SpecifiedArrayIndexInvalid)
            if (r != 713)
            {
                if (retry_count == MAX_RETRY_ON_ERR)
                {
                    LOG(LOG_ERR, "GetGenericPortMappingEntry() returned %d (%s)", r, strupnperror(r));
                    return -ERR_RETRY;
                }
                retry_count++;
                i--;
                LOG(LOG_WARN, "GetGenericPortMappingEntry() returned %d (%s). Retrying...", r, strupnperror(r));
                r = 0;
                continue;
            }
            else
            {
                LOG(LOG_DBG, "GetGenericPortMappingEntry() done");
            }
        }
        i++;
    } while (r == 0);
    return SUCCESS;
}

int upnpPFInterface_updatePortMapping(MappingRule_t rules[], int num_of_rules)
{
    int r = 0;
    int retry_count = 0;

    list list_remove;

    int i = 0;
    char index[6];      /* Port Mapping index */
    char intClient[40]; /* Internal Client IP */
    char intPort[6];    /* Internal Port Mapping */
    char extPort[6];    /* External Port Mapping */
    char protocol[4];   /* Protocol */
    char desc[80];      /* Port Mapping Description */
    char duration[16];  /* Expired Duration of this entry map */

    // create a linked list.
    // We don't need free function because we don't use malloc anywhere in our struct.
    list_new(&list_remove, sizeof(MappingRule_t), NULL /*freeFunction*/);

    LOG(LOG_DBG, " i protocol exPort->inAddr:inPort description leaseTime");
    do
    {
        snprintf(index, 6, "%d", i);
        duration[0] = '\0';
        desc[0] = '\0';
        extPort[0] = '\0';
        intPort[0] = '\0';
        intClient[0] = '\0';
        r = UPNP_GetGenericPortMappingEntry(g_urls.controlURL,
                                            g_data.first.servicetype,
                                            index,
                                            extPort, intClient, intPort,
                                            protocol, desc, NULL /*enabled*/,
                                            NULL /* Remote Host */, duration);

        if (r == 0)
        {
            LOG(LOG_DBG, "%2d %s %5s->%s:%-5s '%s' %s",
                i, protocol, extPort, intClient, intPort,
                desc, duration);

            // check old port mapping rule
            if (strcmp(desc, g_desc) == 0)
            {
                MappingRule_t rmnode;
                strcpy(rmnode.eport, extPort);
                strcpy(rmnode.iport, intPort);
                if (strcmp(protocol, "UDP") == 0)
                {
                    rmnode.proto = UDP;
                }
                else if (strcmp(protocol, "TCP") == 0)
                {
                    rmnode.proto = TCP;
                }
                // add to linked list
                list_append(&list_remove, &rmnode);
            }
        }
        else
        {
            // try again with the same index entry if r != 713 (SpecifiedArrayIndexInvalid)
            if (r != 713)
            {
                if (retry_count == MAX_RETRY_ON_ERR)
                {
                    LOG(LOG_ERR, "GetGenericPortMappingEntry() returned %d (%s)", r, strupnperror(r));
                    return -ERR_RETRY;
                }
                retry_count++;
                i--;
                LOG(LOG_WARN, "GetGenericPortMappingEntry() returned %d (%s). Retrying...", r, strupnperror(r));
                r = 0;
                continue;
            }
            else
            {
                LOG(LOG_DBG, "GetGenericPortMappingEntry() done");
            }
        }
        i++;
    } while (r == 0);

    // iterate over the linked list to remove old rules
    list_for_each(&list_remove, remove_rule);

    if (upnpPFInterface_addPortMapping(rules, num_of_rules) != SUCCESS)
    {
        //@todo rollback
        list_destroy(&list_remove);
        return -2;
    }

    list_destroy(&list_remove);
    return SUCCESS;
}

int upnpPFInterface_removePortMapping(const char *eport, SupportedProtocol_t proto)
{
    int r;
    const char *str_proto = get_proto_str(proto);
    r = UPNP_DeletePortMapping(g_urls.controlURL, g_data.first.servicetype, eport, str_proto, NULL);
    if (r != UPNPCOMMAND_SUCCESS)
    {
        LOG(LOG_ERR, "UPNP_DeletePortMapping(%s, %s) failed with code : %d (%s)", eport, str_proto, r, strupnperror(r));
        return -2;
    }
    LOG(LOG_INFO, "UPNP_DeletePortMapping(%s, %s) success", eport, str_proto);

    return SUCCESS;
}

int upnpPFInterface_destroy()
{
    FreeUPNPUrls(&g_urls);
    return SUCCESS;
}