
# include <ping.h>

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <stdlib.h>

#ifdef DEBUG
# include <string.h>
# include <errno.h>
# include <debug.h>

#endif

# include <debug.h>


# define PRINT_SOCKADDR_IN(s) (\
        printf("\nPRINT SOCKADDR:\n\t-sin_family: %hd\n\t-sin_port: %hu\n\t-sin_addr.s_addr: %u\n\n", \
            (s).sin_family, (s).sin_port, (s).sin_addr.s_addr) \
        )

# include <string.h>

# define TOTALPACKET_SIZE(msg_size) (sizeof(struct iphdr) + sizeof(struct icmphdr) + msg_size)

void send_ping4()
{
    uint8_t                 packet[MAX_PACKET_SIZE] = {0};
    struct iphdr* const     ip = (struct iphdr* const)packet;
    struct icmphdr* const   icp = (struct icmphdr* const)(packet + sizeof(*ip));
    struct timeval* const   tp = (struct timeval* const)(packet + sizeof(*ip) + sizeof(*icp));

    *ip = (struct iphdr){
        .version = 4,
        .ihl = sizeof(struct iphdr) / 4,
        .tos = OPT_HAS(OPT_TOS) ? gctx.parse.opts_args.tos : 0,
        .tot_len = gctx.packet_datalen + sizeof(*ip) + sizeof(*icp),
        .id = 0,
        .frag_off = 0,
        .ttl = OPT_HAS(OPT_TTL) ? gctx.parse.opts_args.ttl : 0X40,
        .protocol = 
#ifdef IS_IPV6_SUPORTED
        OPT_HAS(OPT_IPV6_ONLY) ? IPPROTO_ICMPV6 :
#endif
        IPPROTO_ICMP,
        .check = 0,
        .saddr = INADDR_ANY,
        .daddr = gctx.dest_sockaddr.sin_addr.s_addr
    };

    *icp = (struct icmphdr){
        .type = ICMP_ECHO,
        .code = 0,
        .checksum = 0,
        .un.echo.id = gctx.prog_id,
        .un.echo.sequence = gctx.nb_packets_transmited,
    };

    ///TODO: Don't like the idea to overwrite data...
    memset(packet + sizeof(*ip) + sizeof(*icp) + sizeof(*tp), 0XFF, gctx.packet_datalen); ///TODO: remove this

    if (gettimeofday(tp, &gctx.tz) != 0)
    {
        PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
        exit(ERR_SYSCALL);
    }

    // register size_t value = sizeof(struct timeval);
    // for (register uint8_t* ptr = (uint8_t*)tp + sizeof(struct timeval)
    // ; value < gctx.packet_datalen ; ptr++)
    //     *ptr = value++;


    const ssize_t total_packet_size = TOTALPACKET_SIZE(gctx.packet_datalen);

    icp->checksum = in_cksum((uint16_t*)(packet + sizeof(*ip)), gctx.packet_datalen + sizeof(*icp));

    const ssize_t bytes_sent = sendto(
        gctx.sockfd,
        (const void*)packet,
        (const size_t)total_packet_size,
        0,
        (const struct sockaddr*)&gctx.dest_sockaddr,
        sizeof(const struct sockaddr)
    );

    if (bytes_sent < 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "sendto");
		exit(ERR_SYSCALL);
	}
	else if (bytes_sent != total_packet_size)
	{
		printf(__progname ": wrote %s %ld chars, ret=%ld\n",
			gctx.dest_ip, total_packet_size, bytes_sent);
	}

    gctx.nb_packets_transmited++;

#ifdef DEBUG
    print_packet_data((uint8_t*)packet, total_packet_size, "SEND");
#endif

}

#ifdef IS_IPV6_SUPORTED

void send_ping6()
{

}

#endif
