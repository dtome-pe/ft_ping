#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/ft_ping.h"

int	parse_arg(t_data *data, int argc, char **argv)
{
	if (argc == 1)
	{
		print_missing_host(argv[0]);
		exit(EXIT_USAGE);
	}

	memset(&data->opts, 0, sizeof(data->opts));

	opterr = 0;

	int	c;

	while ((c = getopt (argc, argv, "?v")) != -1)
	{
		switch (c)
		{
		case '?':
			if (optopt)
			{
				print_invalid_option(argv[0], c);
				exit(EXIT_USAGE);
			}
			else
			{
				data->opts.help = 1;
				break;
			}
		case 'v':
			data->opts.verbose = 1;
			break;
		}
	}
	return (0);
}
