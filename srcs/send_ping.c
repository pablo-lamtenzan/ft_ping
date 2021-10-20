
#include <ping.h>

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

#define PRINT_SOCKADDR_IN(s) (                                                                    \
	printf("\nPRINT SOCKADDR:\n\t-sin_family: %hd\n\t-sin_port: %hu\n\t-sin_addr.s_addr: %u\n\n", \
		   (s).sin_family, (s).sin_port, (s).sin_addr.s_addr))

#include <string.h> /// TODO: Remove

void send_ping4()
{
	uint8_t packet[MAX_PACKET_SIZE] = {0};
	struct iphdr* const ip = (struct iphdr* const)packet;
	struct icmphdr* const icp = (struct icmphdr* const)(packet + sizeof(*ip));
	struct timeval* const tp = (struct timeval* const)(packet + sizeof(*ip) + sizeof(*icp));

	*ip = (struct iphdr){
		.version = 4,
		.ihl = sizeof(struct iphdr) >> 2,
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
		.daddr = (*((struct sockaddr_in*)&gctx.dest_sockaddr)).sin_addr.s_addr};

	*icp = (struct icmphdr){
		.type = ICMP_ECHO,
		.code = 0,
		.checksum = 0,
		.un.echo.id = gctx.prog_id,
		.un.echo.sequence = gctx.nb_packets_transmited,
	};

	if (gettimeofday(tp, &gctx.tz) != 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
		exit(ERR_SYSCALL);
	}

	memset(packet + sizeof(*ip) + sizeof(*icp) + sizeof(*tp), 0XFF, gctx.packet_datalen);

	const ssize_t total_packet_size = TOTALPACKET_SIZE4(gctx.packet_datalen);

	icp->checksum = in_cksum((uint16_t*)(packet + sizeof(*ip)), gctx.packet_datalen + sizeof(*icp));

	const ssize_t bytes_sent = sendto(
		gctx.sockfd,
		(const void*)packet,
		(const size_t)total_packet_size,
		0,
		(const struct sockaddr*)&gctx.dest_sockaddr,
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

#ifdef DEBUG
	print_packet_data((uint8_t *)packet, total_packet_size, "SEND");
#endif
}

#ifdef IS_IPV6_SUPORTED

void send_ping6()
{
	uint8_t packet[MAX_PACKET_SIZE] = {0};
	struct ip6_hdr* const ip = (struct ip6_hdr*)packet;
	struct icmp6_hdr* const icp = (struct icmp6_hdr*)(packet + sizeof(*ip));
	struct timeval* const tp = (struct timeval*)(packet + sizeof(*ip) + sizeof(*icp));

	///TODO: INIT MORE !!!
	// *ip = (struct ip6_hdr){
	// 	.ip6_src = INADDR_ANY,
	// 	.ip6_dst = (*((struct sockaddr_in6*)&gctx.dest_sockaddr)).sin6_addr.__in6_u
	// };

	*icp = (struct icmp6_hdr){
		.icmp6_type = ICMP_ECHO,
		.icmp6_code = 0,
		.icmp6_cksum = 0,
		.icmp6_dataun.icmp6_un_data16 = {gctx.prog_id, gctx.nb_packets_transmited}
	};

	if (gettimeofday(tp, &gctx.tz) != 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "gettimeofday");
		exit(ERR_SYSCALL);
	}

	memset(packet + sizeof(*ip) + sizeof(*icp) + sizeof(*tp), 0XFF, gctx.packet_datalen);

	const ssize_t total_packet_size = TOTALPACKET_SIZE6(gctx.packet_datalen);

	icp->icmp6_cksum = in_cksum((uint16_t*)(packet + sizeof(*ip)), gctx.packet_datalen + sizeof(*icp));

	const ssize_t bytes_sent = sendto(
		gctx.sockfd,
		(const void*)packet,
		(const size_t)total_packet_size,
		0,
		(const struct sockaddr*)&gctx.dest_sockaddr,
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

#ifdef DEBUG
	print_packet_data((uint8_t *)packet, total_packet_size, "SEND");
#endif
}

#endif
