#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "../inc/ft_ping.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        error("Wrong number of arguments\n");

    t_data  data;

    init(&data, argv[1]);
    print_addr(data.ip_header.src_ip);
    print_addr(data.ip_header.dest_ip);
    
    return 0;
}