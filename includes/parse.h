/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 19:18:32 by pablo             #+#    #+#             */
/*   Updated: 2021/09/01 22:40:29 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once


# include <inttypes.h>
# include <stdbool.h>

# include <ft_error.h>

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

	/* Otps without argument */
	OPT_IPV4_ONLY = (OPT_TIMESTAMP << 1),	// '-4' option
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
}						opts_args_t;

# define SET_OPT_ARG_MARK(value) (parse.opts_args->mark = (int32_t)value)
# define SET_OPT_ARG_PRELOAD(value) (parse.opts_args->preload = (int32_t)value)

# define SET_OPT_ARG_PMTUDISK(opt) (parse.opts_args->pmtudisc_opts = (opt))
# define SET_OPT_ARG_DEADLINE(value) (parse.opts_args->deadline = (int32_t)(value))
# define SET_OPT_ARG_TIMEOUT(value) (parse.opts_args->timeout = (uint16_t)(value))
# define SET_OPT_ARG_PATTERN(value, neg) \
								memcpy(parse.opts_args->pattern + 1, (value), ARR_SIZE(parse.opts_args->pattern) - 1); \
								*parse.opts_args->pattern = neg ? '-' : 0;
# define USE_OPT_ARG_PATTERN (*const_parse->opts_args->pattern == '-' ? \
        const_parse->opts_args->pattern : const_parse->opts_args->pattern + 1)
# define PRINT_OPT_ARG_PATTERN(neg) (printf("PATTERN: %s0x%s\n", neg ? "-" : "", \
        const_parse->opts_args->pattern + 1))
# define SET_OPT_ARG_TOS(value) (parse.opts_args->tos = (uint8_t)(value))
# define SET_OPT_ARG_SNDBUFF(value) (parse.opts_args->sndbuff = (int32_t)(value))
# define SET_OPT_ARG_TTL(value) (parse.opts_args->ttl = (uint8_t)(value))
# define SET_OPT_ARG_TIMESTAMP(opt) (parse.opts_args->timestamp = (opt))

/// Global struct type containing the parsing context
typedef struct			parse
{
	uint32_t			opts;
	opts_args_t* const	opts_args;
}						parse_t;

/// Global instance of opts_args_t
extern opts_args_t	opts_args;

/// Global instance of parse_t
extern parse_t		parse; 

/// Global readonly parse reference
extern const parse_t* const const_parse;

/// Add an option (opt_t) to the parse context
# define OPT_ADD(opt) (parse.opts |= (opt))
/// Delete an option (opt_t) from the parse context
# define OPT_DEL(opt) (parse.opts &= ~(opt))
/// Expands to non zero if the option (opt_t) is in the parse context
# define OPT_HAS(opt) (parse.opts & (opt))

typedef bool (*const fill_opt_args_t)(const char*);

error_code_t    parse_opts(const char** av[]);
