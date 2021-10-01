/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_handlers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 17:52:15 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:49:24 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <parse.h>
# include <ping.h>
# include <unistd.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>

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

//__attribute__ ((interrupt))
void terminate()
{

    // NOTE: Statistics changes between OS
    
    printf("\n----%s PING Statistics----\n"
    "%d packets transmitted, %d packets received",
    gctx.dest_info->ai_canonname,
    gctx.nb_packets_transmited, gctx.nb_packets_received);
    if (gctx.nb_packets_transmited)
    {
        if (gctx.nb_packets_transmited > gctx.nb_packets_received)
            printf(", %ld%% packet loss\n", (int64_t)PERCENTAGE(gctx.nb_packets_received, gctx.nb_packets_transmited));
        else
            printf(" -- somebody's printing up packets!\n");
    }
    // if timing
    if (gctx.nb_packets_received)
        printf("rrt min/avg/max = %ld/%ld/%ld\n", \
        gctx.tmin, gctx.tsum / gctx.nb_packets_received, gctx.tmax);

    close(gctx.sockfd);
    exit(gctx.nb_packets_received == 0);
}
