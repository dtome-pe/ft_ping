#ifndef FT_PING_H
# define FT_PING_H

# define EXIT_FAILURE 1
# define INET "nordlynx"

# include <sys/time.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>

# include <stdint.h>


typedef struct s_data
{   
    struct timeval timeval;
    unsigned char           *packet;
    size_t                  packet_size;
    int                     ping_fd;
    char                    ping_buffer[10000];
    size_t                  ping_interval;
    struct sockaddr_in      *from_addr;
    struct sockaddr_in      *dest_addr;
    struct icmphdr          *last_reply;

    struct iphdr            ip_hdr;
    struct icmphdr          icmp_hdr;
    char                    *payload;
    
} t_data;

void            init(t_data *data, char *dst);
void            generate_headers(t_data *data);
void            run(t_data *data);

void            error(char *str);
void            print_addr(uint32_t ip);
void            print_binary(unsigned char *addr, size_t size);
unsigned short  calculate_checksum(void *data, int length);

#endif