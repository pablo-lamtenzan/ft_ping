
# include <ft_error.h>
# include <ping.h>

# include <sys/socket.h>
# include <stdbool.h>

error_code_t init_socket(
#ifdef IS_IPV6_SUPORTED
    bool is_ipv6
#endif
)
{
    error_code_t st = SUCCESS;

    if ((gctx.sockfd = socket(
#ifdef IS_IPV6_SUPORTED
        is_ipv6 ? AF_INET6 :
#endif
        AF_INET, SOCK_RAW/*SOCK_DGRAM*/,
#ifdef IS_IPV6_SUPORTED
        is_ipv6 ? IPPROTO_ICMPV6 :
#endif
        IPPROTO_ICMP/*IPPROTO_IP*/)) < 0)
    {
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "socket");
        goto error;
    }

    if (setsockopt(gctx.sockfd, 
#ifdef IS_IPV6_SUPORTED
        is_ipv6 ? IPPROTO_IPV6 :
#endif
        IPPROTO_IP,
#ifdef IS_IPV6_SUPORTED
        is_ipv6 ? IPV6_HDRINCL :
#endif
        IP_HDRINCL, (int[]){1}, sizeof(int)) < 0)
	{
		st = ERR_SYSCALL;
		PRINT_ERROR(INVALID_SYSCALL, "setsocketopt");
	}

error:
    return st;
}
