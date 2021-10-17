
# include <ping.h>

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <stdlib.h>

# define TOTALMSG_SIZE(msg_size) (sizeof(struct iphdr) + sizeof(struct icmphdr) + msg_size)

void send_ping4()
{
    uint8_t packet[MAX_PACKET_SIZE];
    struct iphdr* const ip = (struct iphdr* const)packet;
    struct icmphdr* const icp = (struct icmphdr* const)(packet + sizeof(*ip));
    struct timeval* const tp = (struct timeval* const)(packet + sizeof(*ip) + sizeof(*icp));

    *ip = (struct iphdr){
        .version = 4,
        .ihl = sizeof(struct iphdr) / 4,
        .tos = 0,
        .tot_len = gctx.packet_datalen + sizeof(struct iphdr),
        .id = 0,
        .frag_off = 0,
        .ttl = 42,
        .protocol = IPPROTO_ICMP,
        .check = 0,
        .saddr = INADDR_ANY,
        .daddr = ((struct sockaddr_in*)gctx.dest_info)->sin_addr.s_addr
    };

    *icp = (struct icmphdr){
        .type = ICMP_ECHO,
        .un.echo.id = gctx.prog_id,
        .un.echo.sequence = gctx.nb_packets_transmited++,
        .checksum = 0
    };

    gettimeofday(tp, &gctx.tz);
    // maybe need to put here the time ... (don't think)

    register size_t value = sizeof(struct timeval);
    for (register uint8_t* ptr = (uint8_t*)tp + sizeof(struct timeval)
    ; value < gctx.packet_datalen ; ptr++)
        *ptr = value++;

    const ssize_t total_msg_size = TOTALMSG_SIZE(gctx.packet_datalen);

    icp->checksum = in_cksum((uint16_t*)packet, total_msg_size);

    const ssize_t bytes_sent = sendto(gctx.sockfd, &packet, total_msg_size,
    0, gctx.dest_info->ai_addr, sizeof(struct sockaddr));

    if (bytes_sent < 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "sendto");
		exit(ERR_SYSCALL); // I guess
	}
	else if (bytes_sent != total_msg_size)
	{
		printf(__progname ": wrote %s %ld chars, ret=%ld\n",
			gctx.dest_dns, total_msg_size, bytes_sent);
	}
}

#ifdef IS_IPV6_SUPORTED

void send_ping6()
{

}

#endif
