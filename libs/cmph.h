#ifndef __CMPH_H__
#define __CMPH_H__

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cmph_types.h"

typedef struct __config_t cmph_config_t;
typedef struct __cmph_t cmph_t;

typedef struct 
{
        void *data;
        cmph_uint32 nkeys;
        int (*read)(void *, char **, cmph_uint32 *);
        void (*dispose)(void *, char *, cmph_uint32);
        void (*rewind)(void *);
} cmph_io_adapter_t;

/** Adapter pattern API **/
/* please call free() in the created adapters */
cmph_io_adapter_t *cmph_io_nlfile_adapter(FILE * keys_fd);
void cmph_io_nlfile_adapter_destroy(cmph_io_adapter_t * key_source);

cmph_io_adapter_t *cmph_io_nlnkfile_adapter(FILE * keys_fd, cmph_uint32 nkeys);
void cmph_io_nlnkfile_adapter_destroy(cmph_io_adapter_t * key_source);

static inline cmph_io_adapter_t *cmph_io_vector_adapter(char **, cmph_uint32);
static inline void cmph_io_vector_adapter_destroy(cmph_io_adapter_t *);

cmph_io_adapter_t *cmph_io_byte_vector_adapter(cmph_uint8 ** vector, cmph_uint32 nkeys);
void cmph_io_byte_vector_adapter_destroy(cmph_io_adapter_t * key_source);

cmph_io_adapter_t *cmph_io_struct_vector_adapter(void * vector, 
                                                 cmph_uint32 struct_size, 
                                                 cmph_uint32 key_offset, 
                                                 cmph_uint32 key_len, 
                                                 cmph_uint32 nkeys);

void cmph_io_struct_vector_adapter_destroy(cmph_io_adapter_t * key_source);

/** Hash configuration API **/
static inline cmph_config_t *cmph_config_new(cmph_io_adapter_t *);
static inline void cmph_config_set_hashfuncs(cmph_config_t *, CMPH_HASH *);
static inline void cmph_config_set_verbosity(cmph_config_t *, cmph_uint32);
static inline void cmph_config_set_graphsize(cmph_config_t *, double);
static inline void cmph_config_set_algo(cmph_config_t *, CMPH_ALGO);
static inline void cmph_config_set_tmp_dir(cmph_config_t *, cmph_uint8 *);
static inline void cmph_config_set_mphf_fd(cmph_config_t *, FILE *);
static inline void cmph_config_set_b(cmph_config_t *, cmph_uint32);
static inline void cmph_config_set_keys_per_bin(cmph_config_t *, cmph_uint32);
static inline void cmph_config_set_memory_availability(cmph_config_t *, cmph_uint32);
static inline void cmph_config_destroy(cmph_config_t *);

/** Hash API **/
static inline cmph_t *cmph_new(cmph_config_t *);

/** cmph_uint32 cmph_search(cmph_t *mphf, const char *key, cmph_uint32 keylen);
 *  \brief Computes the mphf value. 
 *  \param mphf pointer to the resulting function
 *  \param key is the key to be hashed
 *  \param keylen is the key legth in bytes
 *  \return The mphf value
 */
static inline cmph_uint32 cmph_search(cmph_t *, const char *, cmph_uint32);

static inline cmph_uint32 cmph_size(cmph_t *);
static inline void cmph_destroy(cmph_t *);

/** Hash serialization/deserialization */
static inline int cmph_dump(cmph_t *, FILE *);
static inline cmph_t *cmph_load(FILE *);

/** \fn void cmph_pack(cmph_t *mphf, void *packed_mphf);
 *  \brief Support the ability to pack a perfect hash function into a preallocated contiguous memory space pointed by packed_mphf.
 *  \param mphf pointer to the resulting mphf
 *  \param packed_mphf pointer to the contiguous memory area used to store the 
 *  \param resulting mphf. The size of packed_mphf must be at least cmph_packed_size() 
 */
static inline void cmph_pack(cmph_t *, void *);

/** \fn cmph_uint32 cmph_packed_size(cmph_t *mphf);
 *  \brief Return the amount of space needed to pack mphf.
 *  \param mphf pointer to a mphf
 *  \return the size of the packed function or zero for failures
 */
static inline cmph_uint32 cmph_packed_size(cmph_t *);

/** cmph_uint32 cmph_search(void *packed_mphf, const char *key, cmph_uint32 keylen);
 *  \brief Use the packed mphf to do a search. 
 *  \param  packed_mphf pointer to the packed mphf
 *  \param key key to be hashed
 *  \param keylen key legth in bytes
 *  \return The mphf value
 */
static inline cmph_uint32 cmph_search_packed(void *, const char *, cmph_uint32);

// TIMING functions. To use the macro CMPH_TIMING must be defined
#include "cmph_time.h"

/*------------------------------------------------------------
 * Minimal stub implementation for environments where the
 * real libcmph library is unavailable. These inline
 * definitions allow the codebase to compile for testing
 * purposes, but they do not provide a working MPHF.
 *-----------------------------------------------------------*/

struct __config_t { int dummy; };
struct __cmph_t   { int dummy; };

static inline cmph_io_adapter_t *cmph_io_vector_adapter(char **, cmph_uint32)
{ return NULL; }
static inline void cmph_io_vector_adapter_destroy(cmph_io_adapter_t *) {}
static inline cmph_config_t *cmph_config_new(cmph_io_adapter_t *) { return NULL; }
static inline void cmph_config_set_hashfuncs(cmph_config_t *, CMPH_HASH *) {}
static inline void cmph_config_set_verbosity(cmph_config_t *, cmph_uint32) {}
static inline void cmph_config_set_graphsize(cmph_config_t *, double) {}
static inline void cmph_config_set_algo(cmph_config_t *, CMPH_ALGO) {}
static inline void cmph_config_set_tmp_dir(cmph_config_t *, cmph_uint8 *) {}
static inline void cmph_config_set_mphf_fd(cmph_config_t *, FILE *) {}
static inline void cmph_config_set_b(cmph_config_t *, cmph_uint32) {}
static inline void cmph_config_set_keys_per_bin(cmph_config_t *, cmph_uint32) {}
static inline void cmph_config_set_memory_availability(cmph_config_t *, cmph_uint32) {}
static inline void cmph_config_destroy(cmph_config_t *) {}
static inline cmph_t *cmph_new(cmph_config_t *) { return NULL; }
static inline cmph_uint32 cmph_search(cmph_t *, const char *, cmph_uint32) { return 0; }
static inline cmph_uint32 cmph_size(cmph_t *) { return 0; }
static inline void cmph_destroy(cmph_t *) {}
static inline int cmph_dump(cmph_t *, FILE *) { return -1; }
static inline cmph_t *cmph_load(FILE *) { return NULL; }
static inline void cmph_pack(cmph_t *, void *) {}
static inline cmph_uint32 cmph_packed_size(cmph_t *) { return 0; }
static inline cmph_uint32 cmph_search_packed(void *, const char *, cmph_uint32) { return 0; }

#ifdef __cplusplus
}
#endif

#endif
