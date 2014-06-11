#include "endian.h"

void* endian_copyToB16(void* dest, const void* src, const size_t n)
{
    void* ret;
    uint16_t* cur;
    size_t i;

    cur = (uint16_t*) dest;
    ret = memcpy(dest, src, n);
    for (i = 0 ; i < n / sizeof(uint16_t) ; i++)
    {
        cur[i] = endian_htons(cur[i]);
    }

    return ret;
}

void* endian_copyToB32(void* dest, const void* src, size_t n)
{
    void* ret;
    uint32_t* cur;
    size_t i;

    cur = (uint32_t*) dest;
    ret = memcpy(dest, src, n);
    for (i = 0 ; i < n / sizeof(uint32_t) ; i++)
    {
        cur[i] = endian_htonl(cur[i]);
    }
    return ret;
}

void* endian_copyToB(void* dest, const void* src, const size_t n, const size_t size)
{
    void* ret;

    switch (size)
    {
        case sizeof(uint32_t) :
            ret = endian_copyToB32(dest, src, n);
            break;
        case sizeof(uint16_t) :
            ret = endian_copyToB16(dest, src, n);
            break;
        case sizeof(uint8_t) :
        default :
            ret = memcpy(dest, src, n);
            break;
    }

    return ret;
}
