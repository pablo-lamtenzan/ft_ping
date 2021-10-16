/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handlers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 17:52:15 by pablo             #+#    #+#             */
/*   Updated: 2021/10/16 20:18:45 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <ping.h>
# include <unistd.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>

# define ABS(x) ((x) >= 0 ? (x) : -(x))

//__attribute__ ((interrupt))
void pinger_loop()
{
    pinger();
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

__attribute__ ((always_inline))
static inline double calc_standard_deviation(double dividendsum, double amount)
{ return (dividendsum / amount); }

//__attribute__ ((interrupt))
void terminate()
{

    // NOTE: Statistics changes between OS

    printf(
#ifdef __linux__
    "\n----%s PING Statistics----\n"
#elif __APPLE__
    "\n--- %s ping statistics ---\n"
#endif
    "%ld packets transmitted, %ld packets received",
    gctx.dest_info->ai_canonname ? gctx.dest_info->ai_canonname : gctx.hostaddr,
    (long)gctx.nb_packets_transmited, (long)gctx.nb_packets_received);
    if (gctx.nb_packets_transmited)
    {
        if (gctx.nb_packets_transmited >= gctx.nb_packets_received)
            printf(", %.1f%% packet loss\n", (double)PERCENTAGE(gctx.nb_packets_received, gctx.nb_packets_transmited));
        else
            printf(" -- somebody's printing up packets!\n");
    }
    // if timing
    if (gctx.nb_packets_received)
#ifdef  __linux__
        printf("rrt min/avg/max = %ld/%ld/%ld\n", \
        (long)gctx.tmin, (long)(gctx.tsum / gctx.nb_packets_received), (long)gctx.tmax);
#elif __APPLE__
        printf("round-trip min/avg/max/stddev = %.3lf/%.3lf/%.3lf/%.3lf ms\n",
        (double)gctx.tmin, (double)(gctx.tsum / gctx.nb_packets_received), (double)gctx.tmax,
        ABS(calc_standard_deviation(gctx.stddevsum, gctx.nb_packets_received)));
#endif

    close(gctx.sockfd);
    exit(gctx.nb_packets_received == 0);
}

void info()
{
    dprintf(STDERR_FILENO, "cmd: " __progname " %d running %.2fu %.2fs\n%lld/%lld packets received (%.1f%%)"
    "%.3f min / %.3f avg / %.3f max\n", gctx.pid, 0.0, 0.0, gctx.nb_packets_received, gctx.nb_packets_transmited,
    (double)PERCENTAGE(gctx.nb_packets_received, gctx.nb_packets_transmited), gctx.tmin,
    (double)(gctx.tsum / gctx.nb_packets_received), gctx.tmax);

}
