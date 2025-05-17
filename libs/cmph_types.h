#ifndef __CMPH_TYPES_H__
#define __CMPH_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int cmph_uint32;
typedef unsigned char cmph_uint8;

typedef enum {
    CMPH_HASH_JENKINS = 0
} CMPH_HASH;

typedef enum {
    CMPH_BDZ = 0
} CMPH_ALGO;

#ifdef __cplusplus
}
#endif

#endif
