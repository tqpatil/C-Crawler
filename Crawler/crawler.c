#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crawler.h"
#include "curl.h"
#include <stdbool.h>
#include "url.h"
#include "pagedir.h"
#include <unistd.h>
/**
 * Parses command-line arguments, placing the corresponding values into the pointer arguments seedURL,
 * pageDirectory and maxDepth. argc and argv should be passed in from the main function.
 */
static void pageScan(webpage_t *page, bag_t *pagesToCrawl, hashtable_t *pagesSeen);
static void parseArgs(const int argc, char *argv[], char **seedURL, char **pageDirectory, int *maxDepth) {
	if (argc != 4){
                fprintf(stderr, "Incorrect number of Args\n");
                exit(1);
        }
        char *ptr;
        int num = (int) strtol(argv[3], &ptr, 10);
        if (ptr == argv[3]) {
                fprintf(stderr, "Failed to read depth\n");
                exit(2);
        }
        if (!(num >= 0 && num <= 10)){
                fprintf(stderr, "Depth is not within the range\n");
                exit(3);
        }
	bool pDir = pagedir_init(argv[2]);
	if (!pDir){
		fprintf(stderr, "Error validating passed page directory\n");
		exit(5);
	}
	// Dealing with #s
	int count =0;
	while (argv[1][count] != '\0'){
		if (argv[1][count] == '#'){
			argv[1][count] = '\0';
			break;
		}
		count += 1;
	}
	char *placeholder = (char *) malloc((strlen(argv[1]) + 1)*sizeof(char));
	for (size_t i=0; i<strlen(argv[1]); i++){
		placeholder[i] = argv[1][i];
	}
	placeholder[strlen(argv[1])] = '\0';
	if ((strncmp(placeholder, "http://", 7) != 0) && (strncmp(placeholder, "https://", 8) != 0)){
		free(placeholder);
		fprintf(stderr, "Invalid Seed URL format\n");
		exit(8);
		
	}
	char *testHTML = download(placeholder, NULL);
	if (testHTML == NULL){
		free(testHTML);
		free(placeholder);
		fprintf(stderr, "Unable to access Seed URL\n");
		exit(21);
	}
	free(testHTML);
	*seedURL = placeholder;
	*pageDirectory = argv[2];
	*maxDepth = num;
}

/**
 * Crawls webpages given a seed URL, a page directory and a max depth.
 */
bool deletePage(webpage_t *page){
	if (page == NULL){
		return false;
	}
	if (page->html != NULL){
		free(page->html);
	}
	if (page->url != NULL){
		free(page->url);
	}
	if (page->next != NULL){
		if (page->prev != NULL){
			page->next->prev = page->prev;
			page->prev->next = page->next;
		}
		else{
			page->next->prev = NULL;
		}

	}
	else{
		if (page->prev != NULL){
			page->prev->next = NULL;
		}
	}
	page->next = NULL;
	page->prev = NULL;

	free(page);
	return true;
	
	// Given a webpage, delete and free it and any memory mallocd to it(html is mallocd) 
}
webpage_t *popHead(bag_t *bag){
	if (bag == NULL || bag->head == NULL){
		return NULL;
		
	}
	webpage_t *old = bag->head;
	bag->head = old->next;
	if (bag->head != NULL){
		bag->head->prev = NULL;
	}
	else{
		bag->tail= NULL;
	}
	old->next = NULL;
	bag->num_items -= 1;
	return old;
	// Stack pop: remove the Head webpage, set the next to head, and return the old head webpage
}
bool insertTail(bag_t *bag, webpage_t *page){
	if (bag == NULL || page== NULL){
		return false;
	}
	if (bag->head == NULL){
		bag->head = page;
		bag->tail = page;
	}
	else{
		bag->tail->next = page;
		page->prev = bag->tail;
		bag->tail = page;
	}
	bag->num_items += 1;
	return true;
	// Insert page into the tail of bag(if bag was empty make page the head and tail)
	// 	If bag not empty make tail, make page prev the old tail, make page next NULL, etc
	// increment num_items
}
bool freeList(bag_t *bag){
	if (bag == NULL){
		return false;
	}
	webpage_t *head = bag->head;
	webpage_t *prev = bag->head;
	while(prev != NULL){
		head = prev->next;
		free(prev->url);
		free(prev->html);
		free(prev);
		prev = head;
	}
	free(bag);
	// free all webpages left in the bag and free the bag itself/anything mallocd for the bag 
	return true;
}
static int fromAB(char *seedURL, char *targetURL) {
	hashtable_t *hashtable = hashtable_new(20);
	if (hashtable == NULL){
		fprintf(stderr, "Error creating hashtable\n");
		exit(7);
	}
	bool h_ins = hashtable_insert(hashtable, seedURL, seedURL);
	if (h_ins == false){
		hashtable_delete(hashtable,NULL);
                fprintf(stderr, "Error inserting into hashtable\n");
                exit(8);	
	}
	bag_t *bag = (bag_t *) malloc(sizeof(bag_t));
        if (bag == NULL){
                hashtable_delete(hashtable,NULL);
                fprintf(stderr, "Out of memory error\n");
                exit(1);
        }
        bag->head = NULL;
        bag->tail = NULL;
        bag->num_items = 0;
        webpage_t *seed = (webpage_t *) malloc(sizeof(webpage_t));
        if (seed == NULL){
		free(bag);
                hashtable_delete(hashtable,NULL);
                fprintf(stderr, "Out of memory error\n");
                exit(2);
        }
        seed->url = seedURL;
        seed->html = NULL;
        seed->length = -1;
        seed->depth = 0;
        seed->next = NULL;
        seed->prev = NULL;
	insertTail(bag,seed);
	int num_moves = 0;
	while (bag->num_items>0){
		webpage_t *curr = popHead(bag);
		if (strcmp(curr->url,targetURL) == 0){
			deletePage(curr);
			freeList(bag);
			hashtable_delete(hashtable,NULL);
			// deallocate my shit
			return num_moves;
		}
		char *html=download(curr->url, &curr->length);
		if (html == NULL){
			deletePage(curr);
                        fprintf(stderr, "HTML fetch failed\n");
                        continue;
                }
		num_moves+=1;
		curr->html=html;
		pageScan(curr, bag, hashtable);
		printf("pagescan successful\n");
		deletePage(curr);
		printf("delete page success\n");

	}
	printf("Impossible to get from link a to link b\n");
	freeList(bag);
	hashtable_delete(hashtable,NULL);
	return -1;
	// while bag is not empty:
	// 	pop a link from the bag, if it = target, return num moves
	// 	now, download the first webpage, set num moves +=1
	// 	parse it for links,
	// 		If link = target, return num moves
	// 		if link has been seen before, move onto the next link
	// 		if the link doesnt =target, add the link to the bag
	//	
	//
	// 


}
static void crawl(char *seedURL, char *pageDirectory, const int maxDepth) {
	hashtable_t *hashtable = hashtable_new(20);
	if (hashtable == NULL){
		fprintf(stderr, "Error creating hashtable\n");
		exit(7);
	}
	
	bool h_ins = hashtable_insert(hashtable, seedURL, seedURL);
	if (h_ins == false){
		hashtable_delete(hashtable,NULL);
		fprintf(stderr, "Error inserting into hashtable\n");
		exit(8);
	}
	bag_t *bag = (bag_t *) malloc(sizeof(bag_t));
	if (bag == NULL){
		hashtable_delete(hashtable,NULL);
		fprintf(stderr, "Out of memory error\n");
		exit(1);
	}
	bag->head = NULL;
	bag->tail = NULL;
	bag->num_items = 0;
	webpage_t *seed = (webpage_t *) malloc(sizeof(webpage_t));
	if (seed == NULL){
		free(bag);
		hashtable_delete(hashtable,NULL);
		fprintf(stderr, "Out of memory error\n");
		exit(2);
	}
	seed->url = seedURL;
	seed->html = NULL;
	seed->length = -1;
	seed->depth = 0;
	seed->next = NULL;
	seed->prev = NULL;
	insertTail(bag, seed);
	int docID = 1;
	while (bag->num_items > 0){
		sleep(1);
		webpage_t *curr = popHead(bag);
		printf("%d   Fetched: %s\n", curr->depth, curr->url);
		printf("%d  Scanning: %s\n", curr->depth, curr->url);
		char *html = download(curr->url, &curr->length);
		if (html == NULL){
			deletePage(curr);
			fprintf(stderr, "HTML fetch failed\n");
			continue; 
		}
		curr->html = html;
		pagedir_save(curr, pageDirectory, docID);
		docID += 1;
		if (curr->depth != maxDepth){
			pageScan(curr, bag, hashtable);
		}
		deletePage(curr);

	}
	hashtable_delete(hashtable, NULL);
	freeList(bag);
}

/**
 * Scans a webpage for URLs.
 */

static void pageScanTwo(webpage_t *page, bag_t *pagesToCrawl, hashtable_t *pagesSeen){
	char *haystack = page->html;
        char *whats = strstr(haystack, "<a href=\"");
        char *up;
	while(whats != NULL){
                whats += 9;
                up = strstr(whats, "\"");
                if (up == NULL){
                        break;
                }
                char url[(up - whats)+1];
                for (int k=0; k<(up-whats); k++){
                        url[k] = whats[k];
                }
                url[(up-whats)] = '\0';
                int c = 0;
                if (url != NULL){
                        while(url[c] != '\0'){
                                if (url[c] == '#'){
                                        url[c] = '\0';
					break;
                                }
                                c +=1;
                        }
                }
                char *normURL = normalizeURL(page->url, url);
		if (normURL == NULL){
                        whats = strstr(haystack, "<a href=\"");
                        haystack = up+1;
                        continue;
                }
		printf("%d     Found: %s\n", page->depth, normURL);
		bool exists = hashtable_insert(pagesSeen, normURL, normURL);
                if (exists){
                	webpage_t *newPage = (webpage_t *) malloc(sizeof(webpage_t));
                        if (newPage == NULL){
                                fprintf(stderr, "Out of memory error\n");
                                free(normURL);
                                exit(21);
                        }
                        newPage->url = normURL;
                        newPage->html = NULL;
                        newPage->length = -1;
                        newPage->depth = 0;
			newPage->next = NULL;
                	newPage->prev = NULL;
                        insertTail(pagesToCrawl, newPage);
		}
	}
}
static void pageScan(webpage_t *page, bag_t *pagesToCrawl, hashtable_t *pagesSeen) {
	char *haystack = page->html;
	char *whats = strstr(haystack, "<a href=\"");
	char *up;
	while(whats != NULL){
		whats += 9;
		up = strstr(whats, "\"");
		if (up == NULL){
			break;
		}
		char url[(up - whats)+1];
		for (int k=0; k<(up-whats); k++){
			url[k] = whats[k];
		}
		url[(up-whats)] = '\0';
		int c = 0;
		if (url != NULL){		
			while(url[c] != '\0'){
				if (url[c] == '#'){
					url[c] = '\0';
					break;
				}
				c +=1;
			}
		}
		char *normURL = normalizeURL(page->url, url);
		if (normURL == NULL){
			whats = strstr(haystack, "<a href=\"");
			haystack = up+1;
			continue;
		}
		printf("%d     Found: %s\n", page->depth, normURL);
		if (isInternalURL(page->url, normURL)){
			bool exists = hashtable_insert(pagesSeen, normURL, normURL);
			if (exists){
				webpage_t *newPage = (webpage_t *) malloc(sizeof(webpage_t));
				if (newPage == NULL){
					fprintf(stderr, "Out of memory error\n");
					free(normURL);
					exit(21);
				}
				newPage->url = normURL;
				newPage->html = NULL;
				newPage->length = -1;
				newPage->depth = page->depth +1;
				newPage->next = NULL;
				newPage->prev = NULL;
				insertTail(pagesToCrawl, newPage);
				printf("%d      Added: %s\n", page->depth, normURL);
			}
			else{
				printf("%d    IgnDupl: %s\n", page->depth, normURL);
				free(normURL);
			}
		}
		else{
			printf("%d   IgnExtrn: %s\n", page->depth, normURL);
			free(normURL);
		}

		//if url is internal ...
		whats = strstr(haystack, "<a href=\"");
		haystack = up + 1;
		
	}

}
int main(const int argc, char *argv[]) {
	//int result = fromAB(argv[1], argv[2]);
	//printf("num moves required: %d\n", result);
	char *seedURL;
	char *pageDirectory;
	int maxDepth;
	parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
	crawl(seedURL, pageDirectory, maxDepth);
	exit(0);
}
