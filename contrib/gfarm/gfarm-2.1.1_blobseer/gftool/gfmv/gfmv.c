/*
 * $Id: gfmv.c 2640 2006-05-29 22:38:29Z soda $
 */

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#include <gfarm/gfarm.h>

char *program_name = "gfmv";

static void
usage(void)
{
	fprintf(stderr, "Usage: %s src dst\n", program_name);
	exit(1);
}

int
main(int argc, char **argv)
{
	gfarm_error_t e;
	int c, status = 0;
	extern int optind;

	if (argc > 0)
		program_name = basename(argv[0]);
	e = gfarm_initialize(&argc, &argv);
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(e));
		exit(1);
	}

	while ((c = getopt(argc, argv, "h?")) != -1) {
		switch (c) {
		case 'h':
		case '?':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 2)
		usage();

	e = gfs_rename(argv[0], argv[1]);
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s %s %s: %s\n",
		    program_name, argv[0], argv[1], gfarm_error_string(e));
		status = 1;
	}
	e = gfarm_terminate();
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(e));
		status = 1;
	}
	return (status);
}
