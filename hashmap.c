#include "hashmap.h"

/*
** UTILITY FUNCTIONS
*/

map_node_t * create_after(map_node_t * node,
                          unsigned int hash)
{
     map_node_t * new = (map_node_t *) malloc(sizeof(map_node_t));
     if (!new)
          return NULL;
     new->hash = hash;
     new->list = (node_t *) malloc(sizeof(node_t));
     if (!new->list) {
          free(new);
          return NULL;
     }
     new->list->next = NULL;   // first inserted element serves as a guard
     new->list->value = NULL;

     new->next = node->next;
     node->next = new;
     return new;
}

node_t * insert_into(map_node_t * node,
                     void * value)
{
     node_t * new = (node_t *) malloc(sizeof(node_t));
     if (!new)
          return NULL;

     new->next = node->list;
     node->list = new;
     new->value = value;

     return new;
}

map_node_t * find_hash(hashmap_t * hm,
                       void * key)
{
     unsigned int hash = (*hm->hash_func)(key);

     map_node_t * ptr = hm->map;
     while (ptr->next->hash <= hash) {
          ptr++;
          if (ptr->next == NULL)
               return NULL;
     }

     if (ptr->hash == hash)
          return ptr;

     return NULL;
}

void * pop(map_node_t * mpnode)
{
     node_t * node = mpnode->list;
     if (!node->next)
          return NULL;

     void * rv = node->value;
     mpnode->list = node->next;
     free(node);
     return rv;
}

void del_mpnode(hashmap_t * hm,
                map_node_t * mpnode)
{
     // pop all nodes with the same hash as key
     while (pop(mpnode));

     map_node_t * prev = hm->map;

     while (prev->next != mpnode)
          prev++;

     prev->next = mpnode->next;
     free(mpnode);
     return;
}


/*
** INTERFACE
*/

hashmap_t * create_map(size_t item_size,
                       unsigned int (*hash_func)(void *),
                       int (*print_func)(void *))
{
     hashmap_t * rv = (hashmap_t *) malloc(sizeof(hashmap_t));
     if (!rv)
          return NULL;
     rv->item_size = item_size;
     rv->length = 0;
     rv->hash_func = hash_func;
     rv->print_func = print_func;
     rv->map = (map_node_t *) malloc(sizeof(map_node_t));
     if (!rv->map) {
          free(rv);
          return NULL;
     }
     map_node_t * guard = (map_node_t *) malloc(sizeof(map_node_t));
     if (!guard) {
          free(rv->map);
          free(rv);
          return NULL;
     }
     rv->map->next = guard;
     rv->map->list = NULL;
     guard->next = NULL;
     guard->list = NULL;
     return rv;
}

unsigned int add_elem(hashmap_t * hm,
                      void * key,
                      void * value)
{
     unsigned int hash = (*hm->hash_func)(key);

     map_node_t * ptr = hm->map;
     while (ptr->next->hash <= hash && ptr->next->next)
          ptr++;

     if (ptr->hash != hash) {
          ptr = create_after(ptr, hash);
     }

     insert_into(ptr, value);

     return hash;
}


void * get_first(hashmap_t * hm,
                 void * key)
{
     map_node_t * mpnode = find_hash(hm, key);
     if (!mpnode)
          return NULL;

     return mpnode->list->value;
}

void * pop_first(hashmap_t * hm,
                 void * key)
{
     map_node_t * mpnode = find_hash(hm, key);
     if (!mpnode)
          return NULL;

     return pop(mpnode);
}

void del_hash(hashmap_t * hm,
              void * key)
{
     map_node_t * mpnode = find_hash(hm, key);
     del_mpnode(hm, mpnode);
     return;
}

void del_map(hashmap_t * hm)
{
     while(hm->map->next)
          del_mpnode(hm, hm->map);

     del_mpnode(hm, hm->map);

     free(hm);
     return;
}

void print_map(hashmap_t * hm,
               FILE * stream)
{
     map_node_t * ptr = hm->map->next;
     while (ptr->next) {
          fprintf(stream, "%ud:\n", ptr->hash);
          node_t * elem = ptr->list;
          while (elem->next) {
               char * buffer = (*hm->print_func)(elem->value);
               fprintf(stream, "\t%s\n", buffer);
          }
     }
     return;
}
