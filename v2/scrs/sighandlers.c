
# include <ping.h>
# include <signal.h>

# define GET_DESTADDR gctx.dest_info->ai_canonname ? gctx.dest_info->ai_canonname : gctx.dest_dns

# define PRINT_TERMINATION_HDR(to, transm, receiv) \
        printf("\n----%s PING Statistics----\n%lu packets transmitted, %lu packets received", \
        to, transm, receiv); \
        transm ? \
            (transm >= receiv ? \
                printf(", %.1f%% packet loss\n", (double)PERCENTAGE(receiv, transm)) \
            : \
                printf(" -- somebody's printing up packets!\n")) \
        : 0

# define PRINT_TERMINATION_RTT(min, max, sum, receiv) \
    printf("rtt min/avg/max = %lu/%lu/lu\n", \
    min, sum / receiv, max)

void pinger_loop()
{
    gctx.send_ping();
    if (gctx.nb_packets == 0 || gctx.nb_packets_transmited <= gctx.nb_packets)
    alarm(1);
    else
    {
        const int32_t timeout = gctx.nb_packets_received ? \
        (gctx.tmax < 500 ? 1 : gctx.tmax * 2 / 1000) : MAXWAITTIME;

        signal(SIGALRM, terminate);
        alarm(timeout);
    }
}

void terminate()
{
    PRINT_TERMINATION_HDR(GET_DESTADDR, gctx.nb_packets_transmited, gctx.nb_packets_received);
    PRINT_TERMINATION_RTT(gctx.tmin, gctx.tmax, gctx.tsum, gctx.nb_packets_received);
    close(gctx.sockfd);
    exit(gctx.nb_packets_received == 0);
}
