#include "malloc.h"

zone_t				*zones_g = NULL;
pthread_mutex_t		mutex_g = PTHREAD_MUTEX_INITIALIZER;

void	*malloc_fn(size_t sz) {
	struct rlimit	rlimit;
	void			*alloc = NULL;
	zone_t			*last, *ptr;
	size_t			zone_sz;

	getrlimit(RLIMIT_AS, &rlimit);
	if (!sz || sz >= rlimit.rlim_max)
		return (NULL);
	zone_sz = get_zone_sz(sz);
	if (!zones_g)
		zones_g = init_zone(sz);
	for (ptr = zones_g; !alloc && ptr; last = ptr, ptr = ptr->next)
		if (zone_sz == ptr->sz)
			alloc = add_block_to_zone(ptr, sz);
	if (!alloc) {
		last->next = init_zone(sz);
		alloc = add_block_to_zone(last->next, sz);
	}
	return (alloc);
}

void	*malloc(size_t sz) {
	void			*alloc = NULL;

	pthread_mutex_lock(&mutex_g);
	alloc = malloc_fn(sz);
	pthread_mutex_unlock(&mutex_g);
	return (alloc);
}
