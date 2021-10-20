
# include <ping.h>

# include <signal.h>
# include <unistd.h>
# include <stdlib.h>

# define GET_DESTADDR (*gctx.dest_dns != 0 ? gctx.dest_dns : gctx.dest_ip)

# define PRINT_TERMINATION_HDR(to, transm, receiv, error, time) \
        (error ? \
            printf("\n--- %s ping statistics ---\n%lu packets transmitted, %lu received, +%lu errors", \
			to, transm, receiv, error) \
        : \
            printf("\n--- %s ping statistics ---\n%lu packets transmitted, %lu received", \
            to, transm, receiv)); \
        transm ? \
            (transm >= receiv ? \
                printf(", %hhu%% packet loss, time %lums\n", (uint8_t)PERCENTAGE(receiv, transm), (uint64_t)time) \
            : \
                printf(" -- somebody's printing up packets!\n")) \
        : 0

# define PRINT_TERMINATION_RTT(min, max, sum, receiv) \
    printf("rtt min/avg/max = %.3f/%.3f/%.3f\n", \
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
    struct timeval tv;

    if (gettimeofday(&tv, &gctx.tz) != 0)
        goto end;
    tvsub(&tv, &gctx.start_time);
    PRINT_TERMINATION_HDR(GET_DESTADDR, gctx.nb_packets_transmited, gctx.nb_packets_received, gctx.nb_packets_error, TV_TO_MS(tv));
    if (gctx.nb_packets_received)
        PRINT_TERMINATION_RTT(gctx.tmin, gctx.tmax, gctx.tsum, gctx.nb_packets_received);
end:
    close(gctx.sockfd);
    exit(gctx.nb_packets_received == 0);
}
