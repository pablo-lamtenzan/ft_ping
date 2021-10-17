/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_opts.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 19:52:07 by pablo             #+#    #+#             */
/*   Updated: 2021/10/16 21:55:02 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <parse.h>
# include <ping.h>

# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h> // TO DO: remove

//*** TO DO IN ANOTHER FILE ***//

__attribute__ ((pure))
char            ft_tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        c += ('a' - 'A');
    return (c);
}

__attribute__ ((pure))
bool            is_string_digit(const char* s)
{
    if (*s == '-')
        s++;
    while (*s)
    {
        if (*s < '0' || *s > '9')
            return (false);
        s++;
    }
    return (true);
}

__attribute__ ((pure))
bool            is_string_hex(const char* s)
{
    if (*s == '-')
        s++;
    if (*s == '0' && (*(s + 1) == 'x'
    || *(s + 1) == 'X') && *(s + 2) != 0)
    {
        s += 2;
        while (*s)
        {
            if (*s < '0' || (*s > '9' && ft_tolower(*s) < 'a')
            || ft_tolower(*s) > 'f')
                goto error;
            s++;
        }
        return (true);
    }

error:
    return (false);
}

//***                       ***//

__attribute__ ((cold))
static bool     parse_mark_arg(const char* arg)
{
    const char* format_ptr;

    if (is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    if (strlen(arg) > MAX_64BITS_CHARS - 1)
    {
        format_ptr = MSG_INV_ARG_LEN;
        goto error;
    }
    int64_t value = atol(arg);
    if (value > INT32_MAX || value < 0)
    {
        format_ptr = MSG_INV_ARG_RANGE_INT;
        goto error;
    }
    SET_OPT_ARG_MARK(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_preload_arg(const char* arg)
{
    const char* format_ptr;

#ifdef __APPLE__
    if (IS_ROOT == false)
    {
        PRINT_ERROR("%s\n", "ping: -l flag: Operation not permitted");
        return (false);
    }
#endif

    if (is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    if (strlen(arg) > MAX_64BITS_CHARS - 1)
    {
        format_ptr = MSG_INV_ARG_LEN;
        goto error;
    }
    int64_t value = atol(arg);
    if (value > UINT16_MAX + 1 || value < 1)
    {
        format_ptr = MSG_INV_ARG_RANGE_SHORT;
        goto error;
    }
    if (IS_ROOT == false && value > 3)
    {
        PRINT_ERROR(MSG_INV_ARG_PRELOAD_SU, (pid_t)value);
        goto error_su;
    }
    SET_OPT_ARG_PRELOAD(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
error_su:
    return (false);
}

__attribute__ ((cold))
static bool     parse_interface_arg(const char* arg)
{
    (void)arg;
    // TO DO: When i ll understand how this works

    return (true);
}

__attribute__ ((cold))
static bool     parse_pmtudisc_arg(const char* arg)
{
    static const char* const opts[] = {
        "do", "want", "dont"
    };
    static const size_t sizes[] = {
        sizeof("do"), sizeof("want"), sizeof("dont")
    };

    for (size_t i = 0 ; i < ARR_SIZE(opts) ; i++)
    {
        if (strncmp(arg, opts[i], sizes[i]) == 0)
        {
            SET_OPT_ARG_PMTUDISK(1 << i);
            break ;
        }
    }
    if (gctx.parse.opts_args.pmtudisc_opts == 0)
    {
        PRINT_ERROR(MSG_INV_ARG_PREFFIX2, arg - 1, arg);
        return (false);
    }
    return (true);
}

__attribute__ ((cold))
static bool     parse_deadline_arg(const char* arg)
{
    const char* format_ptr;

    if (is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    if (strlen(arg) > MAX_64BITS_CHARS - 1)
    {
        format_ptr = MSG_INV_ARG_LEN;
        goto error;
    }
    int64_t value = atol(arg);
    if (value > INT32_MAX || value < 0)
    {
        format_ptr = MSG_INV_ARG_RANGE_INT;
        goto error;
    }
    SET_OPT_ARG_DEADLINE(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_timeout_arg(const char* arg)
{
    const char* format_ptr;
    const bool is_hex = is_string_hex(arg);

    if (is_hex == false && is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    int64_t value = strtol(arg, NULL, 0);
    if (strlen(arg) > 7 || value < 0)
    {
        format_ptr = MSG_INV_ARG_BAD_TIMEOUT;
        goto error;
    }
    SET_OPT_ARG_TIMEOUT(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_pattern_arg(const char* arg)
{
    if (is_string_hex(arg) == true)
    {
        const bool is_neg = (*arg == '-');
        arg += 2 + is_neg;
        SET_OPT_ARG_PATTERN(arg, is_neg);
        PRINT_OPT_ARG_PATTERN(is_neg);
        return (true);
    }
    PRINT_ERROR(MSG_INV_ARG_BAD_PATTERN, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_tos_arg(const char* arg)
{
    const char* format_ptr;
    const bool is_hex = is_string_hex(arg);

    if (is_hex == false && is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_BAD_TOS;
        goto error;
    }
    int64_t value = strtol(arg, NULL, 0);
    if (value < 0 || value > UINT8_MAX)
    {
        format_ptr = MSG_INV_ARG_TOS_RANGE;
        goto error;
    }
    SET_OPT_ARG_TOS(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_sndbuff_arg(const char* arg)
{
    const bool is_hex = is_string_hex(arg);
    const char* format_ptr;

    if (is_hex == false && is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    if (strlen(arg) > MAX_64BITS_CHARS - 1)
    {
        format_ptr = MSG_INV_ARG_LEN;
        goto error;
    }
    int64_t value = strtol(arg, NULL, 0);
    if (value > INT32_MAX || value < 1)
    {
        format_ptr = MSG_INV_ARG_RANGE_INT2;
        goto error;
    }
    SET_OPT_ARG_SNDBUFF(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_ttl_arg(const char* arg)
{
    const bool is_hex = is_string_hex(arg);
    const char* format_ptr;

    if (is_hex == false && is_string_digit(arg) == false)
    {
        format_ptr = MSG_INV_ARG_STR;
        goto error;
    }
    if (strlen(arg) > MAX_64BITS_CHARS - 1)
    {
        format_ptr = MSG_INV_ARG_LEN;
        goto error;
    }
    int64_t value = strtol(arg, NULL, 0);
    if (value > UINT8_MAX || value < 1)
    {
        format_ptr = MSG_INV_ARG_RANGE_UCHAR;
        goto error;
    }
    SET_OPT_ARG_TTL(value);
    return (true);

error:
    PRINT_ERROR(format_ptr, arg);
    return (false);
}

__attribute__ ((cold))
static bool     parse_timestamp_arg(const char* arg)
{
    static const char* const opts[] = {
        "tsonly", "tsandaddr", "tsprespec"
    };
    static const size_t sizes[] = {
        sizeof("tsonly"), sizeof("tsonly"), sizeof("tsprespec")
    };

    for (size_t i = 0 ; i < ARR_SIZE(opts) ; i++)
    {
        if (strncmp(arg, opts[i], sizes[i]) == 0)
        {
            SET_OPT_ARG_TIMESTAMP(1 << i);
            break ;
        }
    }
    if (gctx.parse.opts_args.timestamp == 0)
    {
        PRINT_ERROR(MSG_INV_ARG_BAD_TIMESTAMP, arg);
        return (false);
    }
    return (true);
}

static bool     parse_count_arg(const char* arg)
{
    if (is_string_digit(arg) == false || strlen(arg) > MAX_64BITS_CHARS - 1)
        goto error;
    int64_t value = atol(arg);
    if (value > INT32_MAX || value < 0)
        goto error;

    SET_OPT_ARG_COUNT(value);
    return true;

error:
    PRINT_ERROR(MSG_INV_ARG_COUNT, arg);
    return false;
}

static bool    opt_arg_is_present(register size_t* const av_idx, const char** av[])
{
    if ((*av)[++(*av_idx)] == NULL)
    {
        PRINT_ERROR(USAGE_NO_ARG, ++(*av)[*av_idx - 1]);
        return (false);
    }
    return (true);
}

/**
 *  @brief Parse the options.
 *  @param av A pointer to the pointer that points to the
 *  first possible main argument ( av[1] ).
 *  @return 0 on success and increment @p av address
 *  to the address that points to the destination string.
*/
error_code_t    parse_opts(const char** av[])
{
    static const char* const opts[] = {
        "-m", "-l", "-I", "-M",
        "-w", "-W", "-p", "-Q",
        "-S", "-t", "-T", "-c",
        "-4", "-6", "-v", "-h",
        "-f", "-n"
    };

    static const fill_opt_args_t fillers[] = {
        &parse_mark_arg,
        &parse_preload_arg,
        &parse_interface_arg,
        &parse_pmtudisc_arg,
        &parse_deadline_arg,
        &parse_timeout_arg,
        &parse_pattern_arg,
        &parse_tos_arg,
        &parse_sndbuff_arg,
        &parse_ttl_arg,
        &parse_timestamp_arg,
        &parse_count_arg,
    };

    size_t av_idx = 0;
    for ( ; (*av)[av_idx] && *(*av)[av_idx] == '-' ; av_idx++)
    {
        bool found = false;
        for (register size_t opts_idx = 0 ; opts_idx < ARR_SIZE(opts)
        ; opts_idx++)
        {
            if (strncmp((*av)[av_idx], opts[opts_idx], OPT_SIZE) == 0)
            {
                if (opts_idx < ARR_SIZE(fillers)
                && (opt_arg_is_present(&av_idx, av) == false
                || fillers[opts_idx]((*av)[av_idx]) == false))
                    goto invalid_opt;
                OPT_ADD(1 << opts_idx);
                found = true;
                break ;
            }
        }
        if (found == false)
        {
            PRINT_ERROR(USAGE_INV_OPT, ++(*av)[av_idx]);
            goto invalid_opt;
        }
    }
    *av += av_idx;
    return (SUCCESS);

invalid_opt:
    return (ERR_INV_OPT);
}
