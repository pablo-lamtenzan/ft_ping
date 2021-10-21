
# include <ft_error.h>
# include <ping.h>

# include <sys/socket.h>

error_code_t init_socket4()
{
	error_code_t st = SUCCESS;

	if ((gctx.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "socket");
        goto error;
	}

	if (setsockopt(gctx.sockfd, IPPROTO_IP, IP_HDRINCL,
	(int[]){1}, sizeof(int)) < 0
	||
	setsockopt(gctx.sockfd, SOL_SOCKET, SO_SNDBUF,
	(int[]){OPT_HAS(OPT_SNDBUFF) ? gctx.parse.opts_args.sndbuff : MAX_PACKET_SIZE_BITS},
	sizeof(int)) < 0
	||
	(OPT_HAS(OPT_TIMEOUT) && gctx.parse.opts_args.timeout
	&& setsockopt(gctx.sockfd, SOL_SOCKET, SO_RCVTIMEO,
	(struct timeval[]){{.tv_sec=gctx.parse.opts_args.timeout}},
	sizeof(struct timeval)) < 0))
	{
		st = ERR_SYSCALL;
		PRINT_ERROR(INVALID_SYSCALL, "setsocketopt");
	}

error:
	return st;
}

error_code_t init_socket6()
{
	error_code_t st = SUCCESS;

	if ((gctx.sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
	{
		st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "socket");
        goto error;
	}

	if (setsockopt(gctx.sockfd, IPPROTO_IPV6, IPV6_HDRINCL,
	(int[]){1}, sizeof(int)) < 0)
	{
		st = ERR_SYSCALL;
		PRINT_ERROR(INVALID_SYSCALL, "setsocketopt");
	}

error:
	return st;
}
