
# include <ping.h>

# include <linux/ip.h>
# include <netinet/ip_icmp.h>

void print_packet(const void* const buff, ssize_t packet_len)
{
    const struct iphdr* const ip = (const struct iphdr* const)buff;
    const struct icmphdr* const icp = (const struct icmphdr* const)(buff + sizeof(*ip));

    const uint8_t hl = ip->ihl << 2; // TODO: This value may change depend on endianess

    if (packet_len < hl + ICMP_MINLEN)
    {
        ///TODO: Verbose stuff
        return ;
    }

    if (icp->type == ICMP_ECHOREPLY)
    {
        if (icp->un.echo.id != gctx.progpid)
        {
            ///TODO: Maybe verbose stuff
            return ;
        }

        gctx.nb_packets_received++;

        struct timeval now_tv;
        gettimeofday(&now_tv, &gctx.tz);

        /// OPTION 1 (preferable)
        struct timeval* packet_tv = (struct timeval*)(buff + sizeof(*ip) + sizeof(*icp) + 4); // can directelly cast in next line
        tvsub(&now_tv, packet_tv);

        /// OPTION 2
        /// tvsub(&now_tv, &gctx.tsend_date );

        const double t = TV_TO_MS(now_tv);
        gctx.tsum += t;
        if (t < gctx.tmin)
            gctx.tmin = t;
        if (t > gctx.tmax)
            gctx.tmax = t;

        /// TODO: Inet_ntop + print stats
    }
}

#ifdef IS_IPV6_SUPORTED

void print_packet6(const void* const buff, ssize_t packet_len)
{
    
}

#endif

