#include <errno.h>
#include <stdlib.h>

#include <gfarm/error.h>
#include <gfarm/gfarm_misc.h>

size_t
gfarm_size_add(int *overflowp, size_t a, size_t b)
{
	size_t sum = a + b;

	if (sum < a)
		*overflowp = 1;
	return (sum);
}

size_t
gfarm_size_mul(int *overflowp, size_t a, size_t b)
{
	size_t product = a * b;

	if (b != 0 && product / b != a)
		*overflowp = 1;
	return (product);
}

void *
gfarm_calloc_array(size_t number, size_t size)
{
	int overflow = 0;

	gfarm_size_mul(&overflow, number, size);
	if (overflow) {
		errno = ENOMEM;
		return NULL;
	}
	return (calloc(number, size));
}

void *
gfarm_malloc_array(size_t number, size_t size)
{
	int overflow = 0;
	size_t total_size = gfarm_size_mul(&overflow, number, size);

	if (overflow) {
		errno = ENOMEM;
		return NULL;
	}
	return (malloc(total_size));
}

void *
gfarm_realloc_array(void *src, size_t number, size_t size)
{
	int overflow = 0;
	size_t total_size = gfarm_size_mul(&overflow, number, size);

	if (overflow) {
		errno = ENOMEM;
		return NULL;
	}
	return (realloc(src, total_size));
}
