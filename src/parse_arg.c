#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "../inc/ft_ping.h"

struct option long_options[] = {
        {"ttl", required_argument, 0, 't'}, // Long option --ttl with required argument
        {0, 0, 0, 0}                       // End of options
    };

void	init_options(t_data *data)
{
	data->opts.help = 0;
	data->opts.verbose= 0;
	data->opts.ttl = 64;
    data->opts.w = 0;

	data->code = 0;
}

void    set_w_option(t_data *data, char *optarg)
{
    char *endptr;
    errno = 0; // Reset errno before conversion

    long long val = strtoll(optarg, &endptr, 10);

    // Check if conversion failed
    if (errno == ERANGE || val > INT_MAX) {
        fprintf(stderr, "Error: option -w value too big: %s\n", optarg);
        exit(EXIT_FAILURE);
    }
    if (val <= 0) {
        fprintf(stderr, "Error: option -w value too small\n");
        exit(EXIT_FAILURE);
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Error: invalid characters in -w option: %s\n", optarg);
        exit(EXIT_FAILURE);
    }

    data->opts.w = (int)val;

}

void    parse_arg(t_data *data, int argc, char **argv)
{
	if (argc == 1)
	{
		print_missing_host(argv[0]);
		exit(EXIT_USAGE);
	}

	init_options(data);
	
	opterr = 0;
	
	int	c;
	while ((c = getopt_long(argc, argv, "?vw:t:", long_options, NULL)) != -1)
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
                    return ;
                }
            case 't':
                if (optarg)
                {
                    int ttl = atoi(optarg);
                    if (ttl < 1)
                    {
                        fprintf(stderr, "./ft_ping: option value too small: %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                    else if (ttl > 255)
                    {
                        fprintf(stderr, "./ft_ping: option value too big: %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                    data->opts.ttl = ttl;
                    break;
                }
                else
                {
                    fprintf(stderr, "Option --ttl requires a value.\n");
                    exit(EXIT_USAGE);
                }
            case 'v':
                data->opts.verbose = 1;
                break;
            case 'w':
                set_w_option(data, optarg);
                break;
    

		}
	}
	
	// Ensure the remaining argument is the host
    if (optind < argc)
    {
        // Remaining argument is the host
        data->hostname = argv[optind];
    }
    else
    {
        print_missing_host(argv[0]);
        exit(EXIT_USAGE);
    }

	return ;
}
