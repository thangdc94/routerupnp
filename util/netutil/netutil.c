/**
 * @file netutil.c
 * @brief Network Utility functions
 * @details Network Utility functions
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

char *getmac_from_ifname(char *iface)
{
    const int mac_string_length = 13;
    char *ret = malloc(mac_string_length);
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, iface);
    if (fd >= 0 && ret && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
    {
        int i;
        for (i = 0; i < 6; ++i)
            snprintf(ret + i * 2, mac_string_length - i * 2, "%02x", (unsigned char)s.ifr_addr.sa_data[i]);
    }
    else
    {
        perror("malloc/socket/ioctl failed");
        exit(1);
    }
    return (ret);
}

char *getmac_from_ip(const char *ip)
{
    struct ifaddrs *addrs, *iap;
    struct sockaddr_in *sa;
    char buf[32];
    char *macaddr = NULL;

    getifaddrs(&addrs);
    for (iap = addrs; iap != NULL; iap = iap->ifa_next)
    {
        if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) && iap->ifa_addr->sa_family == AF_INET)
        {
            sa = (struct sockaddr_in *)(iap->ifa_addr);
            inet_ntop(iap->ifa_addr->sa_family, (void *)&(sa->sin_addr), buf, sizeof(buf));
            if (!strcmp(ip, buf))
            {
                macaddr = getmac_from_ifname(iap->ifa_name);
                break;
            }
        }
    }
    freeifaddrs(addrs);
    return macaddr;
}