#include "malloc.h"

static void	free_zone(zone_t *zone) {
	size_t	sz = (((SMALL_ALLOC + sizeof(block_t))
					* (NB_ALLOC + 1)) / PAGE_SZ)
					* PAGE_SZ;
	zone_t	*prev = NULL, *tmp;

	for (tmp = zones_g; tmp && tmp != zone; prev = tmp, tmp = tmp->next);
	if (prev || tmp->next || zone->sz < sz) {
		if (!prev)
			zones_g = tmp->next;
		else
			prev->next = tmp->next;
		munmap(zone, zone->sz);
	}
}

static void	free_block(zone_t *zone, void *block) {
	block_t	*ptr, *prev = NULL;

	for (ptr = zone->blocks;
		ptr && ((void *)ptr + sizeof(block_t)) != block;
		prev = ptr, ptr = ptr->next);
	if (!prev)
		zone->blocks = ptr->next;
	else
		prev->next = ptr->next;
}

void		free_fn(void *ptr) {
	zone_t	*zone;

	if (!(zone = find_zone(ptr)))
		return ;
	free_block(zone, ptr);
	if (!zone->blocks)
		free_zone(zone);
}

void		free(void *ptr) {
	if (!ptr)
		return ;
	pthread_mutex_lock(&mutex_g);
	free_fn(ptr);
	pthread_mutex_unlock(&mutex_g);
}
