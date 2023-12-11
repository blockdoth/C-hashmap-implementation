#include "solution.h"

#include "gest.h"
#include "gest-run.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



void hashTest() {
    assert_int_equals(hash("a"), 97);
    assert_int_equals(hash("A"), 65);
    assert_int_equals(hash("B"), 66);
    assert_int_equals(hash("aAB"), 228);
    assert_int_equals(hash("BAa"), 228);
    assert_int_equals(hash("abcdefghijklmopqrstuvwxyz"), 2737);
}

void createHashMapTest() {
    size_t key_space = 10000;
    HashMap *hm = create_hashmap(key_space);
    assert_int_equals(hm->num_buckets, key_space);
    assert_int_equals(hm->size, 0);
    assert_int_equals(memSize(hm), sizeof(HashMap) + sizeof(Entry) * key_space);
    delete_hashmap(hm, NULL);
}


void insertGetTest() {
    HashMap *hm = create_hashmap(100);
    insert_data(hm, "a", "b", NULL);
    assert_str_equals(get_data(hm, "a"), "b");

    insert_data(hm, "a", "c", NULL);
    assert_str_equals(get_data(hm, "a"), "b");
    insert_data(hm, "b", "a", NULL);
    assert_str_equals(get_data(hm, "b"), "a");
    assert_str_equals(get_data(hm, "a"), "b");
    delete_hashmap(hm, NULL);
}



void removeDataTest() {
    HashMap *hm = create_hashmap(100);
    int prev_mem_size = memSize(hm);
    remove_data(hm, "a", NULL);
    assert_int_equals(hm->size, 0);
    assert_ptr_equals(get_data(hm, "a"), NULL);
    insert_data(hm, "a", "b", NULL);
    remove_data(hm, "a", NULL);
    assert_int_equals(hm->size, 0);
    assert_ptr_equals(get_data(hm, "a"), NULL);
    assert_int_equals(memSize(hm), prev_mem_size);
    insert_data(hm, "b", "c", NULL);
    insert_data(hm, "a", "b", NULL);
    assert_int_equals(hm->size, 2);
    assert_ptr_equals(get_data(hm, "b"), "c");
    remove_data(hm, "b", NULL);
    assert_int_equals(hm->size, 1);
    remove_data(hm, "a", NULL);
    remove_data(hm, "a", NULL);
    assert_int_equals(hm->size, 0);


    delete_hashmap(hm, NULL);
}

int global_iterator_counter = 0;

void silentCallback(char *key, void *data){
    global_iterator_counter += *(char *) data;

}

void iterateTest(){
    HashMap *hm = create_hashmap(100);
    insert_data(hm, "a", "1", NULL);
    insert_data(hm, "b", "2", NULL);
    insert_data(hm, "c", "3", NULL);
    global_iterator_counter = 0;
    iterate(hm, silentCallback);
    assert_int_equals(global_iterator_counter, 150);
    delete_hashmap(hm, NULL);
}

void checkCollisionTest(){
    HashMap *hm = create_hashmap(100);
    insert_data(hm, "2222", "test", NULL);
    assert_str_equals(get_data(hm, "2222"), "test");
    insert_data(hm, "dd", "test", NULL);
    insert_data(hm, "dda", "test", NULL);
    insert_data(hm, "xx", "test", NULL);
    insert_data(hm, "<<<<", "test", NULL);
    insert_data(hm, "PPP", "test", NULL);

    delete_hashmap(hm, NULL);
}

void insertLargeKeysTest(){
    int size = 1000;
    HashMap *hm = create_hashmap(size);

    char** keys = malloc(sizeof(char*) * size);
    for (int i = 0; i < size; ++i) {
        int maxIntLength = snprintf(NULL, 0, "%d", i)+1;
        keys[i] = (char *)malloc(sizeof(char) * maxIntLength);
        sprintf(keys[i], "%d", i);
    }
    for (int i = 0; i < size; ++i) {
        insert_data(hm, keys[i] , keys[i], NULL);
    }
    assert_int_equals(hm->size, size);
    for (int i = 0; i < size; ++i) {
        assert_str_equals(get_data(hm, keys[i]), keys[i]);
        remove_data(hm, keys[i], NULL);
        free(keys[i]);
    }
    assert_int_equals(hm->size, 0);
    free(keys);
    delete_hashmap(hm, NULL);
}


void manyKeysSmallMapTest(){
    int hm_size = 10;
    int keys_count = 10000;
    HashMap *hm = create_hashmap(hm_size);

    char** keys = malloc(sizeof(char*) * keys_count);
    for (int i = 0; i < keys_count; ++i) {
        int maxIntLength = snprintf(NULL, 0, "%d", i)+1;
        keys[i] = malloc(sizeof(char) * maxIntLength);
        sprintf(keys[i], "%d", i);
    }
    for (int i = 0; i < keys_count; ++i) {
        insert_data(hm, keys[i] , keys[i], NULL);
    }
    assert_int_equals(hm->size, keys_count);
    for (int i = 0; i < keys_count; ++i) {
        assert_str_equals(get_data(hm, keys[i]), keys[i]);
        remove_data(hm, keys[i], NULL);
        free(keys[i]);
    }
    assert_int_equals(hm->size, 0);
    free(keys);
    delete_hashmap(hm, NULL);
}


void nullDataTest(){
    HashMap *hm = create_hashmap(100);

    insert_data(hm, "a", NULL, NULL);
    assert_ptr_equals(get_data(hm, "a"), NULL);
    delete_hashmap(hm, NULL);
}


void destroyDataCallbackTest(){
    int size = 1000;
    HashMap *hm = create_hashmap(size);

    char** keys = malloc(sizeof(char*) * size);
    for (int i = 0; i < size; ++i) {
        int maxIntLength = snprintf(NULL, 0, "%d", i)+1;
        keys[i] = (char *)malloc(sizeof(char) * maxIntLength);
        sprintf(keys[i], "%d", i);
    }
    for (int i = 0; i < size; ++i) {
        insert_data(hm, keys[i] , keys[i], NULL);
    }
    assert_int_equals(hm->size, size);
    for (int i = 0; i < size; ++i) {
        assert_str_equals(get_data(hm, keys[i]), keys[i]);
        remove_data(hm, keys[i], destroyDataCallback);
    }
    assert_int_equals(hm->size, 0);
    free(keys);
    delete_hashmap(hm, NULL);
}



void resolveCollisionCallbackTest(){
    HashMap *hm = create_hashmap(100);
    insert_data(hm, "a", "1", dontOverWriteCallback);
    insert_data(hm, "a", "2", dontOverWriteCallback);
    insert_data(hm, "a", "3", dontOverWriteCallback);
    insert_data(hm, "a", "4", dontOverWriteCallback);
    insert_data(hm, "a", "5", dontOverWriteCallback);
    assert_str_equals(get_data(hm, "a"), "1");
    delete_hashmap(hm, NULL);

}

void countTest(){
    FILE *file = fopen("../count.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
    }

    count_words(file);

    fclose(file);
}

void checkDuplicatedKey() {
    size_t key_space = 100;
    HashMap *hm = create_hashmap(key_space);
    char* key = malloc(strlen("test") + 1);
    strcpy(key, "test");

    insert_data(hm, key, "b", NULL);
    free(key);
    assert_str_equals(get_data(hm, "test"), "b");
    delete_hashmap(hm, NULL);
}

unsigned int hashPlus1(char *key){
    return hash(key) + 1;
}

void rehashTest(){
    int map_size = 5000;
    int key_count = 1000;
    HashMap *hm = create_hashmap(map_size);

    char** keys = malloc(sizeof(char*) * key_count);
    for (int i = 0; i < key_count; ++i) {
        int maxIntLength = snprintf(NULL, 0, "%d", i)+1;
        keys[i] = (char *)malloc(sizeof(char) * maxIntLength);
        sprintf(keys[i], "%d", i);
    }
    for (int i = 0; i < key_count; ++i) {
        insert_data(hm, keys[i] , keys[i], NULL);
        assert_str_equals(get_data(hm, keys[i]), keys[i]);
    }

    set_hash_function(hm, hashPlus1);

    for (int i = 0; i < key_count; ++i) {
        assert_str_equals(get_data(hm, keys[i]), keys[i]);
        remove_data(hm, keys[i], destroyDataCallback);
    }

    free(keys);
    delete_hashmap(hm, NULL);
}

void mutabilityTest() {
    HashMap *hm = create_hashmap(100);
    char key[4];
    char value[2];

    strcpy(key, "key");
    strcpy(value, "1");

    insert_data(hm,key, value, NULL);
    assert_str_equals((char*)get_data(hm, "key"), "1");

    strcpy(value, "2");
    assert_str_equals(get_data(hm, "key"), "2");

    strcpy(key, "kay");
    assert_str_equals(get_data(hm, "key"), "2");
    assert_ptr_equals(get_data(hm, "kay"), NULL);


    delete_hashmap(hm, NULL);
}

void deepTest() {
    size_t key_count = 1000;

    HashMap *hm = create_hashmap(key_count);
    insert_data(hm, "key", "1", NULL);
    insert_data(hm, "yek", "2", NULL);
    insert_data(hm, "kay", "3", NULL);

    size_t hash1 = hm->hash("key");
    size_t hash2 = hm->hash("yek"); // same as "key"
    size_t hash3 = hm->hash("kay");
    for(size_t i = 0; i < key_count;i++){
        Entry *entry = hm->entries[i];
        if(i == hash1){
            assert_str_equals(entry->value, "1");
        }else if (i == hash2){
            assert_str_equals(entry->next->value, "2");
        }else if (i == hash3){
            assert_str_equals(entry->value, "3");
        }else{
            assert_ptr_equals(entry->key, NULL);
        }
    }
    remove_data(hm, "key", NULL);
    for(size_t i = 0; i < key_count;i++){
        Entry *entry = hm->entries[i];
        if(i == hash1){
            assert_str_equals(entry->value, "2");
        }else if (i == hash2){
            assert_ptr_equals(entry->next, NULL);
        }else if (i == hash3){
            assert_str_equals(entry->value, "3");
        }else{
            assert_ptr_equals(entry->key, NULL);
        }
    }
    remove_data(hm, "kay", NULL);
    for(size_t i = 0; i < key_count;i++){
        Entry *entry = hm->entries[i];
        if(i == hash1){
            assert_str_equals(entry->value, "2");
        }else if (i == hash2){
            assert_ptr_equals(entry->next, NULL);
        }else if (i == hash3){
            assert_ptr_equals(entry->value, NULL);
        }else{
            assert_ptr_equals(entry->key, NULL);
        }
    }
    delete_hashmap(hm, NULL);
}


void deeperTest() {
    size_t key_count = 1000;

    HashMap *hm = create_hashmap(key_count);
    insert_data(hm, "key", "1", NULL);
    insert_data(hm, "yek", "2", NULL);
    insert_data(hm, "kay", "3", NULL);

    assert_str_equals(get_data(hm, "key"), "1");
    assert_str_equals(get_data(hm, "yek"), "2");
    assert_str_equals(get_data(hm, "kay"), "3");

    remove_data(hm, "koy", NULL);

    assert_str_equals(get_data(hm, "key"), "1");
    assert_str_equals(get_data(hm, "yek"), "2");
    assert_str_equals(get_data(hm, "kay"), "3");

    remove_data(hm, "key", NULL);

    assert_ptr_equals(get_data(hm, "key"), NULL);
    assert_str_equals(get_data(hm, "yek"), "2");
    assert_str_equals(get_data(hm, "kay"), "3");

    remove_data(hm, "yek", NULL);

    assert_ptr_equals(get_data(hm, "key"), NULL);
    assert_ptr_equals(get_data(hm, "yek"), NULL);
    assert_str_equals(get_data(hm, "kay"), "3");

    remove_data(hm, "kay", NULL);

    assert_ptr_equals(get_data(hm, "key"), NULL);
    assert_ptr_equals(get_data(hm, "yek"), NULL);
    assert_ptr_equals(get_data(hm, "kay"), NULL);

    delete_hashmap(hm, NULL);


}



void callBackTest() {
    HashMap *hm = create_hashmap(100);

    insert_data(hm,"key", "1", NULL);
    assert_str_equals(get_data(hm, "key"), "1");

    insert_data(hm,"key", "2", dontOverWriteCallback);
    assert_str_equals(get_data(hm, "key"), "1");

    insert_data(hm,"key", "2", overWriteCallback);
    assert_str_equals(get_data(hm, "key"), "2");

    delete_hashmap(hm, NULL);
}


/* Register all test cases. */
void register_tests() {
    register_test(hashTest);
    register_test(createHashMapTest);
    register_test(insertGetTest);
    register_test(removeDataTest);
    register_test(checkCollisionTest);
    register_test(insertLargeKeysTest);
    register_test(manyKeysSmallMapTest);
    register_test(iterateTest);
    register_test(nullDataTest);
    register_test(destroyDataCallbackTest);
    register_test(resolveCollisionCallbackTest);
    register_test(countTest);
    register_test(checkDuplicatedKey);
    register_test(rehashTest);
    register_test(mutabilityTest);
    register_test(deepTest);
    register_test(deeperTest);
    register_test(callBackTest);



}



