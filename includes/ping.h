/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 20:12:13 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:42:36 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#ifdef __linux__
# ifndef __USE_XOPEN2K
//#  define __USE_XOPEN2K
# endif
//# include <linux/time.h>
#else
//# include <sys/time.h>
#endif

# include <sys/time.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <stdbool.h>
# include <ft_error.h>

#ifdef AF_INET6
# define IS_IPV6_SUPORTED
#endif

# define DEBUG
# undef DEBUG /* Comment/Uncomment this line to turn on/off the debug messages*/

/// Extands to the size of the array given as argumnt
# define ARR_SIZE(array) (sizeof(array) / sizeof(*array))

/// Expands to non zero if the caller user has not root privileges
# define IS_ROOT (getuid() == 0)

# define MAX_PACKET_SIZE 4096

# define PERCENTAGE(x, max) ((((double)(max) - (double)(x)) * 100.0) / (double)(max))

# define PSEUDO_INFINITY 999999999

# define MAXWAITTIME 10

typedef struct          gcontext
{
    struct addrinfo*    dest_info;
    size_t              msg_datalen;
    int                 sockfd;
    pid_t               pid;
    struct timezone     tz;
    struct timeval      tsend_date;
    const char*         hostaddr;

    struct
    {
        uint64_t		pc_nb_packets;
        uint64_t		pc_nb_packets_received;
        uint64_t		pc_nb_packets_transmited;
    } context_packet_count;
    # define nb_packets context_packet_count.pc_nb_packets
    # define nb_packets_received context_packet_count.pc_nb_packets_received
    # define nb_packets_transmited context_packet_count.pc_nb_packets_transmited

    struct
    {
        bool            t_is_timed;
        double          t_tmax;
        double          t_tmin;
        double          t_tsum;
        double          stddevsum;
    } context_timing;
    # define is_timed context_timing.t_is_timed
	# define tmax context_timing.t_tmax
	# define tmin context_timing.t_tmin
	# define tsum context_timing.t_tsum
    # define stddevsum context_timing.stddevsum
}                       gcontext_t;

extern gcontext_t		gctx;

# define PRINT_HEADER(hostaddr) (printf("PING %s (%s): %ld data bytes\n", \
	gctx.dest_info->ai_canonname ? gctx.dest_info->ai_canonname : hostaddr, \
	hostaddr, gctx.msg_datalen))

error_code_t	resolve_dest_addr(const char* av[]);

const char* const get_hostaddr(
#ifdef IS_IPV6_SUPORTED
	bool* const is_ipv6
#endif
);

void    print_packet(const void* const packet, ssize_t packet_len, uint8_t family);

/// Sig handlers
void			pinger_loop();
void			terminate();
void            info();

void			pinger();

/// Legacy utils
void tvsub(struct timeval* out, struct timeval* in);
unsigned short in_cksum(unsigned short *addr, int len);
