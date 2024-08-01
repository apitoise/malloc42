#include "malloc.h"

size_t	ft_strlen(const char *s) {
	size_t len = 0;
	for (; s[len]; ++len);
	return (len);
}

void	ft_putnbr(size_t n) {
	char	str[12];
	size_t	len = 0;

	for (size_t tmp = n; tmp; ++len)
		tmp /= 10;
	if (!len)
		str[len++] = '0';
	str[len] = '\0';
	for (; len--; n /= 10)
		str[len] = (n % 10) + 48;
	write(STDOUT_FILENO, str, ft_strlen(str));
}

void	ft_puthexa(unsigned long n) {
	int		len = 0;
	char	str[20];

	for (unsigned long tmp = n; tmp; ++len)
		tmp /= 16;
	if (!len)
		str[len++] = '0';
	str[len] = '\0';
	for (; len--; n /= 16) {
		if (n % 16 < 10)
			str[len] = (n % 16) + 48;
		else
			str[len] = (n % 16) + (65 - 10);
	}
	write(STDOUT_FILENO, str, ft_strlen(str));
}

void	*ft_memcpy(void *dst, const void *src, size_t n) {
	char	*ptr_src;
	char	*ptr_dst;

	ptr_src = (char *)src;
	ptr_dst = (char *)dst;
	while (n--)
		*ptr_dst++ = *ptr_src++;
	return (dst);
}
