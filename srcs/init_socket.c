
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
	(int[]){1}, sizeof(int)) < 0)
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
