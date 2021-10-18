
# include <ping.h>

# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <string.h> /// TODO: LIBC

# define PRINT_STATS(bytes, from, ip, seq, ttl, time) ( \
            (!ip) ? \
                printf("%hu bytes from %s: icmp_seq=%hu ttl=%hu time=%.1f ms\n", \
                bytes, from, seq, ttl, time) \
            : \
            printf("%hu bytes from %s (%s): icmp_seq=%hu ttl=%hu time=%.1f ms\n", \
                bytes, from, ip, seq, ttl, time) \
        )

# define PRINT_ICMP_ERROR(from, seq, verbose, error, error_info) ( \
        (verbose) ? \
            printf("From %s icmp_seq=%hu %s %s\n", \
            from, seq, error, error_info) \
        : \
            printf("From %s icmp_seq=%hu %s\n", \
            from, seq, error) \
        )

#ifndef ICMP_MINLEN
# define ICMP_MINLEN 8
#endif

# define GET_TV_FROM_PACKET(packetptr) (struct timeval*)(packetptr + sizeof(struct iphdr) + sizeof(struct icmphdr))

__attribute__ ((always_inline))
static inline void handle_other_replies(struct icmphdr* icp, char* const address, const void* const packetbuff)
{
    icp = (struct icmphdr*)(packetbuff + 48);
    static const char* const errors[] = {
        ICMP_ERROR_MSG_0,  ICMP_ERROR_MSG_1,  ICMP_ERROR_MSG_2,
        ICMP_ERROR_MSG_3,  ICMP_ERROR_MSG_4,  ICMP_ERROR_MSG_5,
        ICMP_ERROR_MSG_6,  ICMP_ERROR_MSG_7,  ICMP_ERROR_MSG_8,
        ICMP_ERROR_MSG_9,  ICMP_ERROR_MSG_10, ICMP_ERROR_MSG_11,
        ICMP_ERROR_MSG_12, ICMP_ERROR_MSG_13, ICMP_ERROR_MSG_14,
        ICMP_ERROR_MSG_15, ICMP_ERROR_MSG_16, ICMP_ERROR_MSG_17,
        ICMP_ERROR_MSG_18
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
        ICMP_ERROR_INFO_MSG_16
    };

    /// TODO: Perhabs the printing format is bad or there could be some condition too
    PRINT_ICMP_ERROR(address, icp->un.echo.sequence, OPT_HAS(OPT_VERBOSE), errors[icp->type], errors_info[icp->code]);
}

static double get_time_diff(struct timeval* packet_tv)
{
    struct timeval now_tv;

    gettimeofday(&now_tv, &gctx.tz);
    tvsub(&now_tv, packet_tv);

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
    error_code_t st = SUCCESS;
    const struct iphdr* const ip = (const struct iphdr* const)packetbuff;
    const struct icmphdr* const icp = (const struct icmphdr* const)(packetbuff + sizeof(*ip));

    const uint8_t hl = ip->ihl << 2; // TODO: This value may change depend on endianess

    // char addrbuff[INET_ADDRSTRLEN] = {0};

    // if (inet_ntop(AF_INET, gctx.dest_dns, addrbuff, ARR_SIZE(addrbuff)) == 0)
    // {
    //     PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
    //     st = ERR_SYSCALL;
    //     goto end;
    // }

    ///TODO: Maybe need to calculate using the ip header ther source address
    // NOT MAYBE, I MUST DO IT !!!!

    if (packet_len < hl + ICMP_MINLEN)
    {
        ///TODO: Verbose stuff
        printf("%s", "{DEBUG} PACKET TOO SHORT\n");
        goto end;
    }

    if (icp->type == ICMP_ECHOREPLY)
    {
        if (icp->un.echo.id != gctx.prog_id)
        {
            ///TODO: Maybe verbose stuff
            printf("{DEBUG} ID: %hu != from prog id (%hu)\n", icp->un.echo.id, gctx.prog_id);
            goto end;
        }

        gctx.nb_packets_received++;

        /// TODO: ntohs forbiden !!!
        /// TODO: Perhabs the printing format is bad or there could be some condition too
        PRINT_STATS((uint16_t)(((ntohs(ip->tot_len)) & 0XFFFF) - sizeof(*ip)), gctx.dest_dns,
        strcmp((const char*)gctx.dest_ip, (const char*)gctx.dest_dns) ? gctx.dest_ip : NULL,
        icp->un.echo.sequence, ip->ttl, get_time_diff(GET_TV_FROM_PACKET(packetbuff)));
    }
    else
        handle_other_replies((struct icmphdr*)icp, (char*)gctx.dest_ip, packetbuff);

end:
    return st;
}

#ifdef IS_IPV6_SUPORTED

error_code_t print_packet6(const void* const packetbuff, ssize_t packet_len)
{
    (void)packetbuff;
    (void)packet_len;
    return (SUCCESS);
}

#endif

