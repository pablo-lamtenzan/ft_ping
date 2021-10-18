# pragma once

# include <parse_types.h>
# include <ft_error.h>

# include <sys/time.h>
# include <sys/types.h>
# include <stdbool.h>
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
# undef IS_IPV6_SUPORTED


typedef error_code_t (*print_packet_t)(const void* const, ssize_t);
typedef void (*send_ping_t)();

typedef struct 		gcontext
{
    //struct addrinfo*    dest_info;
	struct sockaddr_in	dest_sockaddr; // TODO: Also for ipv6 (can be sockaddr and cast it with macros)
	const uint8_t		dest_dns[NI_MAXHOST];
	const uint8_t		dest_ip[HOST_NAME_MAX];
    size_t              packet_datalen;
	int					sockfd;
	uint16_t			prog_id;
	struct timezone		tz;
	struct timeval		start_time;

	struct
	{
		print_packet_t 	_print_packet;
		send_ping_t		_send_ping;
	} gfamilydependent;
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
	} gpacketcount;
	# define nb_packets gpacketcount._nb_packets
	# define nb_packets_received gpacketcount._nb_packets_received
	# define nb_packets_transmited gpacketcount._nb_packets_transmited

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
# define PSEUDOINFINTY 99999999999
# define PERCENTAGE(x, max) ((((double)(max) - (double)(x)) * 100.0) / (double)(max))
# define DEFAULT_DATALEN 56
# define MAXWAITTIME 10
# define TV_TO_MS(tv) (double)((double)(tv.tv_sec) * 1000.0 + (double)(tv.tv_usec) / 1000.0)

error_code_t	get_dest_info(const char* av[]
#ifdef IS_IPV6_SUPORTED
    				, bool * const is_ipv6
#endif
    			);

error_code_t	init_socket(
#ifdef IS_IPV6_SUPORTED
    				bool is_ipv6
#endif
				);

error_code_t	receive_pong(uint8_t* const dest, size_t dest_len, ssize_t* const bytes_recv);
error_code_t	print_packet4(const void* const packetbuff, ssize_t packet_len);
void			send_ping4();
void			pinger_loop();
void			terminate();

#ifdef IS_IPV6_SUPORTED

error_code_t	print_packet6(const void* const packetbuff, ssize_t packet_len);
void			send_ping6();

#endif

/// Legacy utils
void			tvsub(struct timeval* out, struct timeval* in);
unsigned short	in_cksum(unsigned short *addr, int len);
