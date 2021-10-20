# include <parse.h>
# include <ping.h>

///TODO: Search why the makefile relink
///TODO: Handle ipv6 for new type in global context
///TODO: Hanlde ipv6 globaly
///TODO: mdev on stats
///TODO: bonus flags (test & compare with true ping)

# include <sys/socket.h>
# include <signal.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <unistd.h>

#ifdef IS_IPV6_SUPORTED

#include <netinet/ip6.h>
#include <netinet/icmp6.h>

# define GET_SIZE_V6(mgs_size) (sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr) + (msg_size))

#endif

# define IS_IPV6(opts) (opts & OPT_IPV6_ONLY)

# define PRINT_HELP (printf("%s", USAGE_HELP))

# define GET_SIZE_V4(mgs_size) (sizeof(struct iphdr) + sizeof(struct icmphdr) + (msg_size))

///TODO: Size changes if ipv6 !!!
# define PRINT_HEADER(hostaddr) (printf("PING %s (%s): %lu(%lu) bytes of data.\n", \
	*hostaddr ? hostaddr : gctx.dest_ip, \
	gctx.dest_ip, gctx.packet_datalen, gctx.packet_datalen + sizeof(struct iphdr) + sizeof(struct icmphdr)))

__attribute__ ((always_inline))
static inline error_code_t check_initial_validity(int ac)
{
    error_code_t st = SUCCESS;

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
    }

error:
    return st;
}

__attribute__ ((always_inline))
static inline void start_ping_sender()
{
    PRINT_HEADER(gctx.dest_dns);

    gettimeofday(&gctx.start_time, &gctx.tz);

    while (gctx.parse.opts_args.preload-- > 0)
        gctx.send_ping();

    pinger_loop();
}

gcontext_t  gctx = (gcontext_t){
    .tmin = PSEUDOINFINTY,
    .packet_datalen = DEFAULT_DATALEN,
};

__attribute__ ((always_inline))
static inline void spetialize_by_ipv4()
{
    gctx.get_dest_info = &gest_dest_info4;
    gctx.init_socket = &init_socket4;
    gctx.send_ping = &send_ping4;
    gctx.print_packet = &print_packet4;
}

#ifdef IS_IPV6_SUPORTED

__attribute__ ((always_inline))
static inline void spetialize_by_ipv6()
{
    gctx.get_dest_info = &get_dest_info6;
    gctx.init_socket = &init_socket6;
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

    ++av;
    if ((st = check_initial_validity(ac)) != SUCCESS
    || (st = parse_opts(&av)) != SUCCESS)
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
    spetialize_by_ip_type(IS_IPV6(gctx.parse.opts));
#else
    spetialize_by_ipv4();
#endif

    if ((st = gctx.get_dest_info(av)) != SUCCESS
    || (st = gctx.init_socket()) != SUCCESS)
        goto error;

    if (signal(SIGALRM, pinger_loop) == SIG_ERR
    || signal(SIGINT, terminate) == SIG_ERR)
    {
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "signal");
        goto error;
    }

    gctx.prog_id = getpid() & 0XFFFF;

    start_ping_sender();

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
