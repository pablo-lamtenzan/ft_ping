/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   resolve_dest_addr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 16:16:24 by pablo             #+#    #+#             */
/*   Updated: 2021/09/29 15:58:44 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <parse.h>
# include <ping.h>

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

/**
 * 	@brief resolves the destination address.
 * 	The destination address will be stored on 'dest_info'.
 * 	@param av A pointer to the destination. The destination can
 * 	be an IPV4/IPV6 address or a DNS address.
*/
error_code_t resolve_dest_addr(const char* av[])
{
	// TO DO: Sort by: NO_DNS, ONLY_IPV4, ONLY_IPV6, NO_RESTRINCTIONS using opts
#ifdef __linux__
	const struct addrinfo hints = {
		0,
		.ai_flags = AI_CANONNAME, // .ai_cannoname points to the official name of the host
		.ai_family = AF_UNSPEC, // return on any address family (IV4 or IP6)
		.ai_socktype = 0 // any socket type
	};
#elif __APPLE__
	struct addrinfo hints = {0};
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;
#endif

	return (getaddrinfo(*av, (void*)0, &hints, &gctx.dest_info) == 0
	? SUCCESS : ERR_DEST_REQ);
}
