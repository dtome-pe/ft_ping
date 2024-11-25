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
#include <locale.h>

#include "../inc/ft_ping.h"

int main(int argc, char **argv)
{
    t_data  data;

	parse_arg(&data, argc, argv);
	setlocale(LC_ALL, "");
	if (data.opts.help)
		print_usage();
    init(&data);
    run(&data);
    end(&data);

    return (data.code);
}