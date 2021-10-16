# pragma once

# include <ping.h>

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

typedef struct	packet
{
	union
	{
		uint16_t raw[MAX_PACKET_SIZE / sizeof(uint16_t)];
		struct
		{
			struct iphdr ip;
			struct icmphdr ipc;
			struct timeval tp;
		} __attribute__ ((aligned(8), packed)) packet_data ; /// check if is the good alignment;
	} un;
	# define pk_raw un.raw
	# define pk_ip un.packet_data.ip
	# define pk_icp un.packet_data.icp
	# define pk_tp un.packet_data.tp
}				packet_t;
