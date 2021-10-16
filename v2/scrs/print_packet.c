
# include <ping.h>

# include <linux/ip.h>
# include <netinet/ip_icmp.h>

# define PRINT_STATS(bytes, from, seq, ttl, time) \
        printf("%hu bytes from %s: icmp seq=%hu ttl=%hu time=%.3f ms\n", \
        bytes, from, seq, ttl, time)

# define PRINT_ICMP_ERROR(from, seq, verbose, error, error_info) \
        (verbose ? \
            printf("From %s icmp_seq=%hu %s %s\n", \
            from, seq, error, error_info) \
        : \
            printf("From %s icmp_seq=%hu %s\n" \
            from, seq, error))

#ifndef ICMP_MINLEN
# define ICMP_MINLEN 8
#endif


# define GET_TV_FROM_PACKET(packetptr) (packetptr + sizeof(struct iphdr) + sizeof(struct icmphdr) + 4)
# define TV_TO_MS(tv) (double)((double)(tv.tv_sec) * 1000.0 + (double)(tv.tv_usec) / 1000.0)

__attribute__ ((always_inline))
static inline void handle_other_replies(struct icmphdr* icp, char* const address, const void* const packetbuff)
{
    icp = packetbuff + 48;
    static const char* const errors[] = {
        ICMP_ERROR_MSG_0,  ICMP_ERROR_MSG_1,  ICMP_ERROR_MSG_2,
        ICMP_ERROR_MSG_3,  ICMP_ERROR_MSG_4,  ICMP_ERROR_MSG_5,
        ICMP_ERROR_MSG_6,  ICMP_ERROR_MSG_7,  ICMP_ERROR_MSG_8,
        ICMP_ERROR_MSG_9,  ICMP_ERROR_MSG_10, ICMP_ERROR_MSG_11,
        ICMP_ERROR_MSG_12, ICMP_ERROR_MSG_13, ICMP_ERROR_MSG_14,
        ICMP_ERROR_MSG_15, ICMP_ERROR_MSG_16, ICMP_ERROR_MSG_17,
        ICMP_ERROR_MSG_18, ICMP_ERROR_MSG_19,
    };

    static const char* const errors_info[] = {
        ICMP_ERROR_INFO_MSG_0,  ICMP_ERROR_INFO_MSG_1,
        ICMP_ERROR_INFO_MSG_2,  ICMP_ERROR_INFO_MSG_3,
        ICMP_ERROR_INFO_MSG_4,  ICMP_ERROR_INFO_MSG_5,
        ICMP_ERROR_INFO_MSG_6,  ICMP_ERROR_INFO_MSG_7,
        ICMP_ERROR_INFO_MSG_8,  ICMP_ERROR_INFO_MSG_9,
        ICMP_ERROR_INFO_MSG_10, ICMP_ERROR_INFO_MSG_11,
        ICMP_ERROR_INFO_MSG_12, ICMP_ERROR_INFO_MSG_13,
        ICMP_ERROR_INFO_MSG_14, ICMP_ERROR_INFO_MSG_15,
        ICMP_ERROR_INFO_MSG_16,
    };

    /// TODO: Perhabs the printing format is bad or there could be some condition too
    PRINT_ICMP_ERROR(address, icp->un.echo.sequence, OPT_HAS(OPT_VERBOSE), errors[icp->type], errors_info[icp->code]);
}

static double get_time_diff(struct timeval* packet_tv)
{
    struct timeval now_tv;
    gettimeofday(&now_tv, &gctx.tz);

    /// OPTION 1 (preferable)
    tvsub(&now_tv, packet_tv);

    /// OPTION 2
    /// tvsub(&now_tv, &gctx.tsend_date );

    const double t = TV_TO_MS(now_tv);
    gctx.tsum += t;
    if (t < gctx.tmin)
        gctx.tmin = t;
    if (t > gctx.tmax)
        gctx.tmax = t;
    return t;
}

error_code_t print_packet4(const void* const packetbuff, ssize_t packet_len)
{

    ///TODO: A version using packet_t

    error_code_t st = SUCCESS;
    const struct iphdr* const ip = (const struct iphdr* const)packetbuff;
    const struct icmphdr* const icp = (const struct icmphdr* const)(packetbuff + sizeof(*ip));

    const uint8_t hl = ip->ihl << 2; // TODO: This value may change depend on endianess

    char addrbuff[INET_ADDRSTRLEN] = {0};

    if (inet_ntop(AF_INET, gctx.dest_dns, addrbuff, ARR_SIZE(addrbuff)) == 0)
    {
        PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
        st = ERR_SYSCALL;
        goto end;
    }

    if (packet_len < hl + ICMP_MINLEN)
    {
        ///TODO: Verbose stuff
        goto end;
    }

    if (icp->type == ICMP_ECHOREPLY)
    {
        if (icp->un.echo.id != gctx.prog_id)
        {
            ///TODO: Maybe verbose stuff
            goto end;
        }

        gctx.nb_packets_received++;

        /// TODO: ntohs forbiden !!!
        /// TODO: Perhabs the printing format is bad or there could be some condition too
        PRINT_STATS(((ntohs(ip->tot_len)) & 0XFFFF) - sizeof(*ip), addrbuff,
        icp->un.echo.sequence, ip->ttl, get_time_diff(GET_TV_FROM_PACKET(packetbuff)));
    }
    else
        handle_other_replies(icp, addrbuff, packetbuff);

end:
    return st;
}

#ifdef IS_IPV6_SUPORTED

error_code_t print_packet6(const void* const packetbuff, ssize_t packet_len)
{
    
}

#endif

