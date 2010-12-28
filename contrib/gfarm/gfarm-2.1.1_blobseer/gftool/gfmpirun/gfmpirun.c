/*
 * $Id: gfmpirun.c 3617 2007-03-12 08:32:55Z n-soda $
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <libgen.h>
#include <gfarm/gfarm.h>
#include "host.h"
#include "config.h"
#include "gfj_client.h"

char *program_name = "gfmpirun_p4";

void
setsig(int signum, void (*handler)(int))
{
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	/* do not set SA_RESTART to make interrupt at waitpid(2) */
	act.sa_flags = 0;
	if (sigaction(signum, &act, NULL) == -1) {
		fprintf(stderr, "%s: sigaction(%d): %s\n",
			program_name, signum, strerror(errno));
		exit(1);
	}
}

void
ignore_handler(int signum)
{
	/* do nothing */
}

void
sig_ignore(int signum)
{
	/* we don't use SIG_IGN to make it possible that child catch singals */
	setsig(signum, ignore_handler);
}

void
usage()
{
	fprintf(stderr,
		"Usage: %s [-G <Gfarm file>|-N <# nodes>|-H <hostfile>]\n",
		program_name);
	fprintf(stderr, "\t[<mpirun_options>] command...\n");
	exit(1);
}

int
main(argc, argv)
	int argc;
	char **argv;
{
	gfarm_stringlist input_list, output_list, arg_list, option_list;
	int command_index;
	int status;
	int i, nhosts, job_id, nfrags, save_errno;
	char *e, **hosts;
	static char template[] = "/tmp/mpXXXXXX";
	char filename[sizeof(template)];
	FILE *fp;
	char total_nodes[GFARM_INT32STRLEN];

	int nprocs = -1, sched_nopt = 0, spooled_command = 0;
	char *hostfile = NULL, *scheduling_file = NULL;
	char *command_name, **delivered_paths = NULL;

	if (argc >= 1)
		program_name = basename(argv[0]);

	e = gfarm_initialize(&argc, &argv);
	if (e != NULL) {
		fprintf(stderr, "%s: gfarm initialize: %s\n", program_name, e);
		exit(1);
	}
	e = gfj_initialize();
	if (e != NULL) {
		fprintf(stderr, "%s: job manager: %s\n", program_name, e);
		exit(1);
	}
	gfarm_stringlist_init(&option_list);

	/*
	 * parse and skip/record options
	 */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-')
			break;
		switch (argv[i][1]) {
		case 'G':
			if (argv[i][2] != '\0') {
				scheduling_file = &argv[i][2];
			} else if (++i < argc) {
				scheduling_file = argv[i];
			} else {
				fprintf(stderr, "%s: "
					"missing argument to %s\n",
					program_name, argv[i - 1]);
				usage();
			}
			++sched_nopt;
			goto skip_opt;
		case 'N':
			if (argv[i][2] != '\0') {
				nprocs = atoi(&argv[i][2]);
			} else if (++i < argc) {
				nprocs = atoi(argv[i]);
			} else {
				fprintf(stderr, "%s: "
					"missing argument to %s\n",
					program_name, argv[i - 1]);
				usage();
			}
			++sched_nopt;
			goto skip_opt;
		case 'H':
			if (argv[i][2] != '\0') {
				hostfile = &argv[i][2];
			} else if (++i < argc) {
				hostfile = argv[i];
			} else {
				fprintf(stderr, "%s: "
					"missing argument to %s\n",
					program_name, argv[i - 1]);
				usage();
			}
			++sched_nopt;
			goto skip_opt;
		}
		if (strcmp(argv[i], "-arch") == 0 ||
		    strcmp(argv[i], "-np") == 0 ||
		    strcmp(argv[i], "-stdin") == 0 ||
		    strcmp(argv[i], "-stdout") == 0 ||
		    strcmp(argv[i], "-stderr") == 0 ||
		    strcmp(argv[i], "-nexuspg") == 0 ||
		    strcmp(argv[i], "-nexusdb") == 0 ||
		    strcmp(argv[i], "-p4pg") == 0 ||
		    strcmp(argv[i], "-tcppg") == 0 ||
		    strcmp(argv[i], "-p4ssport") == 0 ||
		    strcmp(argv[i], "-mvback") == 0 ||
		    strcmp(argv[i], "-maxtime") == 0 ||
		    strcmp(argv[i], "-mem") == 0 ||
		    strcmp(argv[i], "-cpu") == 0) {
			/* an option which does have an argument */
			if (++i >= argc) {
				fprintf(stderr, "%s: "
					"missing argument to %s\n",
					program_name, argv[i - 1]);
				usage();
			}
			gfarm_stringlist_add(&option_list, argv[i - 1]);
		}
		gfarm_stringlist_add(&option_list, argv[i]);
skip_opt: ;
	}
	if (sched_nopt > 1)
		usage();

	command_index = i;
	if (command_index >= argc) /* no command name */
		usage();
	command_name = argv[command_index];
	spooled_command = gfarm_is_url(command_name);

	gfarm_stringlist_init(&input_list);
	gfarm_stringlist_init(&output_list);
	for (i = command_index + 1; i < argc; i++) {
		if (gfarm_is_url(argv[i])) {
			e = gfarm_url_fragment_number(argv[i], &nfrags);
			if (e == NULL) {
				gfarm_stringlist_add(&input_list, argv[i]);
			} else {
				gfarm_stringlist_add(&output_list, argv[i]);
			}
		}
	}

	/* schedule nodes */
	if (hostfile != NULL) {
		int error_line;

		e = gfarm_hostlist_read(hostfile,
					&nhosts, &hosts, &error_line);
		if (e != NULL) {
			if (error_line != -1)
				fprintf(stderr, "%s: %s: line %d: %s\n",
					program_name, hostfile, error_line, e);
			else
				fprintf(stderr, "%s: %s: %s\n",
					program_name, hostfile, e);
			exit(1);
		}
		scheduling_file = hostfile;
	}
	else if (nprocs > 0) {
		nhosts = nprocs;
		GFARM_MALLOC_ARRAY(hosts, nhosts);
		if (hosts == NULL) {
			fprintf(stderr, "%s: not enough memory for %d hosts",
			    program_name, nhosts);
			exit(1);
		}
		if (spooled_command)
			e = gfarm_schedule_search_idle_by_program(
			    command_name, nhosts, hosts);
		else
			e = gfarm_schedule_search_idle_by_all(nhosts, hosts);
		if (e != NULL) {
			fprintf(stderr, "%s: scheduling %d nodes: %s\n",
				program_name, nprocs, e);
			exit(1);
		}
		scheduling_file = "";
	}
	else if (scheduling_file != NULL ||
		 gfarm_stringlist_length(&input_list) != 0) {
		/* file-affinity scheduling */
		if (scheduling_file == NULL)
			scheduling_file = gfarm_stringlist_elem(
				&input_list, 0);
		if (spooled_command)
			e = gfarm_url_hosts_schedule_by_program(
				scheduling_file, command_name, NULL,
				&nhosts, &hosts);
		else
			e = gfarm_url_hosts_schedule(scheduling_file,
				NULL, &nhosts, &hosts);
		if (e != NULL) {
			fprintf(stderr, "%s: scheduling by %s: %s\n",
				program_name, scheduling_file, e);
			exit(1);
		}
	}
	else {
		fprintf(stderr, "%s: no way to schedule nodes\n",
			program_name);
		exit(1);
	}

	/* create a machine file */
	fp = fdopen(mkstemp(strcpy(filename, template)), "w");
	if (fp == NULL) {
		fprintf(stderr, "%s: cannot create tempfile \"%s\"\n",
			program_name, filename);
		exit(1);
	}
	for (i = 0; i < nhosts; i++) {
		char *if_hostname;
		struct sockaddr peer_addr;

		/* reflect "address_use" directive in the `if_hostname' */
		e = gfarm_host_address_get(hosts[i],
		    gfarm_spool_server_port, &peer_addr, &if_hostname);
		if (e != NULL) {
			fprintf(fp, "%s\n", hosts[i]);
		} else {
			fprintf(fp, "%s\n", if_hostname);
			free(if_hostname);
		}
	}
	fclose(fp);

	/*
	 * register job manager
	 */
	e = gfarm_user_job_register(nhosts, hosts, program_name,
	    scheduling_file, argc - command_index, &argv[command_index],
	    &job_id);
	if (e != NULL) {
		fprintf(stderr, "%s: job register: %s\n", program_name, e);
		exit(1);
	}

	/*
	 * deliver gfarm:program.
	 */
	if (gfarm_is_url(command_name)) {
		e = gfarm_url_program_deliver(command_name, nhosts, hosts,
					      &delivered_paths);
		if (e != NULL) {
			fprintf(stderr, "%s: deliver %s: %s\n",
				program_name, command_name, e);
			exit(1);
		}
	}

	sprintf(total_nodes, "%d", nhosts);

	gfarm_stringlist_init(&arg_list);
	gfarm_stringlist_add(&arg_list, "mpirun");
#if 1
	/*
	 * without this option, the machine which is running gfmpirun_p4
	 * always becomes node 0. and total number of nodes becomes
	 * ``1 + total_nodesprocesses'' because this host will be included
	 * regardless whether machine file includes this host or not.
	 * XXX - this option is only available on mpich/p4.
	 */
	gfarm_stringlist_add(&arg_list, "-nolocal");
#endif
	gfarm_stringlist_add(&arg_list, "-machinefile");
	gfarm_stringlist_add(&arg_list, filename);
	gfarm_stringlist_add(&arg_list, "-np");
	gfarm_stringlist_add(&arg_list, total_nodes);
	gfarm_stringlist_add_list(&arg_list, &option_list);
	if (delivered_paths == NULL) {
		gfarm_stringlist_add(&arg_list, command_name);
	} else {
		/*
		 * XXX This assumes that all nodes are same architecture
		 * XXX and all nodes have same gfarm_root!
		 * XXX really broken.
		 */
		gfarm_stringlist_add(&arg_list, delivered_paths[0]);
	}
	gfarm_stringlist_cat(&arg_list, &argv[command_index + 1]);
	gfarm_stringlist_add(&arg_list, NULL);

	switch (fork()) {
	case 0:
		execvp("mpirun", GFARM_STRINGLIST_STRARRAY(arg_list));
		perror("mpirun");
		exit(1);
	case -1:
		perror("fork");
		exit(1);
	}

	sig_ignore(SIGHUP);
	sig_ignore(SIGINT);
	sig_ignore(SIGQUIT);
	sig_ignore(SIGTERM);
	sig_ignore(SIGTSTP);
	while (waitpid(-1, &status, 0) != -1 || errno == EINTR)
		;
	save_errno = errno;

#if 0 /* XXX - temporary solution; it is not necessary for the output
	 file to be the same number of fragments. */
	for (i = 0; i < gfarm_stringlist_length(&output_list); i++)
		gfarm_url_fragment_cleanup(
		    gfarm_stringlist_elem(&output_list, i), nhosts, hosts);
#endif
	unlink(filename);

	if (delivered_paths != NULL)
		gfarm_strings_free_deeply(nhosts, delivered_paths);
	gfarm_strings_free_deeply(nhosts, hosts);
	gfarm_stringlist_free(&arg_list);
	gfarm_stringlist_free(&output_list);
	gfarm_stringlist_free(&input_list);
	gfarm_stringlist_free(&option_list);
	e = gfarm_terminate();
	if (e != NULL) {
		fprintf(stderr, "%s: %s\n", program_name, e);
		exit(1);
	}
	return (save_errno != ECHILD ? 1 :
		WIFEXITED(status) ? WEXITSTATUS(status) : 1);
}
