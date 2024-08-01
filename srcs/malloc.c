#include "malloc.h"

zone_t				*zones_g = NULL;
pthread_mutex_t		mutex_g = PTHREAD_MUTEX_INITIALIZER;

void	*malloc(size_t sz) {
	void	*alloc = NULL;
	zone_t	*last, *ptr;
	size_t	zone_sz;

	pthread_mutex_lock(&mutex_g);
	zone_sz = get_zone_sz(sz);
	if (!zones_g)
		zones_g = init_zone(sz);
	ptr = zones_g;
	while (!alloc && ptr) {
		if (zone_sz == ptr->sz)
			alloc = add_block_to_zone(ptr, sz);
		last = ptr;
		ptr = ptr->next;
	}
}
