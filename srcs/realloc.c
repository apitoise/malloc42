#include "malloc.h"

void	*realloc_fn(void *ptr, size_t sz) {
	zone_t	*zone;
	block_t	*block;
	void	*new;

	zone = find_zone(ptr);
	if (!zone)
		return (NULL);
	block = find_block(zone, ptr);
	if (get_alloc_sz(block->sz) == get_alloc_sz(sz + sizeof(block_t))
		&& (block->sz >= sz + sizeof(block_t)
			|| (block->next
				&& (size_t)(block->next - (void *)block) >= sz + sizeof(block_t)))) {
		block->sz = sz + sizeof(block_t);
		return (ptr);
	}
	if (!(new = malloc_fn(sz)))
		return (NULL);
	if (block->sz - sizeof(block_t) <= sz)
		new = ft_memcpy(new, ptr, block->sz - sizeof(block_t));
	else
		new = ft_memcpy(new, ptr, sz);
	free_fn(ptr);
	return (new);
}

void	*realloc(void *ptr, size_t sz) {
	void	*new;

	if (!ptr)
		return (malloc(sz));
	pthread_mutex_lock(&mutex_g);
	new = realloc_fn(ptr, sz);
	pthread_mutex_unlock(&mutex_g);
	return (new);
}
