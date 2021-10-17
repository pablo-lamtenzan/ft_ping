
# pragma once

# include <inttypes.h>

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

/// struct type used to store the options arguments
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

/// struct type containing the parsing context
typedef struct			parse
{
	uint32_t			opts;
	opts_args_t         opts_args;
}						parse_t;

/// Ping command options masks
typedef enum		opts_masks
{
	/* Opts within argument */
    OPT_MARK		= (1 << 0),				// '-m' option
	OPT_PRELOAD		= (OPT_MARK << 1),		// '-l' option
	OPT_INTERFACE	= (OPT_PRELOAD << 1),	// '-I' option
	OPT_PMTUDISC	= (OPT_INTERFACE << 1),	// '-M' option
	OPT_DEADLINE	= (OPT_PMTUDISC << 1),	// '-w' option
	OPT_TIMEOUT		= (OPT_DEADLINE << 1),	// '-W' option
	OPT_PATTERN		= (OPT_TIMEOUT << 1),	// '-p' option
	OPT_TOS			= (OPT_PATTERN << 1),	// '-Q' option
	OPT_SNDBUFF		= (OPT_TOS << 1),		// '-S' option
	OPT_TTL			= (OPT_SNDBUFF << 1),	// '-t' option
	OPT_TIMESTAMP	= (OPT_TTL << 1),		// '-T' option
	OPT_COUNT		= (OPT_TIMESTAMP << 1),	// '-c' option

	/* Otps without argument */
	OPT_IPV4_ONLY	= (OPT_COUNT << 1),		// '-4' option
	OPT_IPV6_ONLY	= (OPT_IPV4_ONLY << 1),	// '-6' option
	OPT_VERBOSE		= (OPT_IPV6_ONLY << 1),	// '-v' option
	OPT_HELP		= (OPT_VERBOSE << 1),	// '-h' option
	OPT_FLOOD		= (OPT_HELP << 1),		// '-f' option
	OPT_NUMERIC_OUT = (OPT_FLOOD << 1)		// '-n' option
}					opt_t;

/// Add an option (opt_t) to the parse context
# define OPT_ADD(opt) (gctx.parse.opts |= (opt))
/// Delete an option (opt_t) from the parse context
# define OPT_DEL(opt) (gctx.parse.opts &= ~(opt))
/// Expands to non zero if the option (opt_t) is in the parse context
# define OPT_HAS(opt) (gctx.parse.opts & (opt))
