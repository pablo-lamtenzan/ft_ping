/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/31 20:12:13 by pablo             #+#    #+#             */
/*   Updated: 2021/09/01 23:02:50 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# define DEBUG
//# undef DEBUG /* Comment/Uncomment this line to turn on/off the debug messages*/

/// Extands to the size of the array given as argumnt
# define ARR_SIZE(array) (sizeof(array) / sizeof(*array))

/// Expands to non zero if the caller user has not root privileges
# define IS_ROOT (getuid() == 0)
