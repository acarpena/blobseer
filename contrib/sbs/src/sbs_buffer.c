#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sbs_types.h"
#include "sbs_buffer.h"
#include "sbs_variable.h"

/* the buffer is a global variable 
 so it can be accessed by every functions
 in this file */
sbs_buffer buffer;

sbs_buffer sbs_buffer_get_instance() 
{
	return buffer;
}
/* sbs_buffer_create initialize the global buffer by
 allocating memory for it and initializing fields */
sbs_buffer sbs_buffer_create()
{
	buffer = (sbs_buffer)malloc(sizeof(struct sbs_buffer_s));
	buffer->varlist = NULL;
	return buffer;
}

/* this is a recursiv local function that free every
 entry in a variable list */
int free_varlist(struct sbs_var_node_s* node) 
{
	if(node == NULL)
		return 0;
	else
	{
		sbs_variable v = node->var;
		free_varlist(node->next);
		free(node);
		free(v->name);
		free(v->lsizes);
		free(v->gsizes);
		free(v->offsets);
                free(v);
		return 0;
	}
}

/* free the global buffer */
int sbs_buffer_free()
{
	/* call free_varlist to free all the stored variables */
	free_varlist(buffer->varlist);
	free(buffer);
	return 0;
}

/* add a variable entry if it is not already defined */
sbs_variable sbs_buffer_define_var(char* name, int type, int dim,
			           size_t* lsizes, size_t* gsizes,
			           uint64_t* offsets, int time, int count)
{
	sbs_variable var = sbs_buffer_find_var(name, time);
	if(var == NULL)
	{
		var = (sbs_variable)malloc(sizeof(struct sbs_variable_s));
		var->name = (char*)malloc(strlen(name)+1);
		strcpy(var->name,name);
		var->dim = dim;
		var->type = type;
		var->time = time;
		var->lsizes = (size_t*)malloc(sizeof(size_t)*dim);
		memcpy(var->lsizes,lsizes,dim*sizeof(size_t));
		var->gsizes = (size_t*)malloc(sizeof(size_t)*dim);
		memcpy(var->gsizes,gsizes,dim*sizeof(size_t));
		var->offsets = (uint64_t*)malloc(sizeof(uint64_t)*dim);
		memcpy(var->offsets,offsets,dim*sizeof(uint64_t));
		size_t total_size = sbs_sizeof(var->type);
		int i; for(i=0;i<dim;i++) total_size *= lsizes[i];
		var->data = malloc(total_size);
		struct sbs_var_node_s* newnode =
			malloc(sizeof(struct sbs_var_node_s));
		newnode->next = buffer->varlist;
		buffer->varlist = newnode;
		newnode->var = var;
		newnode->mpi_flag = 0;
		newnode->count = count;
		//printf("[memdebug] variable %s has been defined, its address is %d\n",name,(int)(var->data));
		return var;
	}
	else return NULL;
}

/* local function that free a particular variable in a list */
struct sbs_var_node_s* varlist_freevar(struct sbs_var_node_s* node, 
					char* name, int time)
{
	if(node == NULL) return NULL;
	if(strcmp(name,node->var->name) == 0 && node->var->time == time)
	{
		//printf("[debug] in varlist_free, found %s/%s for time %d\n",name,node->var->name,time);
		struct sbs_var_node_s* next = node->next;
		sbs_variable var = node->var;
		free(var->name);
		//printf("[debug] 111\n");
		free(var->offsets);
		free(var->lsizes);
		free(var->gsizes);
		//printf("[debug] var->data = %d\n",(int)(var->data));
		free(var->data);
                free(var);
		free(node);
		return next;
	}
	else
	{
		struct sbs_var_node_s* next 
			= varlist_freevar(node->next,name,time);
		node->next = next;
		return node;
	}
}

/* sbs_buffer_free_var finds a particular variable entry
 given its name and remove it from the buffer */
int sbs_buffer_free_var(char* name, int time)
{
	buffer->varlist = varlist_freevar(buffer->varlist,name, time);
	return 0;
}

/* local function that helps finding a var entry given its name */
sbs_variable varlist_getvar(struct sbs_var_node_s* node, 
					char* name, int time)
{
	if(node == NULL) return NULL;
	if(strcmp(name,node->var->name) == 0 && node->var->time == time)
	{
		return node->var;
	}
	else
	{
		return varlist_getvar(node->next,name,time);
	}
}

/* find a variable entry given its name and returns it.
 return NULL is no such entry exists */
sbs_variable sbs_buffer_find_var(char* name, int time)
{
	return varlist_getvar(buffer->varlist,name,time);
}

int varlist_decrvar(struct sbs_var_node_s* node, 
					char* name, int time)
{
	if(node == NULL) return -1;
	if(strcmp(name,node->var->name) == 0 && node->var->time == time)
	{
		node->count -= 1;
		return node->count;
	}
	else
	{
		return varlist_decrvar(node->next,name,time);
	}
}

/* decrement the counter associated with a given variable.
 This is used to know how many times we have still to access
 this variable before flushing it into files */
int sbs_buffer_decr_counter(char* name, int time)
{
	return varlist_decrvar(buffer->varlist,name,time);
}
