/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packet.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/05 16:37:48 by pablo             #+#    #+#             */
/*   Updated: 2021/10/01 22:50:57 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

//#define __USE_MISC // why do i need this ?

# include <ping.h>
# include <netinet/ip_icmp.h>
//# include <linux/icmp.h> // does not work

typedef struct  packet
{
    union
    {
        uint8_t raw[MAX_PACKET_SIZE];
        struct
        {
            struct icmp     icp;
            struct timeval  tp;
            uint8_t         data[MAX_PACKET_SIZE - (sizeof(struct icmp) + sizeof(struct timeval))];
        } packet_typed_data __attribute__ ((aligned(8), packed)); /// check if is the good alignment
        # define p_icp packet_typed_data.icp
        # define p_tp packet_typed_data.tp
        # define p_data packet_typed_data.data
    };
}               packet_t;
