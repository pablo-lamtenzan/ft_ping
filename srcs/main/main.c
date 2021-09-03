/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 21:22:21 by pablo             #+#    #+#             */
/*   Updated: 2021/09/01 23:13:44 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <parse.h>
# include <stdlib.h>
# include <ping.h>
# include <stdio.h>

# ifdef DEBUG
#include <debug.h>
# endif

# define PRINT_HELP (printf("%s", USAGE_HELP))

opts_args_t opts_args = {0};
parse_t parse = {
	.opts = 0,
	.opts_args = &opts_args
};
const parse_t* const const_parse = (const parse_t* const)&parse;

int main(int ac, const char* av[])
{
    error_code_t st;

    if (ac == 1)
    {
        SET_ERRNO("%s", MSG_REQUIRED_DESTINATION);
        st = ERR_DEST_REQ;
        goto end;
    }
    ++av;
    if ((st = parse_opts(&av)) != SUCCESS)
        goto end;

# ifdef DEBUG
    print_opts();
# endif

    if (OPT_HAS(OPT_HELP))
    {
        PRINT_HELP;
        st = ERR_INV_OPT;
        goto end;
    }

end:
    return (st);
}
