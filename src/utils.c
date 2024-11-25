#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../inc/ft_ping.h"

void    error(char *str)
{
    write(2, str, strlen(str));
    exit(1);
}

void print_binary(unsigned char *addr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (addr[i] >> j) & 1);
        }
        if (i < size - 1) printf(" ");
    }
    printf("\n");
}

void print_addr(uint32_t ip) {
    // Print the binary representation of the IP address
    unsigned char *addr = (unsigned char *)&ip;
    for (size_t i = 0; i < sizeof(ip); i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (addr[i] >> j) & 1);
        }
        if (i < sizeof(ip) - 1) printf(" ");
    }
    
    printf("\n");

    // Convert uint32_t to struct in_addr for human-readable format
    struct in_addr ip_addr;
    ip_addr.s_addr = htonl(ip); // Ensure the IP is in network byte order

    // Print the IP address in string format
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_addr, ip_str, sizeof(ip_str));

    printf("Human-readable IP: %s\n", ip_str);
}

// Function to print a 16-bit word in binary, hexadecimal, and decimal
void print_16bit_word(uint16_t word, char *name) {
    // Print the word in binary
    printf("%s\n", name);

    printf("Binary: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (word >> i) & 1);
    }

    // Print the word in hexadecimal
    printf(" | Hex: 0x%04X", word);

    // Print the word in decimal
    printf(" | Decimal: %d\n", word);
}

// Generic checksum calculation function
unsigned short calculate_checksum(void *data, int length)
{
    uint32_t sum = 0;
    unsigned short *ptr = (unsigned short *)data;

    // Sum all 16-bit words in the data
    for (int i = 0; i < length / 2; i++) {
        sum += ptr[i];
    }

    // If the length is odd, add the last byte (if present)
    if (length % 2 == 1) {
        sum += *((unsigned char *)data + length - 1);
    }

    // Handle carry
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Return one's complement of the sum
    return (unsigned short)(~sum);
}

void	print_usage()
{
	printf("Usage: ping [OPTION...] HOST ...\n"
	"Send ICMP ECHO_REQUEST packets to network hosts\n\n"
	" Options valid for all request types:\n\n"
	"  -v			      verbose output\n\n"
	" Options valid for --echo requests:\n\n"
	"  -?			      give this help list\n\n"
	"Mandatory or optional arguments to long options are also mandatory or optional\n"
	"for any corresponding short options.\n\n"
	"Options marked with (root only) are available only to superuser.\n\n"
	"Report bugs to <bug-inetutils@gnu.org>.\n");
	exit(0);
}

void	print_invalid_option(char *program, int option)
{
	fprintf (stderr, ("%s: invalid option -- '%c'\n"
			"Try 'ft_ping -?' for more information.\n"), program, option);
}

void	print_missing_host(char *program)
{
	fprintf (stderr, ("%s: missing host operand\n"
			"Try 'ft_ping -?' for more information.\n"), program);
}

void	handle_rtt(t_data *data, double rtt_ms)
{
	data->stats.total_rtt += rtt_ms;
    if (data->stats.min_rtt == 0 || rtt_ms < data->stats.min_rtt)
        data->stats.min_rtt = rtt_ms;
    if (data->stats.max_rtt == 0 || rtt_ms > data->stats.max_rtt)
        data->stats.max_rtt = rtt_ms;

    add_rtt(&data->stats, rtt_ms);
    data->stats.rtt_count++;
}