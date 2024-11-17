#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

void print_binary(unsigned char *addr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (addr[i] >> j) & 1);
        }
        if (i < size - 1) printf(" ");
    }
    printf("\n");
}

void get_dst_addr(char *hostname, unsigned char *dst, size_t *dst_len)
{
    struct addrinfo     *result;
    struct addrinfo     *res;
    int                 error;
    char                ip_str[INET_ADDRSTRLEN];

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo(hostname, NULL, NULL, &result);
    if (error != 0) {   
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }   
        exit(EXIT_FAILURE);
    }

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next) {   
        if (res->ai_family == AF_INET) 
        {
            struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
            memcpy(dst, &addr->sin_addr, sizeof(addr->sin_addr));
            *dst_len = sizeof(addr->sin_addr);

            // Print for verification
            printf("IPv4 address in binary: ");
            print_binary(dst, *dst_len);
            inet_ntop(AF_INET, dst, ip_str, sizeof(ip_str));
            printf("IPv4 address in string: %s\n", ip_str);
            break; // Exit after processing the first IPv4 address
        }
    }
    freeaddrinfo(result);
}

char *generate_packet(char *hostname)
{
    char            *packet;
    unsigned char   dst[4]; // IPv4 is always 4 bytes
    size_t          dst_len = 0;

    get_dst_addr(hostname, dst, &dst_len);
}

