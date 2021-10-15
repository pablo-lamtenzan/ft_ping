/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plamtenz <plamtenz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 20:12:13 by pablo             #+#    #+#             */
/*   Updated: 2021/10/15 19:47:05 by plamtenz         ###   ########.fr       */
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





// *** PARSE *** //

/// Expands to the size of any option
# define OPT_SIZE sizeof("-x")

/// Expands to the max writable 64bits integer lenght in chars
# define MAX_64BITS_CHARS 20

/// Ping command options masks
typedef enum			opts_masks
{
	/* Opts within argument */
    OPT_MARK = (1 << 0),					// '-m' option
	OPT_PRELOAD = (OPT_MARK << 1),			// '-l' option
	OPT_INTERFACE = (OPT_PRELOAD << 1),		// '-I' option
	OPT_PMTUDISC = (OPT_INTERFACE << 1),	// '-M' option
	OPT_DEADLINE = (OPT_PMTUDISC << 1),		// '-w' option
	OPT_TIMEOUT = (OPT_DEADLINE << 1),		// '-W' option
	OPT_PATTERN = (OPT_TIMEOUT << 1),		// '-p' option
	OPT_TOS = (OPT_PATTERN << 1),			// '-Q' option
	OPT_SNDBUFF = (OPT_TOS << 1),			// '-S' option
	OPT_TTL = (OPT_SNDBUFF << 1),			// '-t' option
	OPT_TIMESTAMP = (OPT_TTL << 1),			// '-T' option
	OPT_COUNT = (OPT_TIMESTAMP << 1),		// '-c' option

	/* Otps without argument */
	OPT_IPV4_ONLY = (OPT_COUNT << 1),		// '-4' option
	OPT_IPV6_ONLY = (OPT_IPV4_ONLY << 1),	// '-6' option
	OPT_VERBOSE = (OPT_IPV6_ONLY << 1),		// '-v' option
	OPT_HELP = (OPT_VERBOSE << 1),			// '-h' option
	OPT_FLOOD = (OPT_HELP << 1),			// '-f' option
	OPT_NUMERIC_OUT = (OPT_FLOOD << 1)		// '-n' option
}						opt_t;

/// pmtudisc option argument options
typedef enum 			pmtudisc_opt
{
	PMTUDISK_DO = (1 << 0),
	PMTUDISK_WANT = (PMTUDISK_DO << 1),
	PMTUDISK_DONT = (PMTUDISK_WANT << 1)
}						pmtudisc_opt_t;

/// timestamp options argument options
typedef enum			timestamp_opt
{
	TIMESTAMP_TSONLY = (1 << 0),
	TIMESTAMP_TSANDADDR = (TIMESTAMP_TSONLY << 1),
	TIMESTAMP_TSPRESPEC = (TIMESTAMP_TSANDADDR << 1)
}						timestamp_opt_t;

/// Global struct type used to store the options arguments
typedef struct			opt_args
{
	int32_t			mark;
	int32_t			preload;

	pmtudisc_opt_t	pmtudisc_opts;
	int32_t			deadline;
	uint16_t		timeout;	
	uint8_t			pattern[32 + 1]; // NOTE: Last byte is always 0, first bytes is 0 on positive, otherwise is '-'
	uint8_t			tos;
	int32_t			sndbuff;
	uint8_t			ttl;
	timestamp_opt_t	timestamp;
	uint64_t		count;
}						opts_args_t;

# define SET_OPT_ARG_MARK(value) (gctx.parse.opts_args.mark = (int32_t)value)
# define SET_OPT_ARG_PRELOAD(value) (gctx.parse.opts_args.preload = (int32_t)value)
# define LAUNCH_PRELOAD pinger(); gctx.parse.opts_args.preload--;

# define SET_OPT_ARG_PMTUDISK(opt) (gctx.parse.opts_args.pmtudisc_opts = (opt))
# define SET_OPT_ARG_DEADLINE(value) (gctx.parse.opts_args.deadline = (int32_t)(value))
# define SET_OPT_ARG_TIMEOUT(value) (gctx.parse.opts_args.timeout = (uint16_t)(value))
# define SET_OPT_ARG_PATTERN(value, neg) \
								memcpy(gctx.parse.opts_args.pattern + 1, (value), ARR_SIZE(gctx.parse.opts_args.pattern) - 1); \
								*gctx.parse.opts_args.pattern = neg ? '-' : 0;
# define USE_OPT_ARG_PATTERN (*gctx.const_parse->opts_args.pattern == '-' ? \
        gctx.const_parse->opts_args.pattern : gctx.const_parse->opts_args.pattern + 1)
# define PRINT_OPT_ARG_PATTERN(neg) (printf("PATTERN: %s0x%s\n", neg ? "-" : "", \
        gctx.const_parse->opts_args.pattern + 1))
# define SET_OPT_ARG_TOS(value) (gctx.parse.opts_args.tos = (uint8_t)(value))
# define SET_OPT_ARG_SNDBUFF(value) (gctx.parse.opts_args.sndbuff = (int32_t)(value))
# define SET_OPT_ARG_TTL(value) (gctx.parse.opts_args.ttl = (uint8_t)(value))
# define SET_OPT_ARG_TIMESTAMP(opt) (gctx.parse.opts_args.timestamp = (opt))
# define SET_OPT_ARG_COUNT(arg) (gctx.parse.opts_args.count = (arg))

/// Global struct type containing the parsing context
typedef struct			parse
{
	uint32_t			opts;
	opts_args_t         opts_args;
}						parse_t;

/// Add an option (opt_t) to the parse context
# define OPT_ADD(opt) (gctx.parse.opts |= (opt))
/// Delete an option (opt_t) from the parse context
# define OPT_DEL(opt) (gctx.parse.opts &= ~(opt))
/// Expands to non zero if the option (opt_t) is in the parse context
# define OPT_HAS(opt) (gctx.parse.opts & (opt))

typedef bool (*const fill_opt_args_t)(const char*);

error_code_t    parse_opts(const char** av[]);


// *** PING H *** //

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
        parse_t		            p_parse;
        const parse_t* const    p_const_parse;
    } parse__;
    # define parse parse__.p_parse
    # define const_parse parse__.p_const_parse

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
