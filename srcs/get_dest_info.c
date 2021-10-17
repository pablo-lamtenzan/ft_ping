
# include <ping.h>

# include <sys/socket.h>
# include <netdb.h>

#ifdef __linux__
# include <bits/local_lim.h> // for HOST_NAME_MAX solves the problem
#elif __APPLE__
# ifndef HOST_NAME_MAX
#  define HOST_NAME_MAX 255
# endif
#endif

# define IS_IPV6(opts)	( \
				(!(opts & OPT_IPV4_ONLY) && !(opts & OPT_IPV6_ONLY)) \
				|| (opts & OPT_IPV6_ONLY))


__attribute__ ((always_inline))
static inline const char* get_dest_dns(
#ifdef IS_IPV6_SUPORTED
    bool is_ipv6
#endif
)
{
    static char buff[HOST_NAME_MAX];

    struct sockaddr_in* sin;

#ifdef IS_IPV6_SUPORTED

    struct sockaddr_in6* sin6;

    if (is_ipv6)
        sin6 = (struct sockaddr_in6*)gctx.dest_info->ai_addr;
	else
        sin = (struct sockaddr_in*)gctx.dest_info->ai_addr;
#endif

    return inet_ntop(
#ifdef IS_IPV6_SUPORTED
	is_ipv6 ? AF_INET6 :
#endif
	AF_INET,
#ifdef IS_IPV6_SUPORTED
	is_ipv6 ? (void*)&sin6->sin6_addr :
#endif
	(void*)&sin->sin_addr, buff, ARR_SIZE(buff));
}

error_code_t get_dest_info(const char* av[]
#ifdef IS_IPV6_SUPORTED
    , bool * const is_ipv6
#endif
    )
{
    error_code_t st = SUCCESS;

# ifdef IS_IPV6_SUPORTED
    *is_ipv6 = IS_IPV6(gctx.parse.opts);
# endif

    struct addrinfo hints = (struct addrinfo){
        .ai_flags =  AI_CANONNAME, // .ai_cannoname points to the official name of the host
        .ai_family =
#ifdef IS_IPV6_SUPORTED
        *is_ipv6 ? AF_INET6 :
#endif
        AF_INET,
        .ai_socktype = SOCK_RAW,
        .ai_protocol = IPPROTO_ICMP
    };

    if (getaddrinfo(*av, (void*)0, &hints, &gctx.dest_info) != 0)
    {
        st = ERR_DEST_REQ;
        goto error;
    }

    if (*is_ipv6 && gctx.dest_info->ai_addr->sa_family != AF_INET6)
    {
        PRINT_ERROR(MSG_INVALID_FAMILY, "not IPV6");
        st = ERR_INV_FAM;
        goto error;
    }

    if ((gctx.dest_dns = get_dest_dns(
#ifdef IS_IPV6_SUPORTED
        *is_ipv6
#endif
        )) == 0)
    {
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "inet_ntop");
    }

error:
    return st;
}
