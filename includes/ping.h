# pragma once

# include <parse_types.h>
# include <ft_error.h>

# include <sys/time.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <bits/local_lim.h>


# define DEBUG
# undef DEBUG

#ifdef DEBUG
# include <debug.h>
#endif

#ifdef AF_INET6
# define IS_IPV6_SUPORTED
#endif

typedef error_code_t (*get_dest_info_t)(const char* av[]);
typedef error_code_t (*init_socket_t)();
typedef error_code_t (*print_packet_t)(const void* const, ssize_t);
typedef void (*send_ping_t)();

typedef struct 		gcontext
{
	struct sockaddr		dest_sockaddr;
	const uint8_t		dest_dns[NI_MAXHOST];
	const uint8_t		dest_ip[HOST_NAME_MAX];
    size_t              packet_payloadlen;
	int					sockfd;
	uint16_t			prog_id;
	struct timeval		start_time;
	struct timeval		deadline_time;
	struct timeval		aux_pktime;

	struct
	{
		get_dest_info_t	_get_dest_info;
		init_socket_t	_init_socket;
		print_packet_t 	_print_packet;
		send_ping_t		_send_ping;
	} gfamilydependent;
	# define get_dest_info gfamilydependent._get_dest_info
	# define init_socket gfamilydependent._init_socket
	# define print_packet gfamilydependent._print_packet
	# define send_ping gfamilydependent._send_ping

	struct
	{
		parse_t 				_parse;
	} gparse;
	# define parse gparse._parse

	struct
	{
		uint64_t _nb_packets;
		uint64_t _nb_packets_received;
		uint64_t _nb_packets_transmited;
		uint64_t _nb_packets_error;
	} gpacketcount;
	# define nb_packets gpacketcount._nb_packets
	# define nb_packets_received gpacketcount._nb_packets_received
	# define nb_packets_transmited gpacketcount._nb_packets_transmited
	# define nb_packets_error gpacketcount._nb_packets_error

	struct
	{
		double	_tmax;
		double	_tmin;
		double	_tsum;
	} gtiming;
	# define tmax gtiming._tmax
	# define tmin gtiming._tmin
	# define tsum gtiming._tsum
}               	gcontext_t;

extern gcontext_t	gctx;

# define ARR_SIZE(array) (sizeof(array) / sizeof(*array))
# define IS_ROOT (getuid() == 0)
# define MAX_PACKET_SIZE 4096
# define MAX_PACKET_SIZE_BITS (MAX_PACKET_SIZE * 8)
# define PSEUDOINFINTY 99999999999
# define PERCENTAGE(x, max) ((((double)(max) - (double)(x)) * 100.0) / (double)(max))
# define DEFAULT_PAYLOADLEN 56
# define MAXWAITTIME 10
# define TV_TO_MS(tv) (double)((double)(tv.tv_sec) * 1000.0 + (double)(tv.tv_usec) / 1000.0)
# define DEFAULT_INTERVAL 1
# define PAYLOAD_BYTE ((uint8_t)(4 | ((2 << 4) & 0XF0)))

error_code_t	gest_dest_info4(const char* av[]);
error_code_t	init_socket4();
error_code_t	receive_pong(uint8_t* const dest, size_t dest_len, ssize_t* const bytes_recv);
error_code_t	print_packet4(const void* const packetbuff, ssize_t packet_len);
void			send_ping4();
void			pinger_loop();
void			terminate();

#ifdef IS_IPV6_SUPORTED

error_code_t	get_dest_info6(const char* av[]);
error_code_t	init_socket6();
error_code_t	print_packet6(const void* const packetbuff, ssize_t packet_len);
void			send_ping6();

#endif

/// Legacy utils
void			tvsub(struct timeval* out, struct timeval* in);
unsigned short	in_cksum(unsigned short *addr, int len);
