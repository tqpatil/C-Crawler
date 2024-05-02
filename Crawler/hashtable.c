#include "hashtable.h"
#include "set.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
unsigned int hash(const char *str, int num_hash){
        unsigned int val = 0;
        unsigned char *p;
        for (p = (unsigned char *) str; *p != '\0'; p++){
                val = 31 * val + *p;
        }
        return val % num_hash;
}
hashtable_t *hashtable_new(const int num_slots){
        if (num_slots <= 0){
                return NULL;
        }
        hashtable_t *hash;
        hash = malloc(sizeof(hashtable_t));
        if (hash == NULL){
                printf("An error occurred\n");
                return NULL;
        }
        hash->array = malloc(num_slots * sizeof(set_t *));
        if (hash->array == NULL){
                printf("An error occurred\n");
                free(hash);
                return NULL;
        }

        hash->num_slots = num_slots;
        for (int i=0; i<num_slots; i++){
                hash->array[i] = set_new();
        }
        return hash;
}

bool hashtable_insert(hashtable_t *ht, const char *key, void *item){
        if (ht == NULL || key == NULL || item == NULL){
		return false;
        }
        unsigned int keyHash = hash(key, ht->num_slots);
        bool success = set_insert(ht->array[keyHash],key,item);
        return success;
}

void *hashtable_find(hashtable_t *ht, const char *key){
        if (ht == NULL || key == NULL){
                return NULL;
        }
        unsigned int keyHash = hash(key, ht->num_slots);
        void *found = set_find(ht->array[keyHash], key);
        return found;

}

void hashtable_print(hashtable_t *ht, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item)){
        if (fp == NULL){
                return;
        }
        if (ht == NULL){
                fprintf(fp, "(null)");
                return;
        }
        if (itemprint == NULL){
                fprintf(fp, "null : null");
        }
        for (int i=0; i<ht->num_slots; i++){
                set_print(ht->array[i], fp, itemprint);
        }

}

void hashtable_iterate(hashtable_t *ht, void *arg,void (*itemfunc)(void *arg, const char *key, void *item)){
        if (ht == NULL){
                return;
        }
        if (itemfunc == NULL){
                return;
        }
        for (int i=0; i<ht->num_slots;i++){
                set_iterate(ht->array[i], arg, itemfunc);
        }
}

void hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *item)){
        if (ht == NULL){
                return;
        }
        for (int i=0; i<ht->num_slots; i++){
                set_delete(ht->array[i], itemdelete);
        }

        free(ht->array);
        free(ht);
}

void delete_hashkey(hashtable_t *ht, const char *key){
        if (ht == NULL  || key == NULL){
                return;
        }
        int keyHash = hash(key, ht->num_slots); 
        set_t *array = ht->array[keyHash];
        if (array == NULL){
                return;
        }
        node *curr = array->head;
        node *pre = NULL;
        while(curr != NULL){
                if (strcmp(curr->key, key) == 0){
                        if (pre == NULL){
                                array->head = curr->next;
                        }
                        else{
                                pre->next = curr->next;
                        }
                        free(curr->key);
                        free(curr);
                        return;
                }
                pre=curr;
                curr = curr->next;

        }
}
