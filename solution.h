#include <stddef.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct Entry {
    char* key;              // key is NULL if this slot is empty
    void* value;
    struct Entry* next;
} Entry;

typedef struct HashMap{
    Entry** entries;                    // hash slots
    size_t num_buckets;                 // size of _entries array
    size_t size;                        // number of items in hash table
    unsigned int (*hash)(char *key);    // hash function
} HashMap;

typedef void* (*ResolveCollisionCallback)(void *old_data, void *new_data);
typedef void (*DestroyDataCallback)(void *data);
void* dontOverWriteCallback(void *old_data, void *new_data);
void* overWriteCallback(void *old_data, void *new_data);
void destroyDataCallback(void *data);
HashMap *create_hashmap(size_t key_space);
Entry *newEntry();

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data);
void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision);
void *get_data(HashMap *hm, char *key);
void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data);

void iterate(HashMap *hm, void (*callback)(char *key, void *data));

unsigned int hash(char *key);
unsigned int hashPlusOne(char *key);
void set_hash_function(HashMap *hm, unsigned int (*hash_function)(char *key));



