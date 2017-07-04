/**
 * @file upnp_pf_interface.c
 * @brief 
 * @details 
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdlib.h>
#include <string.h>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

#include "logutil.h"
#include "upnp_pf_interface.h"
#include "netutil/netutil.h"

#ifdef LOG_LEVEL
#undef LOG_LEVEL
#define LOG_LEVEL LOG_DBG
#endif //LOG_LEVEL

/** Expired Duration for Port Mapping */
#define LEASE_DURATION "1440"

/** Port Mapping Entry */
typedef struct _PortMappingEntry_t
{
    /** Port Mapping index */
    char index[6];
    /** Internal Client IP */
    char intClient[40];
    /** Internal Port Mapping */
    char intPort[6];
    /** External Port Mapping */
    char extPort[6];
    /** Protocol */
    char protocol[4];
    /** Port Mapping Description */
    char desc[80];
    /** Is Entry enabled */
    char enabled[6];
    /** Remote Host */
    char rHost[64];
    /** Expired Duration of this entry map */
    char duration[16];
} PortMappingEntry_t;

static struct UPNPUrls *g_urls;
static struct IGDdatas *g_data;
static char g_lanaddr[64] = "unset"; /* my ip address on the LAN */
static char g_desc[13] = "description";

/* Function Prototypes */
static int get_list_port_mapping(PortMappingEntry_t *port_mapping_entry);

void upnpPFInterface_init()
{
    struct UPNPDev *devlist = 0;
    const char *multicastif = 0;
    const char *minissdpdpath = 0;
    int localport = UPNP_LOCAL_PORT_ANY;
    int error = 0;
    int ipv6 = 0;
    int i;
    char *desc;

    unsigned char ttl = 2; /* defaulting to 2 */
    LOG(LOG_INFO, "UPnP Discovering ...");
    if ((devlist = upnpDiscover(2000, multicastif, minissdpdpath,
                                localport, ipv6, ttl, &error)))
    {
        struct UPNPDev *device;

        if (devlist)
        {
            LOG(LOG_INFO, "List of UPNP devices found on the network :");
            for (device = devlist; device; device = device->pNext)
            {
                LOG(LOG_INFO, "[desc]: %s | [st]: %s\n", device->descURL, device->st);
            }
        }
        else
        {
            LOG(LOG_ERR, "upnpDiscover() error code=%d", error);
        }

        g_data = (struct IGDdatas *)malloc(sizeof(struct IGDdatas));
        g_urls = (struct UPNPUrls *)malloc(sizeof(struct UPNPUrls));

        if (UPNP_GetIGDFromUrl(0, g_urls, g_data, g_lanaddr, sizeof(g_lanaddr)) || (i = UPNP_GetValidIGD(devlist, g_urls, g_data, g_lanaddr, sizeof(g_lanaddr))))
        {
            switch (i)
            {
            case 1:
                LOG(LOG_DBG, "Found valid IGD : %s", g_urls->controlURL);
                break;
            case 2:
                LOG(LOG_DBG, "Found a (not connected?) IGD : %s", g_urls->controlURL);
                LOG(LOG_DBG, "Trying to continue anyway");
                break;
            case 3:
                LOG(LOG_DBG, "UPnP device found. Is it an IGD ? : %s", g_urls->controlURL);
                LOG(LOG_DBG, "Trying to continue anyway");
                break;
            default:
                LOG(LOG_DBG, "Found device (igd ?) : %s", g_urls->controlURL);
                LOG(LOG_DBG, "Trying to continue anyway");
            }
            LOG(LOG_DBG, "Local LAN ip address : %s", g_lanaddr);
            // use MAC Address as Description
            desc = getmac_from_ip(g_lanaddr);
            strcpy(g_desc, desc);
            free(desc);
        }
    }
}

int get_list_port_mapping(PortMappingEntry_t *port_mapping_entry)
{
    // @todo We may want to remove this function
    return 0;
}

int upnpPFInterface_addPortMapping(const char *eport, const char *iport, SupportedProtocol_t added_protocol)
{
    int r = 0;

    const char *str_proto;

    int i = 0;
    char index[6];
    char intClient[40];
    char intPort[6];
    char extPort[6];
    char protocol[4];
    char desc[80];
    char enabled[6];
    char rHost[64];
    char duration[16];
    printf(" i protocol exPort->inAddr:inPort description remoteHost leaseTime\n");
    do
    {
        snprintf(index, 6, "%d", i);
        rHost[0] = '\0';
        enabled[0] = '\0';
        duration[0] = '\0';
        desc[0] = '\0';
        extPort[0] = '\0';
        intPort[0] = '\0';
        intClient[0] = '\0';
        r = UPNP_GetGenericPortMappingEntry(g_urls->controlURL,
                                            g_data->first.servicetype,
                                            index,
                                            extPort, intClient, intPort,
                                            protocol, desc, enabled,
                                            rHost, duration);

        if (r == 0)
        {
            printf("%2d %s %5s->%s:%-5s '%s' '%s' %s\n",
                   i, protocol, extPort, intClient, intPort,
                   desc, rHost, duration);
            // check if conflict port
            if (strcmp(extPort, eport) == 0 && strcmp(desc, g_desc) != 0)
            {
                LOG(LOG_ERR, "External Port %s has been already in used", eport);
                // @todo define Error code
                return -2;
            }
            // @todo Need variable to
            // @todo Need check description == MAC for remove
        }
        else
        {
            // @todo UPNP_GetGenericPortMappingEntry again with the same index entry if r != 713 (SpecifiedArrayIndexInvalid)
            LOG(LOG_ERR, "GetGenericPortMappingEntry() returned %d (%s)", r, strupnperror(r));
        }
        i++;
    } while (r == 0);

    str_proto = get_proto_str(added_protocol);

    get_list_port_mapping(0);

    r = UPNP_AddPortMapping(g_urls->controlURL, g_data->first.servicetype,
                            eport, iport, g_lanaddr, g_desc,
                            str_proto, 0, "100000");
    if (r != UPNPCOMMAND_SUCCESS)
    {
        LOG(LOG_ERR, "AddPortMapping(%s, %s, %s) failed with code %d (%s)",
            eport, iport, g_lanaddr, r, strupnperror(r));
        return -2;
    }
    LOG(LOG_INFO, "AddPortMapping(%s, %s, %s) success", eport, iport, g_lanaddr);
    return 0;
}

int upnpPFInterface_removePortMapping(const char *eport, SupportedProtocol_t protcol)
{
    return 0;
}