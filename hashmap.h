#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdio.h>

typedef struct node {
     struct node * next;
     void * value;
} node_t;

typedef struct map_node {
     struct map_node * next;
     unsigned int hash;
     node_t * list;
} map_node_t;

typedef struct {
     map_node_t * map;   // last (guard) element of the map
     size_t length;      // length of the map
     size_t value_size;  // item size in bytes
     unsigned int (*hash_func)(void *);
         // user-provided hashing function
     char * (*print_func)(void *);
} hashmap_t;


hashmap_t * create_map(size_t item_size,
                       unsigned int (*hash_func)(void *),
                       int (*print_func)(void *));

unsigned int add_elem(hashmap_t * hm,  // returns hash
                      void * key,
                      void * value);

void * get_first(hashmap_t * hm,
                 void * key);

void * pop_first(hashmap_t * hm,
                 void * key);

void del_hash(hashmap_t * hm,
              void * key);

void del_map(hashmap_t * hm);

void print_map(hashmap_t * hm,
               FILE * stream);

#endif /* HASHMAP_H */
