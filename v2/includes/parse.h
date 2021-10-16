# pragma once

# include <ft_error.h>

# define OPT_SIZE sizeof("-x")
# define MAX_64BITS_CHARS 20


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

typedef bool (*const fill_opt_args_t)(const char*);

error_code_t    parse_opts(const char** av[]);
