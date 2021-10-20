
# include <ping.h>
# include <ftlibc.h>

# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# define PRINT_STATS(bytes, from, ip, seq, ttl, time) ( \
            (!ip) ? \
                printf("%hu bytes from %s: icmp_seq=%hu ttl=%hu time=%.1f ms\n", \
                bytes, from, seq, ttl, time) \
            : \
            printf("%hu bytes from %s (%s): icmp_seq=%hu ttl=%hu time=%.1f ms\n", \
                bytes, from, ip, seq, ttl, time) \
        )

# define PRINT_ICMP_ERROR(from, ip, seq, verbose, error, error_info)  \
            ((ip) ? \
                printf("From %s (%s) ", from, ip) \
            : \
                printf("From %s ", from)); \
            ((verbose) ? \
                printf("icmp_seq=%hu %s %s\n", seq, error, error_info) \
            : \
                printf("icmp_seq=%hu %s\n", seq, error) \
        )

# define PRINT_PACKET_TO_SHORT_ERR(bytes, from) ( \
            printf("packet too short (%ld bytes) from %s\n", bytes, from) \
        )

#ifndef ICMP_MINLEN
# define ICMP_MINLEN 8
#endif

# define GET_TV_FROM_PACKET(packetptr) (struct timeval*)(packetptr + sizeof(struct iphdr) + sizeof(struct icmphdr))


__attribute__ ((always_inline))
static inline void handle_other_replies(struct icmphdr* const icp, const char* const address)
{
    char* type = "";
    char* code = type;

    switch (icp->type)
    {
        case ICMP_DEST_UNREACH:
            type = "Dest unreachable";
            switch (icp->code)
            {
                case ICMP_NET_UNREACH:
                    code = "Destination network unreachable";
                    break ;
                case ICMP_HOST_UNREACH:
                    code = "Destination host unreachable";
                    break ;
                case ICMP_PROT_UNREACH:
                    code = "Destination protocol unreachable";
                    break ;
                case ICMP_PORT_UNREACH:
                    code = "Destination port unreachable";
                    break ;
                case ICMP_FRAG_NEEDED:  
                    code = "Fragmentation required";
                    break ;
                case ICMP_SR_FAILED:
                    code = "Source route failed";
                    break ;
                case ICMP_NET_UNKNOWN:
                    code = "Destination network unknown";
                    break ;
                case ICMP_HOST_UNKNOWN:
                    code = "Destination host unknown";
                    break ;
                case ICMP_HOST_ISOLATED:
                    code = "Source host isolated";
                    break ;
                case ICMP_NET_ANO:
                    code = "Network administratively prohibited";
                    break ;
                case ICMP_HOST_ANO:
                    code = "Network administratively prohibited";
                    break ;
                case ICMP_NET_UNR_TOS:
                    code = "Network unreachable for ToS";
                    break ;
                case ICMP_HOST_UNR_TOS:
                    code = "Host unreachable for ToS";
                    break ;
                case ICMP_PKT_FILTERED:
                    code = "Communication administratively prohibited";
                    break ;
                case ICMP_PREC_VIOLATION:
                    code = "Host Precedence Violation";
                    break ;
                case ICMP_PREC_CUTOFF:
                    code = "Precedence cutoff in effect";
                    break ;
            }
            break ;

        case ICMP_REDIRECT:
            type = "Redirect Message";
            switch (icp->code)
            {
                case ICMP_REDIR_NET:
                    code = "Redirect Datagram for the Network";
                    break ;
                case ICMP_REDIR_HOST:
                    code = "Redirect Datagram for the Host";
                    break ;
                case ICMP_REDIR_NETTOS:
                    code = "Redirect Datagram for the ToS & network";
                    break ;
                case ICMP_REDIR_HOSTTOS:
                    code = "Redirect Datagram for the ToS & host";
                    break ;
            }
            break ;

        case ICMP_ECHO:
            type = "Echo request";
            code = "None";
            break ;

        case ICMP_TIME_EXCEEDED:
            type = "Time to live exceeded";
            if (icp->code == ICMP_EXC_TTL)
                code = "TTL expired in transit";
            else if (icp->code == ICMP_EXC_FRAGTIME)
                code = "Fragment reassembly time exceeded";
            break ;

/*
        case ICMP_PARAMETERPROB:
            break ;
        case ICMP_TIMESTAMP:
            break ;
        case ICMP_TIMESTAMPREPLY:
            break ;
        case 
*/
    }

    gctx.nb_packets_error++;

    /// TODO: Calc dest ip
    /// TODO: Is strcmp the way ?
    PRINT_ICMP_ERROR(address, ft_strcmp((const char*)gctx.dest_ip, (const char*)gctx.dest_dns) ? gctx.dest_ip : NULL,
    icp->un.echo.sequence, OPT_HAS(OPT_VERBOSE), type, code);
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

    // char addrbuff[INET_ADDRSTRLEN] = {0};

    // if (inet_ntop(AF_INET, gctx.dest_dns, addrbuff, ARR_SIZE(addrbuff)) == 0)
    // {
    //     PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
    //     st = ERR_SYSCALL;
    //     goto end;
    // }

    ///TODO: Maybe need to calculate using the ip header ther source address
    // NOT MAYBE, I MUST DO IT !!!!

    if (packet_len < (ip->ihl << 2) + ICMP_MINLEN)
        PRINT_PACKET_TO_SHORT_ERR(packet_len, gctx.dest_dns);

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
        PRINT_STATS((uint16_t)(((ft_ntohs(ip->tot_len)) & 0XFFFF) - sizeof(*ip)), gctx.dest_dns,
        ft_strcmp((const char*)gctx.dest_ip, (const char*)gctx.dest_dns) ? gctx.dest_ip : NULL, // TODO: replace strcmpby condition on GET_DESTADDR
        icp->un.echo.sequence, ip->ttl, get_time_diff(GET_TV_FROM_PACKET(packetbuff)));
    }
    else
        handle_other_replies((struct icmphdr*)icp, (char*)gctx.dest_ip);

end:
    return st;
}

#ifdef IS_IPV6_SUPORTED

error_code_t print_packet6(const void* const packetbuff, ssize_t packet_len)
{
    (void)packetbuff;
    (void)packet_len;
    return (SUCCESS);

    printf("TODO: PRINT PACKET\n");
}

#endif

