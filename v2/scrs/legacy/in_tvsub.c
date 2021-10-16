/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   in_tvsub.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 19:20:22 by pablo             #+#    #+#             */
/*   Updated: 2021/09/03 19:25:47 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 *  NOTE: This file contain legacy code from Mike Muuss's first
 *  ping implementation (from December 1983's implementation).
*/

# include <sys/time.h>

/*
 * 			T V S U B
 * 
 * Subtract 2 timeval structs:  out = out - in.
 * 
 * Out is assumed to be >= in.
 */
void tvsub(struct timeval* out, struct timeval* in)
{
	if( (out->tv_usec -= in->tv_usec) < 0 )   {
		out->tv_sec--;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
