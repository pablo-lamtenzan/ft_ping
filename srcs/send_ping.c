
#include <ping.h>
#include <ftlibc.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>

#ifdef IS_IPV6_SUPORTED

#include <netinet/ip6.h>
#include <netinet/icmp6.h>

#define TOTALPACKET_SIZE6(msg_size) (sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr) + msg_size)

#endif

#define TOTALPACKET_SIZE4(msg_size) (sizeof(struct iphdr) + sizeof(struct icmphdr) + msg_size)

#ifdef DEBUG

#include <string.h>
#include <errno.h>
#include <debug.h>

#endif

# include <errno.h>

#define PRINT_SOCKADDR_IN(s) (                                                                    \
	printf("\nPRINT SOCKADDR:\n\t-sin_family: %hd\n\t-sin_port: %hu\n\t-sin_addr.s_addr: %u\n\n", \
		   (s).sin_family, (s).sin_port, (s).sin_addr.s_addr))

void send_ping4()
{
	uint8_t packet[MAX_PACKET_SIZE] = {0};
	struct iphdr *const ip = (struct iphdr *)packet;
	struct icmphdr *const icp = (struct icmphdr *)(packet + sizeof(*ip));
	struct timeval *const tp = (struct timeval *)(packet + sizeof(*ip) + sizeof(*icp));

	*ip = (struct iphdr){
		.version = 4,
		.ihl = 5,
		.tos = OPT_HAS(OPT_TOS) ? gctx.parse.opts_args.tos : 0,
		.tot_len = gctx.packet_payloadlen + sizeof(*ip) + sizeof(*icp),
		.id = 0,
		.frag_off = 0,
		.ttl = OPT_HAS(OPT_TTL) ? gctx.parse.opts_args.ttl : 0X40,
		.protocol = IPPROTO_ICMP,
		.check = 0,
		.saddr = INADDR_ANY,
		.daddr = (*((struct sockaddr_in *)&gctx.dest_sockaddr)).sin_addr.s_addr};

	*icp = (struct icmphdr){
		.type = ICMP_ECHO,
		.code = 0,
		.checksum = 0,
		.un.echo.id = gctx.prog_id,
		.un.echo.sequence = gctx.nb_packets_transmited,
	};

	if (gctx.packet_payloadlen < sizeof(*tp)
	|| OPT_HAS(OPT_PATTERN))
	{
		if (gettimeofday(&gctx.aux_pktime, NULL) != 0)
		{
			PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
			exit(ERR_SYSCALL);
		}
	}
	else if (gettimeofday(tp, NULL) != 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
		exit(ERR_SYSCALL);
	}

	if (OPT_HAS(OPT_PATTERN))
		ft_memcpy(packet + sizeof(*ip) + sizeof(*icp),
		*gctx.parse.opts_args.pattern ? gctx.parse.opts_args.pattern : gctx.parse.opts_args.pattern + 1,
		*gctx.parse.opts_args.pattern ? ARR_SIZE(gctx.parse.opts_args.pattern) : ARR_SIZE(gctx.parse.opts_args.pattern) - 1);

	memset(packet + sizeof(*ip) + sizeof(*icp) + sizeof(*tp), PAYLOAD_BYTE, gctx.packet_payloadlen);

	const ssize_t total_packet_size = TOTALPACKET_SIZE4(gctx.packet_payloadlen);

	icp->checksum = in_cksum((uint16_t *)(packet + sizeof(*ip)), gctx.packet_payloadlen + sizeof(*icp));

	const ssize_t bytes_sent = sendto(
		gctx.sockfd,
		(const void *)packet,
		(const size_t)total_packet_size,
		0,
		(const struct sockaddr *)&gctx.dest_sockaddr,
		sizeof(const struct sockaddr));

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

	if (OPT_HAS(OPT_FLOOD))
	{
		printf("%c", '.');
		fflush(stdout);
	}

#ifdef DEBUG
	print_packet_data((uint8_t *)packet, total_packet_size, "SEND");
#endif
}

#ifdef IS_IPV6_SUPORTED

void send_ping6()
{
	uint8_t packet[MAX_PACKET_SIZE] = {0};
	struct ip6_hdr *const ip = (struct ip6_hdr *)packet;
	struct icmp6_hdr *const icp = (struct icmp6_hdr *)(packet + sizeof(*ip));
	struct timeval *const tp = (struct timeval *)(packet + sizeof(*ip) + sizeof(*icp));

	*ip = (struct ip6_hdr){
		.ip6_vfc = (uint8_t)(6 | OPT_HAS(OPT_TOS) ? (gctx.parse.opts_args.tos << 4) & 0XF0 : 0),
		.ip6_plen = sizeof(*icp) + gctx.packet_payloadlen,
		.ip6_nxt = IPPROTO_ICMPV6,
		.ip6_hops = OPT_HAS(OPT_TTL) ? gctx.parse.opts_args.ttl : 64,
		.ip6_src = IN6ADDR_ANY_INIT,
		.ip6_dst = (*((struct sockaddr_in6*)&gctx.dest_sockaddr)).sin6_addr
	};

	*icp = (struct icmp6_hdr){
		.icmp6_type = ICMP6_ECHO_REQUEST,
		.icmp6_code = 0,
		.icmp6_cksum = 0,
		.icmp6_dataun.icmp6_un_data16 = {gctx.prog_id, gctx.nb_packets_transmited}
	};

	if (gctx.packet_payloadlen < sizeof(*tp)
	|| OPT_HAS(OPT_PATTERN))
	{
		if (gettimeofday(&gctx.aux_pktime, NULL) != 0)
		{
			PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
			exit(ERR_SYSCALL);
		}
	}
	else if (gettimeofday(tp, NULL) != 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
		exit(ERR_SYSCALL);
	}

	if (OPT_HAS(OPT_PATTERN))
		ft_memcpy(packet + sizeof(*ip) + sizeof(*icp),
		*gctx.parse.opts_args.pattern ? gctx.parse.opts_args.pattern : gctx.parse.opts_args.pattern + 1,
		*gctx.parse.opts_args.pattern ? ARR_SIZE(gctx.parse.opts_args.pattern) : ARR_SIZE(gctx.parse.opts_args.pattern) - 1);

	memset(packet + sizeof(*ip) + sizeof(*icp) + sizeof(*tp), PAYLOAD_BYTE, gctx.packet_payloadlen);

	const ssize_t total_packet_size = TOTALPACKET_SIZE6(gctx.packet_payloadlen);

	///TODO: Checksum is calculated differently on ipv6
	icp->icmp6_cksum = in_cksum((uint16_t *)(packet + sizeof(*ip)), gctx.packet_payloadlen + sizeof(*icp));

	const ssize_t bytes_sent = sendto(
		gctx.sockfd,
		(const void *)packet,
		(const size_t)total_packet_size,
		0,
		(const struct sockaddr *)&gctx.dest_sockaddr,
		sizeof(const struct sockaddr));

	if (bytes_sent < 0)
	{
		printf("%s\n", strerror(errno));
		PRINT_ERROR(INVALID_SYSCALL, "sendto");
		exit(ERR_SYSCALL);
	}
	else if (bytes_sent != total_packet_size)
	{
		printf(__progname ": wrote %s %ld chars, ret=%ld\n",
			   gctx.dest_ip, total_packet_size, bytes_sent);
	}

	gctx.nb_packets_transmited++;

	if (OPT_HAS(OPT_FLOOD))
	{
		printf("%c", '.');
		fflush(stdout);
	}

#ifdef DEBUG
	print_packet_data((uint8_t *)packet, total_packet_size, "SEND");
#endif
}

#endif
