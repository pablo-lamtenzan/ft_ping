# include <parse.h>
# include <ping.h>

///TODO: LIBC for parse ...
///TODO: Search why the makefile relink
///TODO: Handle ipv6 for new type in global context
///TODO: Hanlde ipv6 globaly
///TODO: mdev on stats
///TODO: verbose
///TODO: bonus flags (test & compare with true ping)
///TODO: remove extra header includes

# include <sys/socket.h>
# include <signal.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <unistd.h>

# define PRINT_HELP (printf("%s", USAGE_HELP))

# define PRINT_HEADER(hostaddr) (printf("PING %s (%s): %lu(%lu) bytes of data.\n", \
	*hostaddr ? hostaddr : gctx.dest_ip, \
	gctx.dest_ip, gctx.packet_datalen, gctx.packet_datalen + sizeof(struct iphdr) + sizeof(struct icmphdr)))

gcontext_t  gctx = (gcontext_t){
    .tmin = PSEUDOINFINTY,
    .packet_datalen = DEFAULT_DATALEN,
};

__attribute__ ((always_inline))
static inline void spetialize_by_ipv4()
{
    gctx.send_ping = &send_ping4;
    gctx.print_packet = &print_packet4;
}

#ifdef IS_IPV6_SUPORTED

__attribute__ ((always_inline))
static inline void spetialize_by_ipv6()
{
    gctx.send_ping = &send_ping6;
    gctx.print_packet = &print_packet6;
}

__attribute__ ((always_inline))
static inline void spetialize_by_ip_type(bool is_ipv6)
{ is_ipv6 ? spetialize_by_ipv6() : spetialize_by_ipv4(); }

#endif

int main(int ac, const char* av[])
{
    error_code_t st;

    if (IS_ROOT == false)
    {
        PRINT_ERROR("%s", MSG_MUST_BE_ROOT);
        st = ERR_PRIV;
        goto error;
    }

    if (ac == 1)
    {
        PRINT_ERROR("%s", MSG_REQUIRED_DESTINATION);
        st = ERR_DEST_REQ;
        goto error;
    }
    ++av;
    if ((st = parse_opts(&av)) != SUCCESS)
        goto error;

    gctx.nb_packets = gctx.parse.opts_args.count;

#ifdef DEBUG
    print_opts();
#endif

    if (OPT_HAS(OPT_HELP) || !*av)
    {
        PRINT_HELP;
        st = ERR_INV_OPT;
        goto error;
    }

#ifdef IS_IPV6_SUPORTED
    bool is_ipv6;
#endif

    if ((st = get_dest_info(av
#ifdef IS_IPV6_SUPORTED
        , &is_ipv6
#endif
    )) != SUCCESS)
        goto error;

    if (
#ifdef IS_IPV6_SUPORTED
        !is_ipv6 &&
#endif
        gctx.dest_sockaddr.sin_family != AF_INET)
	{
        PRINT_ERROR(MSG_INVALID_FAMILY, "not IPV4/IPV6");
        st = ERR_INV_FAM;
        goto error;
    }

    if ((st = init_socket(
#ifdef IS_IPV6_SUPORTED
        is_ipv6
#endif
    )) != SUCCESS)
        goto error;

    if (signal(SIGALRM, pinger_loop) == SIG_ERR
    || signal(SIGINT, terminate) == SIG_ERR)
    {
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "signal");
        goto error;
    }

#ifdef IS_IPV6_SUPORTED
    spetialize_by_ip_type(is_ipv6);
#else
    spetialize_by_ipv4();
#endif

    gctx.prog_id = getpid() & 0XFFFF;

    PRINT_HEADER(gctx.dest_dns);

    gettimeofday(&gctx.start_time, &gctx.tz);

    while (gctx.parse.opts_args.preload-- > 0)
        gctx.send_ping();

    pinger_loop();

    static uint8_t packet[MAX_PACKET_SIZE]; // addapt cause how i sent the ip header too
                                    // and have to handle IPV6 addresses too
    ssize_t bytes_recv;

    for ( ; ; )
    {
        if ((st = receive_pong(packet, ARR_SIZE(packet), &bytes_recv)) != SUCCESS)
        {
            if (st == CONTINUE)
                continue ;
            goto error;
        }
        if ((st = gctx.print_packet(packet, bytes_recv)) != SUCCESS)
            goto error;
        if (gctx.nb_packets && gctx.nb_packets_received - gctx.nb_packets == 0)
            terminate();
    }

error:
    close(gctx.sockfd);
    return st;
}
