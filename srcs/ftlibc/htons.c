
# include <netinet/in.h>
# include <ftlibc.h>

__attribute__ ((pure))
uint16_t ft_htons(uint16_t x)
{
#if BYTE_ORDER == BIG_ENDIAN
    return x;
#elif BYTE_ORDER == LITTLE_ENDIAN
    return BIT_SWAP16(x);
#else
# error "What kind of system is this?"
#endif
}
