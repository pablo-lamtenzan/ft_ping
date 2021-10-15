
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
