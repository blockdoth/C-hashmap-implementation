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

void destroyDataCallback(void *data){
    free(data);
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
    int map_size = 10000;
    int key_count = 2000;
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



}



