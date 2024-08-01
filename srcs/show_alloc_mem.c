#include "malloc.h"

static size_t	print_block(block_t *block) {
	write(STDOUT_FILENO, "0x", 2);
	ft_puthexa((unsigned long)block + sizeof(block_t));
	write(STDOUT_FILENO, " - ", 3);
	write(STDOUT_FILENO, "0x", 2);
	ft_puthexa((unsigned long)block + block->sz);
	write(STDOUT_FILENO, " : ", 3);
	ft_putnbr(block->sz - sizeof(block_t));
	write(STDOUT_FILENO, " bytes\n", 7);
	return (block->sz - sizeof(block_t));
}

static size_t	print_zone(zone_t *zone) {
	size_t	sum = 0;
	block_t	*tmp = zone->blocks;

	if (tmp->sz <= TINY_ALLOC)
		write(STDOUT_FILENO, "TINY : 0x", 9);
	else if (tmp->sz <= SMALL_ALLOC)
		write(STDOUT_FILENO, "SMALL : 0x", 10);
	else
		write(STDOUT_FILENO, "LARGE : 0x", 10);
	ft_puthexa((unsigned long)zone + sizeof(zone_t));
	write(STDOUT_FILENO, "\n", 1);
	for (; tmp; sum += print_block(tmp), tmp = tmp->next);
	return (sum);
}

void			show_alloc_mem(void) {
	size_t	sum = 0;

	pthread_mutex_lock(&mutex_g);
	for (zone_t *zones = zones_g; zones; sum += print_zone(zones), zones = zones->next);
	write(STDOUT_FILENO, "Total : ", 8);
	ft_putnbr(sum);
	write(STDOUT_FILENO, " bytes\n", 7);
	pthread_mutex_unlock(&mutex_g);
}
