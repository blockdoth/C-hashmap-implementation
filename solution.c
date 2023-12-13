#include <ctype.h>
#include "solution.h"
#define NEW_HASH


HashMap *create_hashmap(size_t key_space){
    if(key_space < 1){
        return NULL;
    }
    HashMap *hm = calloc(1,sizeof(HashMap));
    if (hm == NULL){
        return NULL;
    }
    hm->entries = calloc(key_space,sizeof(Entry*));
    if (hm->entries == NULL){
        free(hm);
        return NULL;
    }
    hm->num_buckets = key_space;
    hm->size = 0;
    set_hash_function(hm, hash);
    for(size_t i = 0; i < key_space; i++){
        hm->entries[i] = newEntry();
        if (hm->entries[i] == NULL){
            free(hm);
            return NULL;
        }
    }
    return hm;
}

Entry *newEntry(){
    Entry *new_entry = calloc(sizeof(Entry),1);
    if (new_entry == NULL){
        return NULL;
    }
    new_entry->key = NULL;
    new_entry->value = NULL;
    new_entry->next = NULL;
    return new_entry;
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data) {
    if(hm == NULL){
        return;
    }
    for (size_t i = 0; i < hm->num_buckets; i++) {
        Entry *entry = hm->entries[i];
        if (entry->key != NULL) {
            while(entry != NULL){
                Entry *next_entry = entry->next;
                if(destroy_data != NULL){
                    destroy_data(entry->value);
                }
                free(entry->key);
                free(entry);
                entry = next_entry;
            }

        }
        free(entry);
    }
    free(hm->entries);
    free(hm);
}

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision ) {
    if(hm == NULL || key == NULL || resolve_collision == NULL){
        return;
    }
    Entry *entry = hm->entries[hm->hash(key) % hm->num_buckets];

    char* key_copy = calloc(sizeof(char), (strlen(key) + 1));
    if(key_copy == NULL){
        return;
    }
    strcpy(key_copy, key);

    if(entry->key == NULL){
        //check if the list is empty
        entry->key = key_copy;
        entry->value = data;
        hm->size++;
        return;
    }
    //check if key already exists in the list
    while(entry->next != NULL ){
        if(strcmp(entry->key,key) == 0) {
            entry->value = resolve_collision(entry->value, data);
            free(key_copy);
            return;
        }
        entry = entry->next;
    }
    //create new entry
    Entry *new_entry = newEntry();
    if(new_entry == NULL){
        free(key_copy);
        return;
    }
    new_entry->key = key_copy;
    new_entry->value = data;
    entry->next = new_entry;
    hm->size++;
}

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data) {
    if(hm == NULL || key == NULL){
        return;
    }
    unsigned int hash_key = hm->hash(key) % hm->num_buckets;
    Entry *entry = hm->entries[hash_key];

    if (entry->key == NULL) {
        return;
    }

    Entry *prev_entry = NULL;
    while (entry->next != NULL && strcmp(entry->key, key) != 0) {
        prev_entry = entry;
        entry = entry->next;
    }
    //Found correct entry
    if (prev_entry == NULL) {
        if(entry->next == NULL){
            //Only element in list
            free(entry->key);
            entry->key = NULL;
            if (destroy_data != NULL) {
                destroy_data(entry->value);
            }else{
                entry->value = NULL;
            }
            hm->size--;
            return;
        }else{
            //First element in list
            hm->entries[hash_key] = entry->next;
            entry->next = NULL;
        }
    } else {
        //In de midde
        prev_entry->next = entry->next;
    }

    if (destroy_data != NULL) {
        destroy_data(entry->value);
    }
    free(entry->key);
    free(entry);
    hm->size--;
}

void *get_data(HashMap *hm, char *key){
    if(hm == NULL || key == NULL){
        return NULL;
    }
    unsigned int hash_key = hm->hash(key) % hm->num_buckets;
    Entry *entry = hm->entries[hash_key];
    if(entry->key == NULL){
        return NULL;
    }

    while(entry->key != NULL){
        if(strcmp(entry->key,key) == 0){
            return entry->value;
        }
        entry = entry->next;
        if (entry == NULL) {
            return NULL;
        }
    }
    return NULL;
}

void iterate(HashMap *hm, void (*callback)(char *key, void *data)){
    if(hm == NULL){
        return;
    }
    for(size_t i = 0; i < hm->num_buckets; i++){
        Entry *entry = hm->entries[i];
        if(entry->key != NULL){
            while(entry != NULL){
                callback(entry->key,entry->value);
                entry = entry->next;
            }
        }
    }
}

unsigned int hash(char *key){
    unsigned int hash = 0;
    while (*key != '\0') {
        hash += *key;
        key++;
    }
    return hash;
}

unsigned int hashPlusOne(char *key){
    return hash(key) + 1;
}

void set_hash_function(HashMap *hm, unsigned int (*hash_function)(char *key)){
    if(hm == NULL || hash_function == NULL){
        return;
    }
    hm->hash = hash_function;
    if(hm->size == 0){
        return;
    }

    HashMap *new_hm = create_hashmap(hm->num_buckets);
    new_hm->hash = hm->hash;
    for(size_t i = 0; i < hm->num_buckets; i++){
        Entry *entry = hm->entries[i];
        if(entry->key != NULL){
            while(entry != NULL){
                insert_data(new_hm,entry->key,entry->value,overWriteCallback);
                entry = entry->next;
            }
        }
    }
    Entry** old_entries = hm->entries;
    hm->entries = new_hm->entries;
    new_hm->entries = old_entries;
    delete_hashmap(new_hm, NULL);
}

void* dontOverWriteCallback(void *old_data, void *new_data){
    return old_data;
}

void* overWriteCallback(void *old_data, void *new_data){
    return new_data;
}

void destroyDataCallback(void *data){
    if(data != NULL){
        free(data);
    }
}
