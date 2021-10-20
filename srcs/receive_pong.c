
# include <ping.h>

# include <netinet/in.h>
# include <errno.h>

#ifdef DEBUG
# include <debug.h>
#endif

error_code_t receive_pong(uint8_t* const dest, size_t dest_len, ssize_t* const bytes_recv)
{
    error_code_t st = SUCCESS;

    struct msghdr mhdr = (struct msghdr){
        .msg_name = OPT_HAS(OPT_IPV6_ONLY) ? (void*)(struct sockaddr_in6[]){} : (void*)(struct sockaddr_in[]){},
        .msg_namelen = OPT_HAS(OPT_IPV6_ONLY) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in),
        .msg_iov = (struct iovec[]){{
            .iov_base = dest,
            .iov_len = dest_len
        }},
        .msg_iovlen = 1,
        .msg_control = (uint8_t[0X200]){},
        .msg_controllen = ARR_SIZE((uint8_t[0X200]){}),
        .msg_flags = 0
    };

    if ((*bytes_recv = recvmsg(gctx.sockfd, &mhdr, 0)) < 0)
    {
        // has not been interrupted
        if (errno == EINTR)
            st = CONTINUE;
        else
        {
            st = ERR_SYSCALL;
            PRINT_ERROR(INVALID_SYSCALL, "recvmsg");
        }
    }

#ifdef DEBUG
    print_packet_data(dest, *bytes_recv, "RECV");
#endif
    return st;
}
