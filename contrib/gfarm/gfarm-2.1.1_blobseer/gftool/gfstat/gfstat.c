/*
 * $Id: gfstat.c 3868 2007-11-21 06:02:40Z tatebe $
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>

#include <gfarm/gfarm.h>

/*
 * Display struct gfs_stat.
 */

void
display_stat(char *fn, struct gfs_stat *st)
{
	time_t clock;

	printf("  File: \"%s\"\n", fn);
	printf("  Size: %-12" GFARM_PRId64 " Filetype: ", st->st_size);
	switch (st->st_mode & GFARM_S_IFMT) {
	case GFARM_S_IFREG:
		puts("regular file");
		break;
	case GFARM_S_IFDIR:
		puts("directory");
		break;
	default:
		printf("unknown\n");
	}
	printf("  Mode: (%04o) Uid: (%8s) Gid: (%8s)\n",
	       st->st_mode & GFARM_S_ALLPERM,
	       st->st_user, st->st_group);
	printf(" Inode: %-12" GFARM_PRId64 " Gen: %-12" GFARM_PRId64
	       " Links: %-12" GFARM_PRId64 "\n",
	       st->st_ino, st->st_gen, st->st_nlink);
	printf(" Ncopy: %-12" GFARM_PRId64 "\n", st->st_ncopy);

	clock = st->st_atimespec.tv_sec; printf("Access: %s", ctime(&clock));
	clock = st->st_mtimespec.tv_sec; printf("Modify: %s", ctime(&clock));
	clock = st->st_ctimespec.tv_sec; printf("Change: %s", ctime(&clock));
}

void
usage(char *prog_name)
{
	fprintf(stderr, "Usage: %s file1 [file2 ...]\n",
		prog_name);
	exit(2);
}

/*
 *
 */

int
main(int argc, char *argv[])
{
	char *prog_name = basename(argv[0]), c;
	gfarm_error_t e;
	extern int optind;
	int r = 0;

	while ((c = getopt(argc, argv, "h?")) != -1) {
		switch (c) {
		case 'h':
		case '?':
		default:
			usage(prog_name);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage(prog_name);

	e = gfarm_initialize(&argc, &argv);
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", prog_name, gfarm_error_string(e));
		exit(1);
	}

	for (; *argv; ++argv) {
		struct gfs_stat st;

		e = gfs_stat(*argv, &st);
		if (e != GFARM_ERR_NO_ERROR) {
			fprintf(stderr, "%s: %s\n", *argv,
				gfarm_error_string(e));
			r = 1;
			continue;
		}
		display_stat(*argv, &st);

		gfs_stat_free(&st);
	}

	e = gfarm_terminate();
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", prog_name, gfarm_error_string(e));
		exit(1);
	}

	exit(r);
}
