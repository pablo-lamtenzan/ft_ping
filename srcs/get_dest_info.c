
# include <ping.h>

# include <sys/socket.h>
# include <netdb.h>

/** NOTE: This set ipv6 by default
// # define IS_IPV6(opts)	( \
// 				(!(opts & OPT_IPV4_ONLY) && !(opts & OPT_IPV6_ONLY)) \
// 				|| (opts & OPT_IPV6_ONLY))
*/

# define IS_IPV6(opts) ( \
    opts & OPT_IPV6_ONLY)

# include <string.h> // todo remove

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

    struct addrinfo* dest = NULL;
    struct addrinfo hints = (struct addrinfo){
        .ai_flags =  AI_CANONNAME,
        .ai_family =
#ifdef IS_IPV6_SUPORTED
        *is_ipv6 ? AF_INET6 :
#endif
        AF_INET,
        .ai_socktype = SOCK_RAW,
        .ai_protocol =
#ifdef IS_IPV6_SUPORTED
        *is_ipv6 ? IPPROTO_ICMPV6 :
#endif
        IPPROTO_ICMP
    };

    if (getaddrinfo(*av, 0, &hints, &dest) != 0)
    {
        st = ERR_DEST_REQ;
        PRINT_ERROR(MSG_UNKNOWN_DESTINATION, *av);
        goto error;
    }

    /// TODO: LIBC ...
    memcpy((uint8_t*)gctx.dest_dns, dest->ai_canonname, strlen(dest->ai_canonname));

#ifdef IS_IPV6_SUPORTED
    if (*is_ipv6 && dest->ai_family != AF_INET6)
    {
        PRINT_ERROR(MSG_INVALID_FAMILY, "not IPV6");
        st = ERR_INV_FAM;
        goto error;
    }
#endif

    /// TODO: Or ipv6 version ...
    const struct in_addr* const sin_addr = &((struct sockaddr_in*)dest->ai_addr)->sin_addr; 

    gctx.dest_sockaddr = (struct sockaddr_in){
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
