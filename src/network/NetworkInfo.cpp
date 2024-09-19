#include "NetworkInfo.h"

#include "../Globals.h"

#ifdef _WIN32

#elif __linux__
//#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#endif

NetworkInfo::NetworkInfo() : BaseInfo()
{
    qDebug() << __FUNCTION__;
}

NetworkInfo::~NetworkInfo()
{
    qDebug() << __FUNCTION__;
}

void NetworkInfo::init()
{
    readStaticInfo();
}

void NetworkInfo::update()
{
    readDynamicInfo();
}

void NetworkInfo::readStaticInfo()
{
#ifdef _WIN32

#elif __linux__

#endif
}

void NetworkInfo::readDynamicInfo()
{
#ifdef _WIN32

#elif __linux__
    readInterfaces();
#endif

    setDynamicInfo(Globals::SysInfoAttr::Key_Network_Dynamic_Names, QVariant::fromValue(m_networkNames));
    setDynamicInfo(Globals::SysInfoAttr::Key_Network_Dynamic_BytesReceivedPerSec, QVariant::fromValue(m_networkBytesReceivedPerSec));
    setDynamicInfo(Globals::SysInfoAttr::Key_Network_Dynamic_BytesSentPerSec, QVariant::fromValue(m_networkBytesSentPerSec));
    setDynamicInfo(Globals::SysInfoAttr::Key_Network_Dynamic_TotalBytesPerSec, QVariant::fromValue(m_networkBytesTotalPerSec));
}

#ifdef __linux__

void NetworkInfo::readInterfaces()
{
    struct ifaddrs *ifaddr;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    m_networkNames.clear();
    m_networkBytesReceivedPerSec.clear();
    m_networkBytesSentPerSec.clear();
    m_networkBytesTotalPerSec.clear();

    /* Walk through linked list, maintaining head pointer so we
              can free list later. */

    std::map<std::string,std::pair<uint32_t, uint32_t>> afPackets;

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family == AF_INET6) //ignore ipv6 for now
        {
            continue;
        }

        const sa_family_t family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
                  form of the latter for the common families). */

        const std::string interface_name = std::string(ifa->ifa_name);

        /* For an AF_INET* interface address, display the address. */

        if(family == AF_INET || family == AF_INET6)
        {
            s = getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);

            if (s != 0) {
                qWarning() << __FUNCTION__ << " getnameinfo() failed: " << gai_strerror(s);
                return;
            }

            char buffer[INET6_ADDRSTRLEN];

            std::string ipaddress_human_readable_form;
            const void *__restrict cp{nullptr};

            if(family == AF_INET)
            {
                cp = &((struct sockaddr_in*)(ifa->ifa_addr))->sin_addr;
            }
            else
            {
                cp = &((struct sockaddr_in6*)(ifa->ifa_addr))->sin6_addr;
            }

            inet_ntop(family, cp, buffer, sizeof(buffer));

            ipaddress_human_readable_form = std::string(buffer);
            m_networkNames.push_back(interface_name + " : " + ipaddress_human_readable_form);

            //qDebug() << "\t\taddress: <%s>\n" << host;
        }
        else if(family == AF_PACKET && ifa->ifa_data != NULL)
        {
            struct rtnl_link_stats *stats = static_cast<rtnl_link_stats*>(ifa->ifa_data);
            afPackets[interface_name] = {stats->rx_bytes, stats->tx_bytes};
        }
    }

    freeifaddrs(ifaddr);

    for (auto&& afPacket : afPackets)
    {
        for (auto&& name : m_networkNames)
        {
            if(name.find(afPacket.first)!=std::string::npos)
            {
                m_networkBytesReceivedPerSec.push_back(afPacket.second.first);
                m_networkBytesSentPerSec.push_back(afPacket.second.second);
                m_networkBytesTotalPerSec.push_back(afPacket.second.first + afPacket.second.second);
                break;
            }
        }
    }

    qDebug();
}
#endif
