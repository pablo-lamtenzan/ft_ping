# pragma once

# include <ft_error.h>

# include <stdbool.h>

# define OPT_SIZE sizeof("-x")

# define MAX_64BITS_CHARS 20

# define SET_OPT_ARG_MARK(value) (gctx.parse.opts_args.mark = (int32_t)value)
# define SET_OPT_ARG_PRELOAD(value) (gctx.parse.opts_args.preload = (int32_t)value)
# define LAUNCH_PRELOAD pinger(); gctx.parse.opts_args.preload--;

# define SET_OPT_ARG_PMTUDISK(opt) (gctx.parse.opts_args.pmtudisc_opts = (opt))
# define SET_OPT_ARG_DEADLINE(value) (gctx.parse.opts_args.deadline = (int32_t)(value))
# define SET_OPT_ARG_TIMEOUT(value) (gctx.parse.opts_args.timeout = (uint16_t)(value))
# define SET_OPT_ARG_PATTERN(value, neg) \
								ft_memcpy(gctx.parse.opts_args.pattern + 1, (value), ARR_SIZE(gctx.parse.opts_args.pattern) - 1); \
								*gctx.parse.opts_args.pattern = neg ? '-' : 0;
# define USE_OPT_ARG_PATTERN (*gctx.parse.opts_args.pattern == '-' ? \
        gctx.parse.opts_args.pattern : gctx.parse.opts_args.pattern + 1)
# define PRINT_OPT_ARG_PATTERN(neg) (printf("PATTERN: %s0x%s\n", neg ? "-" : "", \
        gctx.parse.opts_args.pattern + 1))
# define SET_OPT_ARG_TOS(value) (gctx.parse.opts_args.tos = (uint8_t)(value))
# define SET_OPT_ARG_SNDBUFF(value) (gctx.parse.opts_args.sndbuff = (int32_t)(value))
# define SET_OPT_ARG_TTL(value) (gctx.parse.opts_args.ttl = (uint8_t)(value))
# define SET_OPT_ARG_TIMESTAMP(opt) (gctx.parse.opts_args.timestamp = (opt))
# define SET_OPT_ARG_COUNT(arg) (gctx.parse.opts_args.count = (arg))

typedef bool (*const fill_opt_args_t)(const char*);

error_code_t    parse_opts(const char** av[]);
