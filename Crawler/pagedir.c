#include "pagedir.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
bool pagedir_init(const char *pageDirectory) {
	int acc = access(pageDirectory, F_OK);
	if (acc != -1){
		char *Name = ".crawler";
		size_t size = (strlen(pageDirectory) + strlen(Name) + 1);
		char *Path = (char *) calloc((strlen(pageDirectory) + strlen(Name) + 1), sizeof(char));
		if (Path == NULL){
			return false;
		}
		int finalIndex = -1;
		for(size_t i=0; i < strlen(pageDirectory); i++){
			Path[i] = pageDirectory[i];
			finalIndex = i;
		}
		if (finalIndex != -1){
			if (Path[finalIndex] != '/' && Path[finalIndex] != '\\'){
				Path = realloc(Path, sizeof(char) * (size + 1));
				if (Path == NULL){
					return false;
				}
				finalIndex += 1;
				char *result = strchr(Path, '/');
				char *result2 = strchr(Path, '\\');
				if (result != NULL){
					Path[finalIndex] = '/';
				}
				else if (result2 != NULL){
					Path[finalIndex] = '\\';
				}
				else{
					Path[finalIndex] = '/';
				}
			}
			finalIndex += 1;
			int finalIndex2 = -1;
			for(size_t j=0; j < strlen(Name); j++){
				Path[finalIndex+j] = Name[j];
				finalIndex2 = finalIndex+j;
			}
			if(finalIndex2 != -1){
				Path[finalIndex2 + 1] = '\0';
			}
			else{
				free(Path);
				return false;
			}
		}
		else{
			free(Path);
			return false;
		}
		FILE* filer = fopen(Path, "w");
		if (filer != NULL){
			fclose(filer);
			free(Path);
			return true;
		}
		else{
			free(Path);
			return false;
		}
	}
	else{
		return false;
	}
	// Fill in with your implementation
}
int numDigits(int n){
	int count =0;
	if (n == 0){
		return 1;
	}
	else{
		while(n != 0){
			n = n/10;
			count += 1;
		}
		return count;
	}
}
void pagedir_save(const webpage_t *page, const char *pageDirectory, const int documentID) {
	int id_digits = numDigits((int) documentID);
	size_t size = strlen(pageDirectory) + id_digits + 2;
	char path[size];
	if (pageDirectory[strlen(pageDirectory)-1] == '/' || pageDirectory[strlen(pageDirectory)-1] == '\\'){
		snprintf(path, size, "%s%d", pageDirectory, documentID);
	}
	else{
		snprintf(path, size, "%s/%d", pageDirectory, documentID);
	}
	FILE *f = fopen(path, "w");
	if (f == NULL){
		fprintf(stderr, "Unable to create or write to file\n");
		return;
	}
	fprintf(f, "%s\n%d\n%s", page->url, page->depth, page->html);
	fclose(f);
	// Fill in with your implementation
} 
