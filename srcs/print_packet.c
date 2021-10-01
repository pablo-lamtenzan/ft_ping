/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_packet.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 20:12:01 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:21:04 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <ping.h>
# include <netinet/ip_icmp.h>
# include <netinet/ip.h>
# include <stdio.h>
# include <sys/time.h>

# define PRINT_STATS(bytes, from, from_ip, seq, ttl, time) \
        printf("%d bytes form %s (%s): icmp_seq=%d ttl=%d time=%f ms\n",\
        bytes, from, from_ip, seq, ttl,time)

# define TV_TO_MS(tv) (tv.tv_sec * 1000 + (tv.tv_usec / 1000))

void    print_packet(const void* const packet, ssize_t packet_len, uint8_t family, const char* const from)
{
    const struct ip* const ip = (const struct ip* const)packet;

    const ssize_t header_len = ip->ip_hl << 2;

    if (packet_len < header_len + ICMP_MINLEN)
    {
        // to do some verbose stuff
        return ;
    }

    packet_len -= header_len;
    const struct icmp* const icmp = (const struct icmp* const)(packet + header_len);

    if (icmp->icmp_type == ICMP_ECHOREPLY)
    {
        if (icmp->icmp_id != gctx.pid)
        {
#define DEBUG
#ifdef DEBUG
            printf("[DEBUG] WNG!: received packet's id (\'%d\') != from \'%s\' id (\'%d\')\n", \
            icmp->icmp_id, __progname, gctx.pid);
#endif
            return ;
        }

        gctx.nb_packets_received++;

        // TO DO: if timing
        struct timeval tv;
        struct timeval* tp = (struct timeval*)icmp->icmp_data;
        gettimeofday(&tv, &gctx.tz);
        tvsub(&tv, tp);
        const ssize_t t = TV_TO_MS(tv);
        gctx.tsum += t;
        if (t < gctx.tmin)
            gctx.tmin = t;
        if (t > gctx.tmax)
            gctx.tmax = t;

        char buff[
#ifdef IS_IPV6_SUPORTED
            family == AF_INET6 ? INET6_ADDRSTRLEN :
#endif
            INET_ADDRSTRLEN];

#ifdef __linux__
        inet_ntop(family, from, buff, sizeof(buff)); // TO DO: Problem with buff
        PRINT_STATS(ip->ip_len, buff, from, icmp->icmp_seq, ip->ip_ttl, t);
#elif __APPLE__
        //PRINT_STATS("todo", );
#endif
    }

    // TO DO: Print routes if there are
}