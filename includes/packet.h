/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packet.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plamtenz <plamtenz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/05 16:37:48 by pablo             #+#    #+#             */
/*   Updated: 2021/10/15 20:44:06 by plamtenz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

//#define __USE_MISC // why do i need this ?

# include <ping.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
//# include <linux/icmp.h> // does not work

typedef struct  packet
{
#ifdef __linux__
    union
    {
        uint8_t raw[MAX_PACKET_SIZE];
        struct
        {
            struct icmp     icp;
            struct timeval  tp;
            uint8_t         data[MAX_PACKET_SIZE - (sizeof(struct icmp) + sizeof(struct timeval))];
        } __attribute__ ((aligned(8), packed)) packet_typed_data ; /// check if is the good alignment
        # define p_icp packet_typed_data.icp
        # define p_tp packet_typed_data.tp
        # define p_data packet_typed_data.data
    };
#elif __APPLE__
    union
    {
        uint16_t _raw[MAX_PACKET_SIZE / sizeof(uint16_t)];
        struct
        {
            struct icmp     icp;
            struct timeval  tp;
            uint8_t         data[MAX_PACKET_SIZE - (sizeof(struct icmp) + sizeof(struct timeval))];
        } /*__attribute__ ((aligned(8), packed))*/ packet_typed_data ; /// check if is the good alignment
        # define p_icp u_packet.packet_typed_data.icp
        # define p_tp u_packet.packet_typed_data.tp
        # define p_data u_packet.packet_typed_data.data
        # define raw u_packet._raw
    } u_packet;

#endif
}               packet_t;
