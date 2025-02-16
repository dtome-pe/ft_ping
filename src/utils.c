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

void add_rtt(struct stats *s, double rtt) {

    struct rtt_node *new_node = malloc(sizeof(struct rtt_node));
    if (!new_node) {
        perror("Failed to allocate memory for RTT node");
        exit(EXIT_FAILURE);
    }
    // Initialize the new node
    new_node->rtt_value = rtt;
    new_node->next = s->rtt_list_head;  // Add to the front of the list

    // Update the head of the list
    s->rtt_list_head = new_node;
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

void	print_headers(struct iphdr *ip_header, struct icmphdr *icmp_header)
{
	ip_header->tot_len = ntohs(ip_header->tot_len);

	printf("IP Hdr Dump:\n");
	unsigned char *header_bytes = (unsigned char *)ip_header;
    for (int i = 0; i < sizeof(struct iphdr); i++)
    {
        printf("%02x", header_bytes[i]);
        if ((i + 1) % 2 == 0) // Add a space every two bytes
            printf(" ");
    }
    printf("\n");

	    // Decode and print the header fields
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst\n");

	// Version (4 bits) and IHL (4 bits)
    printf(" %1u  %1u  ", ip_header->version, ip_header->ihl);

    // Type of Service (8 bits)
    printf("%02x ", ip_header->tos);

    // Total Length (16 bits)
    printf("%04x ", ntohs(ip_header->tot_len));

    // Identification (16 bits)
    printf("%04x ", ntohs(ip_header->id));

    // Flags (3 bits) and Fragment Offset (13 bits)
    unsigned short frag_offset = ntohs(ip_header->frag_off);
    unsigned short flags = (frag_offset & 0xE000) >> 13; // Extract the flags
    unsigned short offset = frag_offset & 0x1FFF;        // Extract the offset
    printf("  %1x %04x ", flags, offset);

    // Time to Live (8 bits)
    printf(" %02x  ", ip_header->ttl);

    // Protocol (8 bits)
    printf("%02x ", ip_header->protocol);

    // Header Checksum (16 bits)
    printf("%04x ", ntohs(ip_header->check));

    // Source Address (32 bits)
    struct in_addr src_addr = {ip_header->saddr};
    printf("%s ", inet_ntoa(src_addr));

    // Destination Address (32 bits)
    struct in_addr dest_addr = {ip_header->daddr};
    printf(" %s\n", inet_ntoa(dest_addr));

	unsigned char type = icmp_header->type;
    unsigned char code = icmp_header->code;
    unsigned short id = ntohs(icmp_header->un.echo.id); // Convert from network to host byte order
    unsigned short seq = ntohs(icmp_header->un.echo.sequence); // Convert from network to host byte order

    // Print ICMP details in the desired format
    printf("ICMP: type %u, code %u, size %zu, id 0x%04x, seq 0x%04x",
		type, code, sizeof(*icmp_header) * 8, id, seq);
}

const char *get_icmp_message_type(unsigned char type) 
{
    switch (type) {
        case 0:  return "Echo Reply (Ping Reply)";
        case 3:  return "Destination Unreachable";
        case 4:  return "Source Quench (Deprecated)";
        case 5:  return "Redirect (Change Route)";
        case 8:  return "Echo Request (Ping Request)";
        case 9:  return "Router Advertisement";
        case 10: return "Router Solicitation";
        case 11: return "Time Exceeded";
        case 12: return "Parameter Problem: Bad IP Header";
        case 13: return "Timestamp Request";
        case 14: return "Timestamp Reply";
        case 15: return "Information Request (Deprecated)";
        case 16: return "Information Reply (Deprecated)";
        case 17: return "Address Mask Request";
        case 18: return "Address Mask Reply";
        default: return "Unknown ICMP Message Type";
    }
}