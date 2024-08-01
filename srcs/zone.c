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
		new = (NB_ALLOC / (PAGE_SZ / alloc_sz) + 1 * PAGE_SZ;
		if (new - NB_ALLOC * alloc_sz < sizeof(zone_t))
			new += PAGE_SZ;
	}
	else {
		new = (alloc_sz / PAGE_SZ + 1) * PAGE_SZ;
		if (new - alloc_sz < sizeof(zone_t))
			new += PAGE_SZ;
	}
	return (new);
}

zone_t	*init_zone(size_t sz) {
	zone_t	*new;
	size_t	zone_sz = get_zone_sz(sz);

	new = mmap(NULL, zone_zs, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	new->sz = zone_sz;
	new->blocks = NULL;
	new->next = NULL;
	return (new);
}

size_t	get_in_use_sz_zone(zone_t *zone) {
	size_t	sz = sizeof(zone_t);

	if (zone->blocks)
		for (block_t *tmp; tmp = zone->blocks; tmp = tmp->next)
			sz += tmp->sz;
	return (sz);
}
