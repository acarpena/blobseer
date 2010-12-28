/*
 * $Id: param.c 3617 2007-03-12 08:32:55Z n-soda $
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <gfarm/gfarm_config.h>
#include <gfarm/error.h>
#include <gfarm/gfarm_misc.h>
#include "liberror.h"
#include "hostspec.h"
#include "param.h"

struct gfarm_param_config {
	struct gfarm_param_config *next;

	int param_type_index;
	long value;
	struct gfarm_hostspec *hostspec;
};

gfarm_error_t
gfarm_param_config_parse_long(int ntypes, struct gfarm_param_type *type_table,
	char *config,
	int *type_indexp, long *valuep)
{
	int i;
	size_t configlen, namelen;
	struct gfarm_param_type *type;
	long value;
	char *ep;

	configlen = strlen(config);
	namelen = strcspn(config, "=");
	for (i = 0; i < ntypes; i++) {
		type = &type_table[i];
		if (strlen(type->name) != namelen ||
		    memcmp(config, type->name, namelen) != 0)
			continue;
		if (configlen == namelen) { /* no value specified */
			if (!type->boolean)
				return (GFARM_ERRMSG_VALUE_IS_NOT_SPECIFIED);
			value = 1;
		} else {
			if (type->boolean)
				return (GFARM_ERRMSG_VALUE_IS_NOT_ALLOWED_FOR_BOOLEAN);
			if (config[namelen + 1] == '\0')
				return (GFARM_ERRMSG_VALUE_IS_EMPTY);
			errno = 0;
			value = strtol(&config[namelen + 1], &ep, 0);
			if (errno != 0)
				return (gfarm_errno_to_error(errno));
			if (*ep != '\0')
				return (GFARM_ERRMSG_INVALID_CHAR_IN_VALUE);
		}

		*type_indexp = i;
		*valuep = value;
		return (GFARM_ERR_NO_ERROR);				
	}
	return (GFARM_ERR_NO_SUCH_OBJECT);
}

gfarm_error_t
gfarm_param_config_add_long(
	struct gfarm_param_config ***config_list_lastp,
	int param_type_index, long value, struct gfarm_hostspec *hsp)
{
	struct gfarm_param_config *pcp;

	GFARM_MALLOC(pcp);
	if (pcp == NULL)
		return (GFARM_ERR_NO_MEMORY);
	pcp->next = NULL;
	pcp->param_type_index = param_type_index;
	pcp->value = value;
	pcp->hostspec = hsp;

	*(*config_list_lastp) = pcp;
	*config_list_lastp = &pcp->next;

	return (GFARM_ERR_NO_ERROR);				
}

gfarm_error_t
gfarm_param_apply_long_by_name_addr(struct gfarm_param_config *list,
	const char *name, struct sockaddr *addr,
	gfarm_error_t (*f)(void *, int, long), void *closure)
{
	gfarm_error_t e_save = GFARM_ERR_NO_ERROR, e;
	long done = 0;

	for (; list != NULL; list = list->next) {
		if (gfarm_hostspec_match(list->hostspec, name, addr)) {
			assert(list->param_type_index < sizeof(long)*CHAR_BIT);
			if ((done & (1 << list->param_type_index)) != 0)
				continue; /* use first match only */
			done |= (1 << list->param_type_index);
			e = (*f)(closure, list->param_type_index, list->value);
			if (e != GFARM_ERR_NO_ERROR &&
			    e_save == GFARM_ERR_NO_ERROR)
				e_save = e;
		}
	}
	return (e_save);
}

gfarm_error_t
gfarm_param_apply_long(struct gfarm_param_config *list,
	gfarm_error_t (*f)(void *, int, long), void *closure)
{
	gfarm_error_t e_save = GFARM_ERR_NO_ERROR, e;
	long done = 0;

	for (; list != NULL; list = list->next) {
		assert(list->param_type_index < sizeof(long)*CHAR_BIT);
		if ((done & (1 << list->param_type_index)) != 0)
			continue; /* use first match only */
		done |= (1 << list->param_type_index);
		e = (*f)(closure, list->param_type_index,  list->value);
		if (e != GFARM_ERR_NO_ERROR && e_save == GFARM_ERR_NO_ERROR)
			e_save = e;
	}
	return (e_save);
}

gfarm_error_t
gfarm_param_get_long_by_name_addr(struct gfarm_param_config *list,
	int param_type_index,
	char *name, struct sockaddr *addr,
	long *valuep)
{
	for (; list != NULL; list = list->next) {
		if (list->param_type_index == param_type_index &&
		    gfarm_hostspec_match(list->hostspec, name, addr)) {
			*valuep = list->value;
			return (GFARM_ERR_NO_ERROR);
		}
	}
	return (GFARM_ERR_NO_SUCH_OBJECT);
}

gfarm_error_t
gfarm_param_get_long(struct gfarm_param_config *list,
	int param_type_index, long *valuep)
{
	for (; list != NULL; list = list->next) {
		if (list->param_type_index == param_type_index) {
			*valuep = list->value;
			return (GFARM_ERR_NO_ERROR);
		}
	}
	return (GFARM_ERR_NO_SUCH_OBJECT);
}

/*
 * netparam
 */

struct gfarm_netparam_info {
	long minimum, maximum, default_value;
	struct gfarm_param_config *list;
	struct gfarm_param_config **last;
};

struct gfarm_netparam_info gfarm_netparam_parallel_streams = {
	/* minimum */ 1,
	/* maximum */ 1000,
	/* default */ 1,
	NULL,
	&gfarm_netparam_parallel_streams.list,
};

struct gfarm_netparam_info gfarm_netparam_stripe_unit_size = {
	/* minimum */ 0,
	/* maximum */ INT_MAX,
	/* default */ 0,
	NULL,
	&gfarm_netparam_stripe_unit_size.list,
};

struct gfarm_netparam_info gfarm_netparam_rate_limit = {
	/* minimum */ 0,
	/* maximum */ INT_MAX,
	/* default */ 0,
	NULL,
	&gfarm_netparam_rate_limit.list,
};

struct gfarm_netparam_info gfarm_netparam_file_read_size = {
	/* minimum */ 0,
	/* maximum */ INT_MAX,
	/* default */ 4096,
	NULL,
	&gfarm_netparam_file_read_size.list,
};

struct gfarm_netparam_info gfarm_netparam_file_sync_rate = {
	/* minimum */ 0,
	/* maximum */ INT_MAX,
	/* default */ 0,
	NULL,
	&gfarm_netparam_file_sync_rate.list,
};

struct gfarm_netparam_info gfarm_netparam_file_sync_stripe = {
	/* minimum */ 0,
	/* maximum */ INT_MAX,
	/* default */ 0,
	NULL,
	&gfarm_netparam_file_sync_stripe.list,
};

struct gfarm_netparam_info gfarm_netparam_send_stripe_sync = {
	/* minimum */ 0,
	/* maximum */ 1,
	/* default */ 0,
	NULL,
	&gfarm_netparam_send_stripe_sync.list,
};

struct gfarm_netparam_info gfarm_netparam_recv_stripe_sync = {
	/* minimum */ 0,
	/* maximum */ 1,
	/* default */ 0,
	NULL,
	&gfarm_netparam_recv_stripe_sync.list,
};

struct gfarm_param_type gfarm_netparam_type_table[] = {
    { "parallel_streams", 0, &gfarm_netparam_parallel_streams },
    { "stripe_unit_size", 0, &gfarm_netparam_stripe_unit_size },
    { "rate_limit", 0, &gfarm_netparam_rate_limit },
    { "file_read_size", 0, &gfarm_netparam_file_read_size },
    { /* "file_" */ "sync_rate", 0, &gfarm_netparam_file_sync_rate },
    { "file_sync_stripe", 0, &gfarm_netparam_file_sync_stripe },
    { "send_stripe_sync", 0, &gfarm_netparam_send_stripe_sync },
    { "recv_stripe_sync", 0, &gfarm_netparam_recv_stripe_sync },
};

#define NNETPARAMS GFARM_ARRAY_LENGTH(gfarm_netparam_type_table)

gfarm_error_t
gfarm_netparam_config_add_long(char *config, struct gfarm_hostspec *hsp)
{
	gfarm_error_t e;
	int param_type_index;
	long value;
	struct gfarm_netparam_info *info;

	e = gfarm_param_config_parse_long(
	    NNETPARAMS, gfarm_netparam_type_table,
	    config, &param_type_index, &value);
	if (e == GFARM_ERR_NO_SUCH_OBJECT)
		return (GFARM_ERRMSG_UNKNOWN_PARAMETER);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);
	info = gfarm_netparam_type_table[param_type_index].extension;
	if (value < info->minimum || value > info->maximum)
		return (GFARM_ERR_NUMERICAL_ARGUMENT_OUT_OF_DOMAIN);
	return (gfarm_param_config_add_long(&info->last, 0, value, hsp));
}

gfarm_error_t
gfarm_netparam_config_get_long(struct gfarm_netparam_info *info,
	char *name, struct sockaddr *addr,
	long *valuep)
{
	gfarm_error_t e;
	long value;

	e = gfarm_param_get_long_by_name_addr(info->list, 0, name, addr,
	    &value);
	if (e == GFARM_ERR_NO_SUCH_OBJECT) {
		*valuep = info->default_value;
	} else if (e != GFARM_ERR_NO_ERROR) {
		return (e);
	} else {
		*valuep = value;
	}
	return (GFARM_ERR_NO_ERROR);
}
