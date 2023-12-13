HashMap Code
=============
This project provides a basic implementation of a hash map data structure in C. 

# Functions #
Create a new hash map with the specified key space \
`create_hashmap(size_t key_space)` \
Delete the hash map and optionally destroy data using a callback \
`delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data)`\
Insert data into the hash map \
`insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision)` \
Retrieve data associated with a key \
`get_data(HashMap *hm, char *key)`\
Remove data associated with a key \
`remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data)` \
Iterate over all key-value pairs in the hash map \
`iterate(HashMap *hm, void (*callback)(char *key, void *data))` \
Set a custom hash function for the hash map \
`set_hash_function(HashMap *hm, unsigned int (*hash_function)(char *key))`


## Callbacks ##
Function to resolve collisions when inserting data \
`ResolveCollisionCallback` \
Function to destroy data when removing or deleting an entry\
`DestroyDataCallback` 
