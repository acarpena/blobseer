#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <gfarm/error.h>
#include <gfarm/gfarm_misc.h>
#include <gfarm/gfs.h> /* gfarm_off_t */

#include "dir.h"

#if USE_HASH

/**********************************************************************
 * hash table version
 **********************************************************************/

#define DIR_HASH_SIZE 53 /* prime */

Dir
dir_alloc(void)
{
	return (gfarm_hash_table_alloc(
	    DIR_HASH_SIZE, gfarm_hash_default, gfarm_hash_key_equal_default));
}

void
dir_free(Dir dir)
{
	gfarm_hash_table_free(dir);
}

#if 0 /* need to check "." and ".." */
int
dir_is_empty(Dir dir)
{
	DirCursor cursor;

	gfarm_hash_iterator_begin(dir, &cursor);
	return (gfarm_hash_iterator_is_end(&cursor));
}
#endif

DirEntry
dir_enter(Dir dir, const char *name, int namelen, int *createdp)
{
	struct inode **eipp;
	int created;
	DirEntry entry = gfarm_hash_enter(dir, name, namelen,
	    sizeof(*eipp), &created);

	if (entry == NULL)
		return (NULL);

	/* for assertion in dir_entry_set_inode() */
	if (created) {
		eipp = gfarm_hash_entry_data(entry);
		*eipp = NULL;
	}

	if (createdp != NULL)
		*createdp = created;
	return (entry);
}

DirEntry
dir_lookup(Dir dir, const char *name, int namelen)
{
	return (gfarm_hash_lookup(dir, name, namelen));
}

int
dir_remove_entry(Dir dir, const char *name, int namelen)
{
	return (gfarm_hash_purge(dir, name, namelen));
}

void
dir_entry_set_inode(DirEntry entry, struct inode *inode)
{
	struct inode **eipp = gfarm_hash_entry_data(entry);

	assert(*eipp == NULL); /* We don't allow to overwrite existing one */
	*eipp = inode;
}

struct inode *
dir_entry_get_inode(DirEntry entry)
{
	struct inode **eipp = gfarm_hash_entry_data(entry);

	assert(*eipp != NULL);
	return (*eipp);
}

char *
dir_entry_get_name(DirEntry entry, int *namelenp)
{
	*namelenp = gfarm_hash_entry_key_length(entry);
	return (gfarm_hash_entry_key(entry));
}

int
dir_cursor_lookup(Dir dir, const char *name, int namelen, DirCursor *cursor)
{
	return (gfarm_hash_iterator_lookup(dir, name, namelen, cursor));
}

int
dir_cursor_next(Dir dir, DirCursor *cursor)
{
	if (gfarm_hash_iterator_is_end(cursor))
		return (0); /* end of directory */
	gfarm_hash_iterator_next(cursor);
	return (gfarm_hash_iterator_is_end(cursor));
}

/* this is stupidly slow */
int
dir_cursor_set_pos(Dir dir, gfarm_off_t offset, DirCursor *cursor)
{
	gfarm_off_t i;

	gfarm_hash_iterator_begin(dir, cursor);
	for (i = 0; i < offset; i++) {
		if (gfarm_hash_iterator_is_end(cursor))
			return (0); /* failed */
		gfarm_hash_iterator_next(cursor);
	}
	return (1); /* ok */
}

/* this is stupidly slow */
gfarm_off_t
dir_cursor_get_pos(Dir dir, DirCursor *cursor)
{
	DirEntry entry = dir_cursor_get_entry(dir, cursor);
	gfarm_off_t i;

	gfarm_hash_iterator_begin(dir, cursor);
	for (i = 0; !gfarm_hash_iterator_is_end(cursor); i++) {
		if (gfarm_hash_iterator_access(cursor) == entry)
			break;
		gfarm_hash_iterator_next(cursor);
	}
	return (i);
}

DirEntry
dir_cursor_get_entry(Dir dir, DirCursor *cursor)
{
	return (gfarm_hash_iterator_access(cursor));
}

/*
 * this is stupidly slow,
 * because currently gfarm_hash doesn't maintain number of entries.
 */
gfarm_off_t
dir_get_entry_count(Dir dir)
{
	DirCursor cursor;
	gfarm_off_t i;

	gfarm_hash_iterator_begin(dir, &cursor);
	for (i = 0; !gfarm_hash_iterator_is_end(&cursor); i++) {
		gfarm_hash_iterator_next(&cursor);
	}
	return (i);
}

#else /* ! USE_HASH */

/**********************************************************************
 * red-black tree version
 *
 * (functions which have rbdir_ prefix are private.)
 **********************************************************************/

#include <stdlib.h>

/* this must be #defined before #include "tree.h" */
#define RB_AUGMENT(entry)	rbdir_augment(entry)

#include "tree.h"

struct rbdir_entry {
	RB_ENTRY(rbdir_entry) node;
	int keylen;
	char *key;

	gfarm_off_t nentries;

	struct inode *inode;
};

RB_HEAD(rbdir, rbdir_entry);

int
rbdir_compare(DirEntry a, DirEntry b)
{
	int len = a->keylen < b->keylen ? a->keylen : b->keylen;
	int cmp;

	cmp = memcmp(a->key, b->key, len);
	if (cmp != 0 || a->keylen == b->keylen)
		return (cmp);
	if (a->keylen < b->keylen)
		return (-1);
	else
		return (1);
}

int
rbdir_node_count(DirEntry entry)
{
	gfarm_off_t lc, rc;

	lc = RB_LEFT (entry, node) == NULL ? 0 : RB_LEFT (entry, node)->nentries;
	rc = RB_RIGHT(entry, node) == NULL ? 0 : RB_RIGHT(entry, node)->nentries;
	return (lc + rc + 1);
}

void
rbdir_augment(DirEntry entry)
{
	entry->nentries = rbdir_node_count(entry);
}

RB_PROTOTYPE(rbdir, rbdir_entry, node, rbdir_compare)
RB_GENERATE(rbdir, rbdir_entry, node, rbdir_compare)

DirEntry
rbdir_entry_prev(DirEntry entry)
{
	if (RB_LEFT(entry, node) != NULL) {
		entry = RB_LEFT(entry, node);
		while (RB_RIGHT(entry, node))
			entry = RB_RIGHT(entry, node);
	} else if (RB_PARENT(entry, node) != NULL &&
	    entry == RB_RIGHT(RB_PARENT(entry, node), node)) {
		entry = RB_PARENT(entry, node);
	} else {
		while (RB_PARENT(entry, node) != NULL &&
		    entry == RB_LEFT(RB_PARENT(entry, node), node))
			entry = RB_PARENT(entry, node);
		entry = RB_PARENT(entry, node);
	}
	return (entry);
}

void
rbdir_fixup(DirEntry entry)
{
	do {
		rbdir_augment(entry);
	} while ((entry = RB_PARENT(entry, node)) != NULL);
}

void
rbdir_entry_free(struct rbdir_entry *entry)
{
	free(entry->key);
	free(entry);
}

int
rbdir_entry_delete(Dir dir, DirEntry entry)
{
	DirEntry deleted;
	DirEntry parent = RB_PARENT(entry, node);
	DirEntry prev = rbdir_entry_prev(entry);

	deleted = RB_REMOVE(rbdir, dir, entry);
	if (prev != NULL)
		rbdir_fixup(prev);
	if (parent != NULL)
		rbdir_fixup(parent);
	if (deleted != NULL) {
		rbdir_entry_free(deleted);
		return (1);
	}
	return (0);
}

Dir
dir_alloc(void)
{
	Dir dir;

	GFARM_MALLOC(dir);
	if (dir == NULL)
		return (NULL);
	RB_INIT(dir);
	return (dir);
}

void
dir_free(Dir dir)
{
	DirEntry entry;

	while ((entry = RB_MIN(rbdir, dir)) != NULL)
		rbdir_entry_delete(dir, entry);
	free(dir);
}

#if 0 /* need to check "." and ".." */
int
dir_is_empty(Dir dir)
{
	return (RB_ROOT(dir) == NULL);
}
#endif

gfarm_off_t
dir_get_entry_count(Dir dir)
{
	DirEntry root = RB_ROOT(dir);

	if (root == NULL)
		return (0);
	return (root->nentries);
}

DirEntry
dir_enter(Dir dir, const char *name, int namelen, int *createdp)
{
	DirEntry entry;
	DirEntry found;
	DirEntry prev;

	GFARM_MALLOC(entry);
	if (entry == NULL)
		return (NULL); /* no memory */
	entry->keylen = namelen;
	GFARM_MALLOC_ARRAY(entry->key, namelen);
	if (entry->key == NULL) {
		free(entry);
		return (NULL); /* no memory */
	}
	memcpy(entry->key, name, namelen);
	entry->nentries = 1; /* leaf */

	found = RB_INSERT(rbdir, dir, entry);
	if (found != NULL) {
		rbdir_entry_free(entry);
		*createdp = 0;
		return (found);
	}

	rbdir_fixup(entry);
	prev = rbdir_entry_prev(entry);
	if (prev != NULL)
		rbdir_fixup(prev);

	/* for assertion in dir_entry_set_inode() */
	entry->inode = NULL;

	if (createdp != NULL)
		*createdp = 1;
	return (entry);
}

DirEntry
dir_lookup(Dir dir, const char *name, int namelen)
{
	struct rbdir_entry entry;

	entry.keylen = namelen;
	entry.key = (char *)name;
	return (RB_FIND(rbdir, dir, &entry));
}

int
dir_remove_entry(Dir dir, const char *name, int namelen)
{
	DirEntry entry;

	entry = dir_lookup(dir, name, namelen);
	if (entry == NULL)
		return (0);
	rbdir_entry_delete(dir, entry);
	return (1);
}

void
dir_entry_set_inode(DirEntry entry, struct inode *inode)
{
	/* We don't allow to overwrite existing one */
	assert(entry->inode == NULL);

	entry->inode = inode;
}

struct inode *
dir_entry_get_inode(DirEntry entry)
{
	assert(entry->inode != NULL);
	return (entry->inode);
}

char *
dir_entry_get_name(DirEntry entry, int *namelenp)
{
	*namelenp = entry->keylen;
	return (entry->key);
}

int
dir_cursor_lookup(Dir dir, const char *name, int namelen, DirCursor *cursor)
{
	DirEntry entry = dir_lookup(dir, name, namelen);

	if (entry != NULL) {
		*cursor = entry;
		return (1);
	}
	return (0);
}

int
dir_cursor_next(Dir dir, DirCursor *cursor)
{
	if (*cursor == NULL)
		return (0); /* end of directory */
	*cursor = RB_NEXT(rbdir, dir, *cursor);
	if (*cursor == NULL)
		return (0); /* end of directory */
	return (1); /* ok */
}

int
dir_cursor_set_pos(Dir dir, gfarm_off_t nth, DirCursor *cursor)
{
	DirEntry entry = RB_ROOT(dir);
	gfarm_off_t index;

	while (entry != NULL) {
		index = RB_LEFT(entry, node) == NULL ? 0 :
		    RB_LEFT(entry, node)->nentries;
		if (index == nth) {
			*cursor = entry;
			return (1); /* ok */
		} else if (nth < index) {
			entry = RB_LEFT(entry, node);
		} else { /* nth > index */
			nth -= index + 1;
			entry = RB_RIGHT(entry, node);
		}
	}
	return (0); /* failed */
}

gfarm_off_t
dir_cursor_get_pos(Dir dir, DirCursor *cursor)
{
	DirEntry key = *cursor;
	DirEntry entry = RB_ROOT(dir);
	int cmp;
	gfarm_off_t delta, index = 0;

	if (key == NULL) /* i.e. end of directory */
		return (dir_get_entry_count(dir));
	while (entry != NULL) {
		cmp = rbdir_compare(key, entry);
		if (cmp < 0) {
			entry = RB_LEFT(entry, node);
			continue;
		}
		delta = RB_LEFT(entry, node) == NULL ? 0 :
		    RB_LEFT(entry, node)->nentries;
		index += delta;
		if (cmp == 0)
			break;
		index++;
		/* cmp > 0 */
		entry = RB_RIGHT(entry, node);
	}
	return (index);
}

DirEntry
dir_cursor_get_entry(Dir dir, DirCursor *cursor)
{
	return (*cursor);
}

#endif /* ! USE_HASH */
