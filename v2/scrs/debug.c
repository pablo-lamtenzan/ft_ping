/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 22:57:42 by pablo             #+#    #+#             */
/*   Updated: 2021/09/01 22:58:44 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef DEBUG

# include <ping.h>

# include <stdio.h>
# include <stdint.h>

void print_opts()
{
    dprintf(2, "[DEBUG] Options:\n");
    static const char* const opts[] = {
        "-m", "-l", "-I", "-M",
        "-w", "-W", "-p", "-Q",
        "-S", "-t", "-T", "-c",
        "-4", "-6", "-v", "-h",
        "-f", "-n"
    };
    for (size_t i = 0 ; i < ARR_SIZE(opts) ; i++)
    {
        if (OPT_HAS(1 << i))
            printf("\t%s\n", opts[i]);
    }

    dprintf(2, "[DEBUG] Options values:\n\t-mark: %d\n\t-preload: %d\n"
    "\t-pmtudisc: %d\n\t-deadline: %d\n\t-timeout: %d\n\t-pattern: [%s]"
    "\n\t-tos: %d\n\t-sndbuff: %d\n\t-ttl: %d\n\t-timestamp: %d\n\n",
    gctx.const_parse->opts_args.mark, gctx.const_parse->opts_args.preload,
    gctx.const_parse->opts_args.pmtudisc_opts, gctx.const_parse->opts_args.deadline,
    gctx.const_parse->opts_args.timeout, USE_OPT_ARG_PATTERN,
    gctx.const_parse->opts_args.tos, gctx.const_parse->opts_args.sndbuff,
    gctx.const_parse->opts_args.ttl, gctx.const_parse->opts_args.timestamp);
}

# endif