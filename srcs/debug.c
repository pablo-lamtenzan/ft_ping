/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 22:57:42 by pablo             #+#    #+#             */
/*   Updated: 2021/10/18 22:32:25 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef DEBUG

# include <parse.h>
# include <ping.h>

# include <stdio.h>
# include <stdint.h>

void print_opts()
{
    dprintf(2, "[DEBUG] Options:\n");
    static const char* const opts[] = {
        "-m", "-l", "-I", "-M",
        "-w", "-W", "-p", "-Q",
        "-S", "-t", "-T", "-c",
        "-4", "-6", "-v", "-h",
        "-f", "-n"
    };
    for (size_t i = 0 ; i < ARR_SIZE(opts) ; i++)
    {
        if (OPT_HAS(1 << i))
            printf("\t%s\n", opts[i]);
    }

    dprintf(2, "[DEBUG] Options values:\n\t-mark: %d\n\t-preload: %d\n"
    "\t-pmtudisc: %d\n\t-deadline: %d\n\t-timeout: %d\n\t-pattern: [%s]"
    "\n\t-tos: %d\n\t-sndbuff: %d\n\t-ttl: %d\n\t-timestamp: %d\n\n",
    gctx.parse.opts_args.mark, gctx.parse.opts_args.preload,
    gctx.parse.opts_args.pmtudisc_opts, gctx.parse.opts_args.deadline,
    gctx.parse.opts_args.timeout, USE_OPT_ARG_PATTERN,
    gctx.parse.opts_args.tos, gctx.parse.opts_args.sndbuff,
    gctx.parse.opts_args.ttl, gctx.parse.opts_args.timestamp);
}

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <sys/time.h>
# include <unistd.h>

# define PRINT_PKDATA(packet, tp) \
        for (uint8_t* i = (uint8_t*)packet + ((uint8_t*)tp + sizeof(*tp) - (uint8_t*)packet) -8 ; i < (uint8_t*)tp + sizeof(*tp) + 50 ; i++) \
        printf("[%lu] = 0X%hhX\n", i - (uint8_t*)packet - (56 - sizeof(*tp)) ,*i);

void print_packet_data(uint8_t* packet, ssize_t packet_size, const char* use)
{
    // struct iphdr* ip = (struct iphdr*)packet;
    // struct icmphdr* icp = (struct icmphdr*)(ip + sizeof(*ip));
    // struct timeval* tv = (struct timeval*)(icp + sizeof(*icp));

    struct iphdr* const     ip = (struct iphdr* const)packet;
    struct icmphdr* const   icp = (struct icmphdr* const)(packet + sizeof(*ip));
    struct timeval* const   tv = (struct timeval* const)(packet + sizeof(*ip) + sizeof(*icp) + 4);

    printf("\n *** ( %s ) print packet debug (size: %ld) ***\n\n", use, packet_size);
    printf("[DEBUG] ip header (size: %lu):\n\t-version: %hhu\n\t-ihl: %hhu\n\t-tos: %hhu\n\t-tot_len: %hu\n\t-id: %hu\n\t-frag_off: %hu\n\t-ttl: %hhu\n\t-protocol: %hhu\n\t-check: %hu\n\t-saddr: %u\n\t-daddr: %u\n\n",
    sizeof(*ip), ip->version, ip->ihl << 2, ip->tos, ip->tot_len, ip->id, ip->frag_off, ip->ttl, ip->protocol, ip->check, ip->saddr, ip->daddr);
    printf("[DEBUG] icmp header (size: %lu):\n\t-type: %hhu\n\t-code: %hhu\n\t-checksum: %hu\n\t-id: %hu\n\t-sequence: %hu\n\n",
    sizeof(*icp), icp->type, icp->code, icp->checksum, icp->un.echo.id, icp->un.echo.sequence);
    printf("[DEBUG] tv in msg data (size: %lu)\n\t-tv_sec: %ld\n\t-tv_usec: %ld\n\n",
    sizeof(*tv), tv->tv_sec, tv->tv_usec);
    printf("[DEBUG] msg data (size: %ld):\n", packet_size - (sizeof(*ip) + sizeof(*icp) + sizeof(*tv)));
    // for (ssize_t i = sizeof(*ip) + sizeof(*icp) + sizeof(*tv) ; i < packet_size ; i++)
    // {
    //     printf("[%ld] = %d ", i - (sizeof(*ip) + sizeof(*icp) + sizeof(*tv)) ,((char*)packet)[i]);
    //     if ((size_t)i < 10 + sizeof(*ip) + sizeof(*icp) + sizeof(*tv))
    //         write(1, " ", sizeof(" ") - 1);
    //     fflush(stdout);
    //     if (i % 4 == 0)
    //         write(1, "\n", sizeof("\n") - 1);
    // }
    PRINT_PKDATA(packet, tv);
    write(1, "\n\n", sizeof("\n\n") - 1);
}

void print_packet_bytes(const void* packet, ssize_t bytes_lenght)
{
    printf("\n*** print packet bytes (size: %ld) ***\n", bytes_lenght);
    for (ssize_t i = 0 ; i < bytes_lenght ; i++)
    {
        printf("[%ld]= { %03hhu } ", i, ((uint8_t*)packet)[i]);
        if (i < 10)
        {
            write(1, " ", sizeof(" "));
            fflush(stdout);
        }
        if (i % 4 == 0)
        {
            write(1, "\n", sizeof("\n") - 1);
            fflush(stdout);
        }
    }
    write(1, "\n\n", sizeof("\n\n") - 1);
    fflush(stdout);
}

# endif
