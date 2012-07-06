/*
 * Copyright 2011 Kenneth Skovhede <kenneth@hexad.dk>
 *
 * This file is part of cphVB.
 *
 * cphVB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cphVB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cphVB. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef CPHVB_MCACHE_H
#define CPHVB_MCACHE_H
#include <cphvb.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  Initiate mcache to a fixed size.
 *
 *  cache will hold 'size' elements in the cache.
 *
 *  Expiration / cache invalidation is based on round-robin;
 *  Whenever an element is added to mcache the round-robin
 *  counter is incremented.
 *
 */
void cphvb_mcache_init( int size );

/**
 * Deallocates mcache arrays.
 */
void cphvb_mcache_delete();

/**
 * Remove all entries from mcache and de-allocate them
 */
void cphvb_mcache_clear();

/**
 * Return and remove a pointer of size 'size' from mcache.
 *
 * This removes it from the mcache!
 *
 * @return null If none exists.
 */
cphvb_data_ptr cphvb_mcache_find( cphvb_intp bytes );

/**
 * Add an element to mcache.
 * 
 * @param data Pointer to allocated data.
 * @param size Size in bytes of the allocated data.
 */
void cphvb_mcache_insert( cphvb_data_ptr data, cphvb_intp size );
cphvb_error cphvb_mcache_free( cphvb_instruction* inst );
cphvb_error cphvb_mcache_malloc( cphvb_instruction* inst );

#ifdef __cplusplus
}
#endif

#endif /* CPHVB_MCACHE_H */