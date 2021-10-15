/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_packet.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plamtenz <plamtenz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 20:12:01 by pablo             #+#    #+#             */
/*   Updated: 2021/10/15 22:24:56 by plamtenz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <ping.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <stdio.h>
# include <sys/time.h>

#ifdef __linux__
# define PRINT_STATS(bytes, from, from_ip, seq, ttl, time) \
        printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%ld ms\n",\
        bytes, from, from_ip, seq, ttl, time)
#elif __APPLE__
# define PRINT_STATS(bytes, from, from_ip, seq, ttl, time) \
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", \
        bytes, from_ip, seq, ttl, (double)time)
#endif

# define TV_TO_MS(tv) (double)((double)(tv.tv_sec) * 1000.0 + (double)(tv.tv_usec) / 1000.0)


__attribute__ ((always_inline))
static inline void calc_dividendsum(double* const total, double x, double mean)
{ *total += (x - mean); }


void    print_packet(const void* const packet, ssize_t packet_len, uint8_t family)
{
    const struct iphdr* const ip = (const struct ip* const)packet;

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
//#define DEBUG
#ifdef DEBUG
            printf("[DEBUG] WNG!: received packet's id (\'%d\') != from \'%s\' id (\'%d\')\n", \
            icmp->icmp_id, __progname, gctx.pid);
#endif
            return ;
        }

        gctx.nb_packets_received++;

        // TO DO: if timing
        struct timeval tv;
        //struct timeval* tp = (struct timeval*)icmp->icmp_data;
        gettimeofday(&tv, &gctx.tz);
        tvsub(&tv, &gctx.tsend_date );
        const double t = TV_TO_MS(tv);
        gctx.tsum += t;
        if (t < gctx.tmin)
            gctx.tmin = t;
        if (t > gctx.tmax)
            gctx.tmax = t;

        calc_dividendsum(&gctx.stddevsum, t, (double)(gctx.tsum / gctx.nb_packets_received));

        char buff[
#ifdef IS_IPV6_SUPORTED
            family == AF_INET6 ? INET6_ADDRSTRLEN :
#endif
            INET_ADDRSTRLEN];

    inet_ntop(family, gctx.hostaddr, buff, sizeof(buff)); // TO DO: Problem with buff

#ifdef __linux__
        
        PRINT_STATS(int)packet_len, buff, gctx.hostaddr, icmp->icmp_seq, ip->ip_ttl, t);
#elif __APPLE__

        printf("[DEBUG] packet_len: %ld, ip header len: %ld ip->ip_hl: %d\n", packet_len, header_len, ip->ip_hl);

        PRINT_STATS((int)packet_len, buff, gctx.hostaddr, icmp->icmp_seq, ip->ip_ttl, t);
#endif
    }

    // TO DO: Print routes if there are
}