
#ifndef __SRC_UTILS_ENDIAN_H__
#define __SRC_UTILS_ENDIAN_H__

#include <stdint.h>

#define bswap16(_x)        (uint16_t)((_x) << 8 | (_x) >> 8)

#define bswap32(_x)                     \
    (((_x) >> 24) |                     \
    (((_x) & (0xff << 16)) >> 8) |      \
    (((_x) & (0xff << 8)) << 8) |       \
    ((_x) << 24))

#define bswap64(_x)                     \
    (((_x) >> 56) |                     \
    (((_x) >> 40) & (0xffUL << 8)) |    \
    (((_x) >> 24) & (0xffUL << 16)) |   \
    (((_x) >> 8) & (0xffUL << 24)) |    \
    (((_x) << 8) & (0xffUL << 32)) |    \
    (((_x) << 24) & (0xffUL << 40)) |   \
    (((_x) << 40) & (0xffUL << 48)) |   \
    ((_x) << 56))

#if ( 1>>1 == 0 )
    // Little Endian
    #ifndef htobe16
        #define    htobe16(x)    bswap16((uint16_t)(x))
    #endif
    #ifndef htobe32
        #define    htobe32(x)    bswap32((uint32_t)(x))
    #endif
    #ifndef htobe64
        #define    htobe64(x)    bswap64((uint64_t)(x))
    #endif
    #ifndef htole16
        #define    htole16(x)    ((uint16_t)(x))
    #endif
    #ifndef htole32
        #define    htole32(x)    ((uint32_t)(x))
    #endif
    #ifndef htole64
        #define    htole64(x)    ((uint64_t)(x))
    #endif
    #ifndef be16toh
        #define    be16toh(x)    bswap16((uint16_t)(x))
    #endif
    #ifndef be32toh
        #define    be32toh(x)    bswap32((uint32_t)(x))
    #endif
    #ifndef be64toh
        #define    be64toh(x)    bswap64((uint64_t)(x))
    #endif
    #ifndef le16toh
        #define    le16toh(x)    ((uint16_t)(x))
    #endif
    #ifndef le32toh
        #define    le32toh(x)    ((uint32_t)(x))
    #endif
    #ifndef le64toh
        #define    le64toh(x)    ((uint64_t)(x))
    #endif
#else
    // Big Endian
    #ifndef htobe16
        #define    htobe16(x)    ((uint16_t)(x))
    #endif
    #ifndef htobe32
        #define    htobe32(x)    ((uint32_t)(x))
    #endif
    #ifndef htobe64
        #define    htobe64(x)    ((uint64_t)(x))
    #endif
    #ifndef htole16
        #define    htole16(x)    bswap16((uint16_t)(x))
    #endif
    #ifndef htole32
        #define    htole32(x)    bswap32((uint32_t)(x))
    #endif
    #ifndef htole64
        #define    htole64(x)    bswap64((uint64_t)(x))
    #endif
    #ifndef be16toh
        #define    be16toh(x)    ((uint16_t)(x))
    #endif
    #ifndef be32toh
        #define    be32toh(x)    ((uint32_t)(x))
    #endif
    #ifndef be64toh
        #define    be64toh(x)    ((uint64_t)(x))
    #endif
    #ifndef le16toh
        #define    le16toh(x)    bswap16((uint16_t)(x))
    #endif
    #ifndef le32toh
        #define    le32toh(x)    bswap32((uint32_t)(x))
    #endif
    #ifndef le64toh
        #define    le64toh(x)    bswap64((uint64_t)(x))
    #endif
#endif

#endif
