#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>
# include <sys/mman.h>
# include <pthread.h>

# define NB_ALLOC		100
# define TINY_ALLOC		256
# define SMALL_ALLOC	4096

# define PAGE_SZ		sysconf(__SC_PAGESIZE)

typedef struct			block_s {
	size_t				sz;
	void				*next;
}						block_t;

typedef struct			zone_s {
	size_t				sz;
	void				*blocks;
	void				*next;
}						zone_t;

extern t_zone			*zones_g;
extern pthread_mutex_t	mutex_g;

void					*malloc(size_t sz);

void					*realloc(void *ptr, size_t sz);

void					free(void *ptr);

void					show_alloc_mem(void);

void					remove_zone(zone_t *zone);
void					remove_block(zone_t *zone, void *block);

size_t					get_alloc_sz(size_t block_sz);
size_t					get_zone_sz(size_t block_sz);
zone_t					*init_zone(size_t sz);
size_t					get_zone_sz(zone_t *zone);
zone_t					*find_zone(void *alloc);

block_t					*find_block(zone_t *zone, void *alloc);
void					*insert_block(zone_t *zone, size_t sz);

size_t					ft_strlen(const char *s);
void					ft_putnbr(unsigned long n);
void					ft_puthexa(unsigned long n);
void					*ft_memcpy(void *dst, const void *src, size_t n);

#endif
