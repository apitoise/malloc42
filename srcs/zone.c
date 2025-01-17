#include "malloc.h"

size_t	get_alloc_sz(size_t block_sz) {
	if (block_sz <= TINY_ALLOC)
		return (TINY_ALLOC);
	else if (block_sz <= SMALL_ALLOC)
		return (SMALL_ALLOC);
	return (block_sz);
}

size_t	get_zone_sz(size_t sz) {
	size_t	new, alloc_sz;

	alloc_sz = get_alloc_sz(sz + sizeof(block_t));
	if (sz + sizeof(block_t) <= SMALL_ALLOC) {
		new = (NB_ALLOC / (PAGE_SZ / alloc_sz) + 1) * PAGE_SZ;
		if (new - NB_ALLOC * alloc_sz < sizeof(zone_t))
			new += PAGE_SZ;
	}
	else {
		new = (alloc_sz / PAGE_SZ + 1) * PAGE_SZ;
		if (new - alloc_sz < sizeof(zone_t))
			new += PAGE_SZ;
	}
	new = align_size(new, 16);
	return (new);
}

zone_t	*init_zone(size_t sz) {
	zone_t	*new;
	size_t	zone_sz = get_zone_sz(sz);

	new = mmap(NULL, zone_sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	new->sz = zone_sz;
	new->blocks = NULL;
	new->next = NULL;
	new = (zone_t *)align_size((size_t)new, 2 * sizeof(size_t));
	return (new);
}

size_t	get_in_use_sz_zone(zone_t *zone) {
	size_t	sz = sizeof(zone_t);

	if (zone->blocks)
		for (block_t *tmp = zone->blocks; tmp; tmp = tmp->next)
			sz += tmp->sz;
	return (sz);
}

zone_t	*find_zone(void *alloc) {
	for (zone_t *tmp = zones_g; tmp; tmp = tmp->next) 
		for (block_t *block = tmp->blocks; block; block = block->next)
			if (((void *)block + sizeof(block_t)) == alloc)
				return (tmp);
	return (NULL);
}
