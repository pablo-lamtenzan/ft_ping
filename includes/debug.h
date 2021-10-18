/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 22:56:24 by pablo             #+#    #+#             */
/*   Updated: 2021/10/18 22:41:46 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <ping.h>

# ifdef DEBUG

void print_opts();
void print_packet_data(uint8_t* packet, ssize_t packet_size, const char* use);
void print_packet_bytes(const void* packet, ssize_t bytes_lenght);


# endif
