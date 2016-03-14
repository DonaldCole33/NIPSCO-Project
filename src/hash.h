/*
 * hash.h
 *
 *  Created on: Mar 10, 2016
 *      Author: doncole
 */

#ifndef HASH_H_
#define HASH_H_


struct entry_s;

typedef struct entry_s entry_t;

struct hashtable_s;

typedef struct hashtable_s hashtable_t;

/* Create a new hashtable. */
hashtable_t *ht_create( int size );

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, char *key );

/* Create a key-value pair. */
entry_t *ht_newpair( char *key, char *value );

/* Insert a key-value pair into a hash table. */
void ht_set( hashtable_t *hashtable, char *key, char *value );

/* Retrieve a key-value pair from a hash table. */
char *ht_get( hashtable_t *hashtable, char *key );


#endif /* HASH_H_ */
