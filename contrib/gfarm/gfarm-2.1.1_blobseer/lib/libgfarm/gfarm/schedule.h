/*
 * Copyright (c) 2003-2006 National Institute of Advanced
 * Industrial Science and Technology (AIST).  All rights reserved.
 *
 * Copyright (c) 2006 National Institute of Informatics in Japan,
 * All rights reserved.
 *
 * This file or a portion of this file is licensed under the terms of
 * the NAREGI Public License, found at
 * http://www.naregi.org/download/index.html.
 * If you redistribute this file, with or without modifications, you
 * must include this notice in the file.
 */

struct gfarm_host_sched_info;
struct gfs_file; /* GFS_File */

gfarm_error_t gfarm_schedule_select_host(int, struct gfarm_host_sched_info *,
	int, char **, int *);
/* XXX - defined in gfs_pio_section.h */
gfarm_error_t gfarm_schedule_file(struct gfs_file *, char **, gfarm_int32_t *);

#if 0 /* not yet in gfarm v2 */

void gfarm_strings_expand_cyclic(int, char **, int, char **);

int gfarm_is_active_fsnode(void);
int gfarm_is_active_fsnode_to_write(file_offset_t);

#endif
