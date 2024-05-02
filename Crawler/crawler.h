#ifndef CRAWLER_H
#define CRAWLER_H

#include <stddef.h>
#include "hashtable.h"
typedef struct webpage webpage_t;
typedef struct bag bag_t;
typedef struct webpage {
	char *url;
	char *html;
	size_t length;
	int depth;
	webpage_t *next;
	webpage_t *prev;
} webpage_t;

typedef struct bag {
	webpage_t *head;
	webpage_t *tail;
        int num_items;
	// Fill in with your implementation
} bag_t;


#endif
