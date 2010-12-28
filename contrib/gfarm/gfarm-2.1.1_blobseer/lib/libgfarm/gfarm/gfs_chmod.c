#include <stdio.h>	/* config.h needs FILE */
#include <stdlib.h>

#define GFARM_INTERNAL_USE
#include <gfarm/gfarm.h>

#include "gfutil.h"
#include "timer.h"

#include "gfs_profile.h"
#include "gfm_client.h"
#include "config.h"
#include "lookup.h"

gfarm_error_t
gfs_chmod(const char *path, gfarm_mode_t mode)
{
	gfarm_error_t e;

	if ((e = gfm_client_compound_begin_request(gfarm_metadb_server))
	    != GFARM_ERR_NO_ERROR)
		gflog_warning("compound_begin request: %s",
		    gfarm_error_string(e));
	else if ((e = gfm_tmp_open_request(gfarm_metadb_server, path,
	    GFARM_FILE_LOOKUP)) != GFARM_ERR_NO_ERROR)
		gflog_warning("tmp_open(%s) request: %s", path,
		    gfarm_error_string(e));
	else if ((e = gfm_client_fchmod_request(gfarm_metadb_server, mode))
	    != GFARM_ERR_NO_ERROR)
		gflog_warning("fchmod request: %s", gfarm_error_string(e));
	else if ((e = gfm_client_compound_end_request(gfarm_metadb_server))
	    != GFARM_ERR_NO_ERROR)
		gflog_warning("compound_end request: %s",
		    gfarm_error_string(e));

	else if ((e = gfm_client_compound_begin_result(gfarm_metadb_server))
	    != GFARM_ERR_NO_ERROR)
		gflog_warning("compound_begin result: %s",
		    gfarm_error_string(e));
	else if ((e = gfm_tmp_open_result(gfarm_metadb_server, path, NULL))
	    != GFARM_ERR_NO_ERROR)
#if 0
		gflog_warning("tmp_open(%s) result: %s", path,
		    gfarm_error_string(e));
#else
		;
#endif
	else if ((e = gfm_client_fchmod_result(gfarm_metadb_server))
	    != GFARM_ERR_NO_ERROR)
#if 0
		gflog_warning("fchmod result: %s", gfarm_error_string(e));
#else
		;
#endif
	else if ((e = gfm_client_compound_end_result(gfarm_metadb_server))
	    != GFARM_ERR_NO_ERROR) {
		gflog_warning("compound_end result: %s",
		    gfarm_error_string(e));
	}
	/* NOTE: the opened descriptor is automatically closed by gfmd */

	return (e);
}
