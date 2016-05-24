#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_MEMORY 16
int main(int argc, char * argv[]){
	//arugment checks
	 if (argc != 3){
		fprintf(stderr, "Invalid number of arguments\n");
	 	return EXIT_FAILURE;
	 }
	//read in the file and allocate dynamic memories
	FILE *file;
	file = fopen(argv[1], "r");
	int init = INITIAL_MEMORY;
	char** words = (char **)calloc(init, sizeof(char*));
	if (words == NULL){
		fprintf(stderr, "allocation failed\n");
		return EXIT_FAILURE;
	}
	printf("Allocated initial array of %d character pointers.\n", init);
	int i,j,end;
	//if the file is opened successful
		//-reallocate space when the memory runs out 
		//-read in the file
	if (file){
		i = 0;
		end = 0;
		while ( i < init && end == 0 ){
			words[i] = (char*)malloc(16*sizeof(char));
			if (fscanf(file, "%s", words[i])==EOF){
				end = 1;
			}
			i++;
			if (i == init && end == 0){
				init = init*2;
				words = (char **)realloc(words, init*sizeof(char *));
				printf( "Re-allocated array of %d character pointers.\n", init );
			}
		}
		printf("All done (successfully read %d words).\n",i-1);
		fclose(file);
	}
	else{
		fprintf(stderr, "File is failed to open\n");
		return EXIT_FAILURE;
	}
	printf("Words containing substring \"%s\" are:\n", argv[2]);
	//check for the substring
	char *pch;
	for ( j = 0 ; j < i ; j++ ){
	  	pch = strstr(words[j], argv[2]);
		if (pch){
			printf( "%s\n", words[j] );
		}
	}
	//free memories

	for(j=0;j<i;j++){
		free(words[j]);
	}
  	free(words);
	
	return EXIT_SUCCESS;
}
