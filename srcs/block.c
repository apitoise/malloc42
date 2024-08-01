#include <malloc.h>

void	*add_block(void *ptr, size_t block_sz, void *next) {
	block_t	*new;

	new = ptr;
	new->sz = block_sz;
	new->next = next;
	return (new);
}

void	*add_block_to_zone(zone_t *zone, size_t sz) {
	size_t	block_sz = sz + sizeof(block_t);
	int		in_use_sz;
	block_t	*block = NULL;

	if (!zone->blocks)
		zone->blocks = (block = add_block((void *)zone + sizeof(zone_t), block_sz, NULL));
	else {
		in_use_sz = get_in_use_sz_zone(zone);
		if (zone->sz - in_use_sz > block_size)
			block = insert_block(zone, block_sz);
	}
	if (!block)
		return (NULL);
	return ((void *)block + sizeof(block_t));
}
