
# include <packet.h>

# define TOTALMSG_SIZE(msg_size) (sizeof(struct iphdr) + sizeof(struct icmphdr) + msg_size)

void send_ping4()
{
    packet_t packet = (packet_t){
        .pk_ip = (struct iphdr){
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
            .daddr = gctx->dest_info.sin_addr.s_addr
        },
        .pk_icp = (struct icmphdr){
            .type = ICMP_ECHO,
            .un.echo.id = gctx.prog_id,
            .un.echo.sequence = gctx.nb_packets_transmited++
        },
    };

    gettimeofday(&packet.pk_tp, &gctx.tz);
    // maybe need to put here the time ... (don't think)

    for (uint8_t* ptr = &packet.pf_tp + sizeof(struct timeval), size_t value = sizeof(struct timeval)
    ; value < gctx.packet_datalen ; ptr++, value++)
        *ptr = value;

    const size_t total_msg_size = TOTALMSG_SIZE(gctx.packet_datalen);

    packet.pk_icp.checksum = in_cksum(packet.pk_raw, total_msg_size);

    const ssize_t bytes_sent = sendto(gctx.sockfd, &packet, total_msg_size,
    0, gctx.dest_info->ai_addr, sizeof(struct sockaddr));

    if (bytes_sent < 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "sendto");
		exit(ERR_SYSCALL); // I guess
	}
	else if (bytes_sent != msg_size)
	{
		printf(__progname ": wrote %s %ld chars, ret=%ld\n",
			gctx.hostaddr, total_msg_size, bytes_sent);
	}
}

#ifdef IS_IPV6_SUPORTED

void send_ping6()
{

}

#endif
