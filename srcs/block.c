#include "malloc.h"

static void			*add_block(void *ptr, size_t block_sz, void *next) {
	block_t	*new;

	new = ptr;
	new->sz = block_sz;
	new->next = next;
	return (new);
}

static void		*insert_block(zone_t *zone, size_t sz) {
	block_t	*block;

	if (((void *)zone + sizeof(zone_t)) < zone->blocks
		&& (size_t)(zone->blocks - ((void *)zone + sizeof(zone_t))) >= sz) {
		block = add_block((void *)zone + sizeof(zone_t), sz, zone->blocks);
		zone->blocks = block;
		return (block);
	}
	for (block = zone->blocks; block->next; block = block->next)
		if ((size_t)(block->next - ((void *)block + block->sz)) >= sz)
			return ((block->next = add_block((void *)block + block->sz, sz, block->next)));
	if (zone->sz - (((void *)block + block->sz) - (void *)zone) > sz)
		return ((block->next = add_block((void *)block + block->sz, sz, NULL)));
	return (NULL);
}

block_t			*find_block(zone_t *zone, void *alloc) {
	block_t	*block;

	for (block = zone->blocks; block && ((void *)block + sizeof(block_t)) != alloc; block = block->next);
	return (block);
}

void			*add_block_to_zone(zone_t *zone, size_t sz) {
	size_t	block_sz = sz + sizeof(block_t);
	int		in_use_sz;
	block_t	*block = NULL;

	if (!zone->blocks)
		zone->blocks = (block = add_block((void *)zone + sizeof(zone_t), block_sz, NULL));
	else {
		in_use_sz = get_in_use_sz_zone(zone);
		if (zone->sz - in_use_sz > block_sz)
			block = insert_block(zone, block_sz);
	}
	if (!block)
		return (NULL);
	return ((void *)block + sizeof(block_t));
}
