/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_hostaddr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/28 20:28:07 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 21:00:49 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <ping.h>
# include <parse.h>
# include <limits.h> // for HOST_NAME_MAX isn't useful on my computer
#ifdef __linux__
# include <bits/local_lim.h> // for HOST_NAME_MAX solves the problem
#elif __APPLE__
# ifndef HOST_NAME_MAX
#  define HOST_NAME_MAX 255
# endif
#endif

# include <netdb.h>

# define IS_IPV6(opts)	( \
				(!(opts & OPT_IPV4_ONLY) && !(opts & OPT_IPV6_ONLY)) \
				|| (opts & OPT_IPV6_ONLY))

const char* const get_hostaddr(
#ifdef IS_IPV6_SUPORTED
	bool* const is_ipv6
#endif
	)
{
	static char buff[HOST_NAME_MAX];

	struct sockaddr_in* sin;

#ifdef IS_IPV6_SUPORTED
	*is_ipv6 = gctx.dest_info->ai_addr->sa_family == AF_INET6
	&& IS_IPV6(gctx.const_parse->opts);

	struct sockaddr_in6* sin6;

	if (*is_ipv6)
		sin6 = (struct sockaddr_in6*)gctx.dest_info->ai_addr;
	else
#endif
		sin = (struct sockaddr_in*)gctx.dest_info->ai_addr;

	return inet_ntop(
#ifdef IS_IPV6_SUPORTED
	*is_ipv6 ? AF_INET6 :
#endif
	AF_INET,
#ifdef IS_IPV6_SUPORTED
	*is_ipv6 ? (void*)&sin6->sin6_addr :
#endif
	(void*)&sin->sin_addr, buff, ARR_SIZE(buff));
}
