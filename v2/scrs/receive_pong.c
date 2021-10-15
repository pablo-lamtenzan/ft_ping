
# include <ping.h>

# include <netdb.h>
# include <netinet/in.h>

error_code_t receive_pong(uint8_t* const dest, size_t dest_len, ssize_t* const bytes_recv)
{
    error_code_t st = SUCCESS;

    struct msghdr mhdr = {
        .msg_name = gctx.dest_info->ai_addr,
        .msg_namelen = sizeof(gctx.dest_info->ai_addr),
        .msg_iov = (struct iovec[]){{
            .iov_base = dest,
            .iov_len = dest_len
        }},
        .msg_iovlen = 1,
        .msg_control = NULL, // todo can put a buff[512 bytes] here to have more info
        .msg_controllen = sizeof(NULL),
        .msg_flags = 0
    };

    if ((*bytes_recv = recvmsg(gctx.sockfd, &mhdr, 0)) < 0)
    {
        // has not been interrupted
        if (errno == EINTR)
            continue ;
        st = ERR_SYSCALL;
        PRINT_ERROR(INVALID_SYSCALL, "recvmsg");
    }
    return st;
}
