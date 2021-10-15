/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pinger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 19:29:35 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:51:42 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <packet.h>
# include <sys/socket.h>
# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>

void pinger()
{
#ifndef __linux__
	packet_t packet_to_send = (packet_t){
			.p_icp = (struct icmp){
			.icmp_type = ICMP_ECHO,
			.icmp_code = 0,
			.icmp_cksum = 0,
			.icmp_seq = gctx.nb_packets_transmited++,
			.icmp_id = gctx.pid
		}
	};
#elif __APPLE__ // to remove
	packet_t packet_to_send = {0};
	packet_to_send.p_icp.icmp_type = ICMP_ECHO;
	packet_to_send.p_icp.icmp_seq = gctx.nb_packets_transmited++;
	packet_to_send.p_icp.icmp_id = gctx.pid;
#endif

	// If timming, to do, how to i use this in real ping
	gctx.is_timed = true; ///TODO: What to do with that
	if (gctx.is_timed)
	{
		gettimeofday(&packet_to_send.p_tp, &gctx.tz);
		gctx.tsend_date = packet_to_send.p_tp;
	}

	// skip 8 for time
	for (size_t index = 0, value = 8 ; value < gctx.msg_datalen ; index++, value++)
		packet_to_send.p_data[index] = value;

	// don't like this naming
	size_t msg_size = gctx.msg_datalen + sizeof(struct icmp); // where 8 is the sizeof struct icmp
	packet_to_send.p_icp.icmp_cksum = in_cksum(packet_to_send.raw, msg_size);

	const ssize_t bytes_sent = sendto(gctx.sockfd, &packet_to_send, msg_size, 0, 
	gctx.dest_info->ai_addr, sizeof(struct sockaddr));

	if (bytes_sent < 0)
	{
		PRINT_ERROR(INVALID_SYSCALL, "sendto");
		exit(ERR_SYSCALL); // I guess
	}
	else if (bytes_sent != msg_size)
	{
		printf("ping: wrote %s %ld chars, ret=%ld\n",
			gctx.hostaddr, msg_size, bytes_sent);
	}
}
