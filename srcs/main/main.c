/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 21:22:21 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:25:13 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <parse.h>
# include <stdlib.h>
# include <ping.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <signal.h>
# include <errno.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>


// Original ping (first implementation): https://gist.github.com/kbaribeau/4495181

# ifdef DEBUG
#include <debug.h>
# endif

# define PRINT_HELP (printf("%s", USAGE_HELP))

opts_args_t opts_args = {0};
parse_t     parse = {
	.opts = 0,
	.opts_args = &opts_args
};
const parse_t* const const_parse = (const parse_t* const)&parse;

gcontext_t  gctx = {
    0,
    .tmin = PSEUDO_INFINITY,
	.msg_datalen = 64 - 8, // why this size is used ?
    .nb_packets = 42 // must be infinity by default ?
};

int main(int ac, const char* av[])
{
    error_code_t st;

    if (ac == 1)
    {
        PRINT_ERROR("%s", MSG_REQUIRED_DESTINATION);
        st = ERR_DEST_REQ;
        goto end;
    }
    ++av;
    if ((st = parse_opts(&av)) != SUCCESS)
        goto end;

# ifdef DEBUG
    print_opts();
# endif

    if (OPT_HAS(OPT_HELP))
    {
        PRINT_HELP;
        st = ERR_INV_OPT;
        goto end;
    }

    if ((st = resolve_dest_addr(av) != SUCCESS))
        goto end;
#ifdef IS_IPV6_SUPORTED
	bool is_ipv6;
#endif
	const char* const hostaddr = get_hostaddr(
#ifdef IS_IPV6_SUPORTED
		&is_ipv6
#endif
	);

	if (hostaddr == NULL)
	{
		st = ERR_SYSCALL;
		PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
		goto end;
	}

	if (
#ifdef IS_IPV6_SUPORTED
		!is_ipv6 &&
#endif
		gctx.dest_info->ai_addr->sa_family != AF_INET)
	{
		// TO DO: Unknow family
		goto end;
	}

    // Pid lenght cannot be greather than 16bits
    gctx.pid = getpid() & 0xffff;

    if ((gctx.sockfd = socket(
# ifdef IS_IPV6_SUPORTED
		is_ipv6 ? PF_INET6 :
# endif
		PF_INET, /*SOCK_RAW*/ SOCK_DGRAM, IPPROTO_ICMP)) < 0)
    {
		st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "socket");
        goto end;
    }

/*
	if (setsockopt(gctx.sockfd, IPPROTO_IP, IP_HDRINCL, (int[]){1}, sizeof(int)) < 0)
	{
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "setsocketopt");
        goto end;
	}
*/
    PRINT_HEADER(hostaddr);

    signal(SIGALRM, pinger_loop);
    signal(SIGINT, terminate);

    while (const_parse->opts_args->preload)
        LAUNCH_PRELOAD;

    pinger_loop();

	static uint8_t packet[MAX_PACKET_SIZE];

    for ( ; ; )
    {
        ssize_t bytes_recv;

		struct iovec iov = {
			.iov_base = packet,
			.iov_len = ARR_SIZE(packet)	
		};
		struct msghdr mhdr = {
			.msg_name = gctx.dest_info->ai_addr,
			.msg_namelen = sizeof(gctx.dest_info->ai_addr),
			.msg_iov = &iov,
			.msg_iovlen = 1,
			.msg_control = NULL, // todo can put a buff[512 bytes] here to have more info
			.msg_controllen = sizeof(NULL),
			.msg_flags = 0
		};

        if ((bytes_recv = recvmsg(gctx.sockfd, &mhdr, 0)) < 0) 
        {
            // has not been interrupted
            if (errno == EINTR)
                continue ;
            st = ERR_SYSCALL;
            PRINT_ERROR(INVALID_SYSCALL, "recvmsg");
            goto end;
        }
        print_packet(packet, bytes_recv,
#ifdef IS_IPV6_SUPORTED
        is_ipv6 ? AF_INET6 :
#endif
        AF_INET, hostaddr);
        if (gctx.nb_packets && gctx.nb_packets_received - gctx.nb_packets == 0)
            terminate();
    }

end:
    return (st);
}
