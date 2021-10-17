# pragma once

# include <parse_types.h>
# include <ft_error.h>

# include <sys/time.h>
# include <sys/types.h>
# include <stdbool.h>
# include <arpa/inet.h>
# include <netdb.h>


#ifdef AF_INET6
# define IS_IPV6_SUPORTED
#endif

typedef error_code_t (*print_packet_t)(const void* const, ssize_t);
typedef void (*send_ping_t)();

typedef struct 		gcontext
{
    struct addrinfo*    dest_info;
	const char*			dest_dns;
    size_t              packet_datalen;
	int					sockfd;
	pid_t				prog_id;
	struct timezone		tz;
	struct timeval		packetsent_time;

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
		//const parse_t* const	_const_parse;
	} gparse;
	# define parse gparse._parse
	//# define const_parse gparse->_const_parse

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
# define MAX_PACKET_SIZE 4096 // can change for IPV6 !!!!!
# define PSEUDOINFINTY 99999999999
# define PERCENTAGE(x, max) ((((double)(max) - (double)(x)) * 100.0) / (double)(max))
# define DEFAULT_DATALEN (64 - 8)
# define MAXWAITTIME 10

error_code_t get_dest_info(const char* av[]
#ifdef IS_IPV6_SUPORTED
    , bool * const is_ipv6
#endif
    );

error_code_t receive_pong(uint8_t* const dest, size_t dest_len, ssize_t* const bytes_recv);

error_code_t print_packet4(const void* const packetbuff, ssize_t packet_len);
error_code_t print_packet6(const void* const packetbuff, ssize_t packet_len);

void send_ping4();
void send_ping6();
void pinger_loop();
void terminate();

/// Legacy utils
void tvsub(struct timeval* out, struct timeval* in);
unsigned short in_cksum(unsigned short *addr, int len);
