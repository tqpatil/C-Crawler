#include "set.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
set_t* set_new(void){
        set_t *returner;
        returner = malloc(sizeof(set_t));
        if (returner == NULL){
                printf("An error occurred\n");
                return NULL;
        }
        returner->head = NULL;
        return returner;
}

bool set_insert(set_t *set, const char *key, void *item){
        if (set == NULL || key == NULL || item == NULL) {
                return false;
        }
        node *curr = set->head;
        while (curr != NULL){
                if (strcmp(curr->key, key) == 0){
                        return false;
                }
                curr = curr->next;
        }
        node *new = (node *) malloc(sizeof(node));
        new->key = strdup(key);
        if (new->key == NULL){
                free(new);
                return false;
        }
        new->item = item;
        new->next = set->head;
        set->head = new;
        return true;
}

void *set_find(set_t *set, const char *key){
        if (set == NULL || key == NULL){
                return NULL;
        }
        node *curr = set->head;
        while (curr != NULL){
                if (strcmp(curr->key, key) == 0){
                        return curr->item;
                }
                curr = curr->next;
        }
        return NULL;
}

void set_print(set_t *set, FILE *fp,void (*itemprint)(FILE *fp, const char *key, void *item)){
        if (fp == NULL){
                return;
        }
        if (set == NULL){
                fprintf(fp, "(null)");
                return;
        }
        if (itemprint == NULL){
                fprintf(fp, "null : null");
                return;
        }
        node *curr = set->head;
        while(curr != NULL){
                if (curr->key != NULL && curr->item != NULL){
                        itemprint(fp, curr->key, curr->item);
                }
                curr=curr->next;
        }

}
void set_iterate(set_t *set, void *arg, void (*itemfunc)(void *arg, const char *key, void *item)){
        if (set == NULL){
                return;
        }
        if (itemfunc == NULL){
                return;
        }
        node *curr = set->head;
        while (curr != NULL){
                if (curr->key != NULL && curr->item != NULL){
                        itemfunc(arg, curr->key, curr->item);
                }
                curr = curr->next;
        }
}
void set_delete(set_t *set, void (*itemdelete)(void *item)){
        if (set == NULL) {
                return;
        }
        bool flag;
        if (itemdelete != NULL) {
                flag = true;
        }
        else{
                flag = false;
        }

        node *curn = set->head;
        node *nxt = set->head;
        while (curn != NULL) {
                nxt = curn->next;
                if (flag) {
                        itemdelete(curn->item);
                }
                free(curn->key);
                free(curn);
                curn = nxt;
        }

        free(set);
}
