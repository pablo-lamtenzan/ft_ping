
# include <ping.h>
# include <ftlibc.h>

# include <sys/socket.h>
# include <sys/errno.h>
# include <string.h>

error_code_t gest_dest_info4(const char* av[])
{
	error_code_t st = SUCCESS;

	struct addrinfo* dest = NULL;
	struct addrinfo hints = (struct addrinfo){
		.ai_flags = AI_CANONNAME,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMP
	};

	if (getaddrinfo(*av, 0, &hints, &dest) != 0)
    {
        st = ERR_DEST_REQ;
        PRINT_ERROR(MSG_UNKNOWN_DESTINATION, *av);
        goto error;
    }

    ft_memcpy((uint8_t*)gctx.dest_dns, dest->ai_canonname, ft_strlen(dest->ai_canonname));

	if (dest->ai_family != AF_INET)
    {
        PRINT_ERROR(MSG_INVALID_FAMILY, "not IPV4");
        st = ERR_INV_FAM;
        goto error;
    }

	const struct in_addr* const sin_addr = &((struct sockaddr_in*)dest->ai_addr)->sin_addr; 

	*(struct sockaddr_in*)&gctx.dest_sockaddr = (struct sockaddr_in){
        .sin_addr.s_addr = sin_addr->s_addr,
        .sin_family = AF_INET,
    };

	if (inet_ntop(dest->ai_family, (const void*)sin_addr,
    (char*)gctx.dest_ip, ARR_SIZE(gctx.dest_ip)) == 0)
    {
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
    }

error:
    freeaddrinfo(dest);
    return st;
}

#ifdef IS_IPV6_SUPORTED

error_code_t get_dest_info6(const char* av[])
{
    error_code_t st = SUCCESS;

    struct addrinfo*	dest = NULL;
    struct addrinfo		hints = (struct addrinfo){
		.ai_flags = AI_CANONNAME,
		.ai_family = AF_INET6,
		.ai_socktype = SOCK_RAW,
		.ai_protocol = IPPROTO_ICMPV6
	};

	if (getaddrinfo(*av, 0, &hints, &dest) != 0)
    {
        st = ERR_DEST_REQ;
        PRINT_ERROR("[IPv6] " MSG_UNKNOWN_DESTINATION, *av);
        goto error;
    }

	if (dest->ai_canonname)
		ft_memcpy((uint8_t*)gctx.dest_dns, dest->ai_canonname, ft_strlen(dest->ai_canonname));

	if (dest->ai_family != AF_INET6)
    {
        PRINT_ERROR(MSG_INVALID_FAMILY, "not IPV6");
        st = ERR_INV_FAM;
        goto error;
    }

	const struct in6_addr* const sin6_addr = &((struct sockaddr_in6*)dest->ai_addr)->sin6_addr;

	*(struct sockaddr_in6*)&gctx.dest_sockaddr = (struct sockaddr_in6){
		.sin6_addr = *sin6_addr,
		.sin6_family = AF_INET6
	};

	if (inet_ntop(AF_INET6, (const void*)sin6_addr,
	(char*)gctx.dest_ip, ARR_SIZE(gctx.dest_ip)) == 0)
	{
		st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
	}

error:
    freeaddrinfo(dest);
    return st;
}

#endif
