#ifndef FT_PING_H
# define FT_PING_H

# define EXIT_FAILURE 1
# define EXIT_USAGE 64
# define INET "nordlynx"

# include <sys/time.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <stdint.h>

struct rtt_node {
    double rtt_value;         // RTT value
    struct rtt_node *next;    // Pointer to the next node
};

typedef struct s_data
{ 
	int						code;

    unsigned char           *packet;
    size_t                  packet_size;
    size_t                  payload_size;
    int                     ping_fd;
    char                    ping_buffer[10000];

    char                    *hostname;
    char                    hostname_ip_str[INET_ADDRSTRLEN];
    
    size_t                  ping_interval;
    struct sockaddr_in      *from_addr;
    struct sockaddr_in      *dest_addr;
    struct icmphdr          *last_reply;

    struct iphdr            ip_hdr;
    struct icmphdr          icmp_hdr;  

    struct timeval          last_ping;
    struct timeval          interval;

    struct stats
    {   
        int             packets_sent;
        int             packets_received;
        uint16_t        seq;
        double          min_rtt;
        double          max_rtt;
        double          total_rtt;
        struct rtt_node *rtt_list_head;
        size_t          rtt_count;
        
    } stats;

	struct opts
	{
		int				help;
		int				verbose;
		int				ttl;
	} opts;

} t_data;

void            init(t_data *data);
int				parse_arg(t_data *data, int argc, char **argv);
void            generate_headers(t_data *data);
void            run(t_data *data);
void            end(t_data *data);

void            error(char *str);
void            print_addr(uint32_t ip);
void            print_binary(unsigned char *addr, size_t size);
void			print_invalid_option(char *program, int option);
void			print_missing_host(char *program);
void			print_usage();
unsigned short  calculate_checksum(void *data, int length);
void			handle_rtt(t_data *data, double rtt_ms);
void			print_headers(struct iphdr *ip_header, struct icmphdr *icmp_header);

#endif