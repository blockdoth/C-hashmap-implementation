#include <ctype.h>
#include "solution.h"
#define NEW_HASH

HashMap *create_hashmap(size_t key_space){
    HashMap *hm = calloc(1,sizeof(HashMap));
    hm->entries = calloc(key_space,sizeof(Entry*));
    set_hash_function(hm, hash);
    for(size_t i = 0; i < key_space; i++){
        hm->entries[i] = newEntry();
    }

    hm->num_buckets = key_space;
    hm->size = 0;
    return hm;
}

Entry *newEntry(){
    Entry *new_entry = calloc(1,sizeof(Entry));
    new_entry->key = NULL;
    new_entry->value = NULL;
    new_entry->next = NULL;
    return new_entry;
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data) {
    for (size_t i = 0; i < hm->num_buckets; i++) {
        Entry *entry = hm->entries[i];
        if (entry->key != NULL) {
            // Free the memory for all the entries in the list
            while(entry != NULL){
                Entry *next_entry = entry->next;
                if(destroy_data != NULL){
                    destroy_data(entry->value);
                }
                free(entry->key);
                free(entry);
                entry = next_entry;
            }
            free(entry);
        }else{
            free(entry); // Free the memory for the entry
        }
    }
    free(hm->entries);  // Free the memory for the hashmap
    free(hm);  // Free the memory for the hashmap
}

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision ) {
    unsigned int hash_key = hm->hash(key) % hm->num_buckets;
    Entry *entry = hm->entries[hash_key];
    //check if the list is empty
    char* key_copy = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(key_copy, key);
    if(entry->key == NULL){
        entry->key = key_copy;
        entry->value = data;
        hm->size++;
    }else{
        //check if key already exists in the list
        if(strcmp(entry->key,key) == 0){
            if(resolve_collision != NULL){
                resolve_collision(entry->value, data);
            }
            free(key_copy);
            return;
        }
        while(entry->next != NULL ){
            if(strcmp(entry->key,key) == 0) {
                if (resolve_collision != NULL) {
                    resolve_collision(entry->value, data);
                }else{
                    entry->value = data;
                }
                free(key_copy);
                return;
            }
            entry = entry->next;
        }
        //create new entry
        Entry *new_entry = newEntry();
        new_entry->key = key_copy;
        new_entry->value = data;
        entry->next = new_entry;
        hm->size++;
    }
}

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data) {
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
    if (prev_entry == NULL) {
        // First entry in the list
        if(entry->next == NULL){
            // Only entry in the list
            free(entry->key);
            entry->key = NULL;
            if (destroy_data != NULL) {
                destroy_data(entry->value);
            } else {
                entry->value = NULL;
            }
            hm->size--;
            return;
        }else{
            hm->entries[hash_key] = entry->next;
            entry->next = NULL;
        }
    } else {
        // Entry in the middle or end of the list
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
    }
    return NULL;
}


void iterate(HashMap *hm, void (*callback)(char *key, void *data)){
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


void set_hash_function(HashMap *hm, unsigned int (*hash_function)(char *key)){
    if(hm->size > 0){
        size_t old_size = hm->size;
        char **keys = calloc(sizeof(char*),old_size);
        void **values = calloc(sizeof(void*),old_size);
        int index = 0;
        for(size_t i = 0; i < hm->num_buckets; i++){
            Entry *entry = hm->entries[i];
            if(entry->key == NULL){
                continue;
            }
            while(entry != NULL){
                keys[index] = strcpy(malloc(sizeof(char) * (strlen(entry->key) + 1)),entry->key);
                values[index] = entry->value;
                entry = entry->next;
                remove_data(hm,keys[index],NULL);
                index++;
            }
        }

        hm->hash = hash_function;

        for(size_t i = 0; i < old_size; i++){
            insert_data(hm,keys[i],values[i],NULL);
            free(keys[i]);
        }
        free(keys);
        free(values);
    }else{
        hm->hash = hash_function;
    }
}


int memSize(HashMap *hm) {
    size_t mem_size = sizeof(HashMap);
    for (size_t i = 0; i < hm->num_buckets; i++) {
        mem_size += sizeof(Entry);
    }
    return mem_size;
}


void printCallback(char *key, void *data);
void increaseCount(void *old_data, void *new_data);


void count_words(FILE * stream){
    char word[20000];
    int c;
    HashMap *hm = create_hashmap(1000);
    char** keys = malloc(sizeof(char*) * 1000);
    int index = 0;
    while ((c = fgetc(stream)) != EOF) {
        if (isalpha(c) || isdigit(c)) {
            int i = 0;
            do {
                word[i++] = c;
                c = fgetc(stream);
            } while (isalpha(c) || isdigit(c));

            // Null-terminate the word
            word[i] = '\0';
            int* count = malloc(sizeof (int));
            *count = 1;
            char* key = malloc(sizeof (char) * (strlen(word) + 1));
            insert_data(hm, strcpy(key, word), count, increaseCount);
            keys[index++] = key;
        }
    }
    iterate(hm, printCallback);
    //printf("Total number of words: %zu\n", hm->size);
    for (int i = 0; i < index; ++i) {
        free(keys[i]);
    }

    delete_hashmap(hm, NULL);
}

void printCallback(char *key, void *data){
    printf("%s: ", key);
    printf("%d\n",  *(int*)data);
}

void increaseCount(void *old_data, void *new_data){
    int *count = (int *)old_data;
    free(new_data);
    (*count)++;
}

void destroyData(void *data){
    free(data);
}

void dontOverWriteCallback(void *old_data, void *new_data){
    //free(new_data);
}
void overWriteCallback(void *old_data, void *new_data){
//    free(old_data);
}
