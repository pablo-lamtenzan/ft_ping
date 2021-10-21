# pragma once

# include <inttypes.h>

# define BIT_SWAP16(x) (uint16_t)((((x) >> 8) & 0XFF) | (((x) & 0XFF) << 8))

uint16_t    ft_htons(uint16_t x);
uint16_t    ft_ntohs(uint16_t x);
char*       ft_inet_ntoa (struct in_addr in);
void        ft_memcpy(void *restrict dest, const void* restrict src, size_t n);
void*       ft_memset(void* s, int c, size_t n);
size_t      ft_strlen(const char* s);
long		ft_atol(const char *str);
int         ft_strcmp(const char* s1, const char* s2);
int         ft_strncmp(const char* s1, const char* s2, size_t size);
long        ft_strtol(const char *nptr, char **endptr, register int base);
double      ft_atof(const char *s);