/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 19:55:21 by pablo             #+#    #+#             */
/*   Updated: 2021/10/18 00:14:41 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <gcc_dependent.h>
# include <stdio.h>

typedef enum	error_code
{
	SUCCESS,
    ERR_DEST_REQ,
    ERR_INV_OPT,
    ERR_SYSCALL,
    ERR_INV_FAM,
    ERR_PRIV,
    CONTINUE
}				error_code_t;

# define PRINT_ERROR(format, args...) dprintf(2, format, args)

# define USAGE_ERR __progname ": option requires an argument -- \'%s\'"
# define USAGE_BRIEF __progname " [options] <destination>"
# define USAGE_OPTIONS "Options:\n\
\t<destination>      dns name or ip address\n\
\t-f                 flood ping\n\
\t-h                 print help and exit\n\
\t-I <interface>     either interface name or address\n\
\t-l <preload>       send <preload> number of packages while waiting replies\n\
\t-m <mark>          tag the packets going out\n\
\t-M <pmtud opt>     define mtu discovery, can be one of <do|dont|want>\n\
\t-n                 no dns name resolution\n\
\t-p <pattern>       contents of padding byte\n\
\t-Q <tclass>        use quality of service <tclass> bits\n\
\t-S <size>          use <size> as SO_SNDBUF socket option value\n\
\t-t <ttl>           define time to live\n\
\t-T <timestamp>     TO DO !!!\n\
\t-W <timeout>       time to wait for response\n\n\
IPv4 options:\n\
\t-4                 use IPv4\n\n\
IPv6 options:\n\
\t-6                 use IPv6\n\
\t-c                 stop after count ECHO_RESPONSE. If count is 0 send packets until interrupt\n"

# define USAGE_HELP USAGE_BRIEF "\n\n" USAGE_OPTIONS

# define USAGE_NO_ARG USAGE_ERR "\n\n" USAGE_HELP

# define USAGE_INV_OPT_ERR __progname ": invalid option -- \'%s\'"
# define USAGE_INV_OPT USAGE_INV_OPT_ERR USAGE_HELP

# define MSG_REQUIRED_DESTINATION __progname ": usage error: Destination address required" "\n"
# define MSG_UNKNOWN_DESTINATION __progname ": %s: Name or service not known" "\n"

# define _MSG_INV_ARG_PREFIX __progname ": invalid argument: \'%s\'"
# define MSG_INV_ARG_STR _MSG_INV_ARG_PREFIX "\n"
# define MSG_INV_ARG_RANGE_INT _MSG_INV_ARG_PREFIX ": out of range: 0 <= value <= 2147483647" "\n"
# define MSG_INV_ARG_RANGE_SHORT _MSG_INV_ARG_PREFIX ": out of range: 1 <= value <= 65536" "\n"
# define MSG_INV_ARG_LEN _MSG_INV_ARG_PREFIX ": Numerical result out of range" "\n"
# define MSG_INV_ARG_PRELOAD_SU __progname ": cannot set preload to value greater than 3: %d" "\n"
# define MSG_INV_ARG_PREFFIX2 __progname ": invalid %s argument: %s" "\n"
# define MSG_INV_ARG_BAD_TIMEOUT __progname ": bad linger time: %s" "\n"
# define MSG_INV_ARG_BAD_PATTERN __progname ": patterns must be specified as hex digits: %s" "\n"
# define MSG_INV_ARG_BAD_TOS __progname ": bad TOS value: %s" "\n"
# define MSG_INV_ARG_TOS_RANGE __progname ": the decimal value of TOS bits must be in range 0-255: %s" "\n"
# define MSG_INV_ARG_RANGE_INT2 _MSG_INV_ARG_PREFIX ": out of range: 1 <= value <= 2147483647" "\n"
# define MSG_INV_ARG_RANGE_UCHAR _MSG_INV_ARG_PREFIX ": out of range: 0 <= value <= 255" "\n"
# define MSG_INV_ARG_BAD_TIMESTAMP __progname ": invalid timestamp type: %s" "\n"
# define INVALID_SYSCALL __progname ": syscall %s failed for some unknown reason\n"
# define MSG_INV_ARG_COUNT __progname ": invalid count of packets to transmit: `%s\'" "\n"
# define MSG_INVALID_FAMILY __progname ": error: invalid family ( hint: %s )" "\n"
# define MSG_MUST_BE_ROOT __progname ": error: user msut be root" "\n"

# define ICMP_ERROR_MSG_0 "Echo Reply"
# define ICMP_ERROR_MSG_1 "None"
# define ICMP_ERROR_MSG_2 "None"
# define ICMP_ERROR_MSG_3 "Dest unreachable"
# define ICMP_ERROR_MSG_4 "Packet lost, slow down"
# define ICMP_ERROR_MSG_5 "Shorter route"
# define ICMP_ERROR_MSG_6 "None"
# define ICMP_ERROR_MSG_7 "None"
# define ICMP_ERROR_MSG_8 "Echo service"
# define ICMP_ERROR_MSG_9 "Router advertisement"
# define ICMP_ERROR_MSG_10 "Router solicitation"
# define ICMP_ERROR_MSG_11 "Time to live exceeded"
# define ICMP_ERROR_MSG_12 "IP header bad"
# define ICMP_ERROR_MSG_13 "Timestamp request"
# define ICMP_ERROR_MSG_14 "Timestamp reply"
# define ICMP_ERROR_MSG_15 "Information request"
# define ICMP_ERROR_MSG_16 "Information reply"
# define ICMP_ERROR_MSG_17 "Address mask request"
# define ICMP_ERROR_MSG_18 "Address mask reply"

# define ICMP_ERROR_INFO_MSG_0 "(Bad net)"
# define ICMP_ERROR_INFO_MSG_1 "(Bad host)"
# define ICMP_ERROR_INFO_MSG_2 "(Bad protocol)"
# define ICMP_ERROR_INFO_MSG_3 "(Bad port)"
# define ICMP_ERROR_INFO_MSG_4 "(IP_DF cause drop)"
# define ICMP_ERROR_INFO_MSG_5 "(Src routee failed)"
# define ICMP_ERROR_INFO_MSG_6 "(Unknow net)"
# define ICMP_ERROR_INFO_MSG_7 "(Unknow host)"
# define ICMP_ERROR_INFO_MSG_8 "(Src host isolated)"
# define ICMP_ERROR_INFO_MSG_9 "(Net denied)"
# define ICMP_ERROR_INFO_MSG_10 "(Host denied)"
# define ICMP_ERROR_INFO_MSG_11 "(Host denied)"
# define ICMP_ERROR_INFO_MSG_12 "(Bad tos for net)"
# define ICMP_ERROR_INFO_MSG_13 "(Bad tos for host)"
# define ICMP_ERROR_INFO_MSG_14 "(Admin prohib)"
# define ICMP_ERROR_INFO_MSG_15 "(Host prec vio.)"
# define ICMP_ERROR_INFO_MSG_16 "(Prec cutoff)"
