#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define INITIAL_MEMORY 16

typedef struct {
	char *word;
	char *fileName;
} Word;

typedef struct {
	char *filePath;
	char *fileName;
	Word *wordArray;
} ThreadArgs;


// Global Vars
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condLock = PTHREAD_COND_INITIALIZER;
int wordCount = 0;
int isResizing = 0; // Used to determine if array is currently resizing
int arraySize = INITIAL_MEMORY;
Word *wordArray;

void *whattodo(void *arg){
	ThreadArgs *args;
	args = (ThreadArgs *)arg;
	
	FILE *file;
	file = fopen(args->filePath, "r");
	if (file){
		int index;
		char* word = malloc(256 * sizeof(char));
		while(fscanf(file, "%s", word) != EOF){
		    while(isResizing == 1){
		        pthread_mutex_lock(&lock);
		        pthread_cond_wait(&condLock, &lock);
				pthread_mutex_unlock(&lock); // Release the lock
		    }
			pthread_mutex_lock(&lock); // Lock the index counter to synchronize
			index = wordCount;
			wordCount++;
			pthread_mutex_unlock(&lock); // Release the lock
			
			Word *singleWord = malloc(sizeof(Word)); // Create a word struct to hold word
			singleWord->word = malloc(256 * sizeof(char));
			strcpy(singleWord->word, word);
			singleWord->fileName = args->fileName;
			
			if (wordCount > arraySize){
			    pthread_mutex_lock(&lock);
				arraySize *= 2;
				wordArray = (Word*)realloc(wordArray, arraySize * sizeof(Word));
			    isResizing = 0;
				if (wordArray){
    				printf("THREAD %u: Re-allocated array of %d character pointers\n", (int)pthread_self(), arraySize);
    				wordArray[index] = *singleWord;
				}
				else{
				    pthread_cond_signal(&condLock);
				    pthread_mutex_unlock(&lock); // Release the lock
					fprintf(stderr, "Could not resize array to %d character pointers\n", arraySize);
					pthread_exit(NULL);
				}
			    pthread_cond_signal(&condLock);
				pthread_mutex_unlock(&lock); // Release the lock
			}
			else{
				wordArray[index] = *singleWord;
			}
			printf("THREAD %u: Added '%s' at index %d\n", (int)pthread_self(), wordArray[index].word, index);
			free(singleWord); // Free the word struct created
		}
		fclose(file);
		free(word);
	}
	else{
		fprintf(stderr, "File failed to open\n");
		pthread_exit(NULL);
	}
	
    pthread_cond_signal(&condLock);
    pthread_mutex_unlock(&lock); // Release the lock
	pthread_exit(NULL);
}

// Assuming paths are entered relative
void findFiles(char* path, char* match) {
	struct stat buf;
	// Holds the files found in directory
	struct dirent *file;
	//Word word; //creating struct
	// Word words[n]; //[word, word]
	wordArray = calloc(INITIAL_MEMORY, sizeof(Word));
	if (wordArray == NULL){
		fprintf(stderr, "allocation failed\n");
	}
	
	int rc = stat(path, &buf);
	if (rc < 0 || !S_ISDIR(buf.st_mode)) {
		fprintf(stderr, "%s is not a directory\n", path);
		return;
	}
	else{ // Legit path
		
		int count = 0;
		int numThreads = 1024;
		char filePath[4096];
		pthread_t tid[numThreads]; // Keep list of threads
		
		DIR *dir = opendir(path);
		if (dir == NULL){
			fprintf(stderr, "Failed to open directory: %s\n", path);
			return;
		}
		
		ThreadArgs threadArgsArray[numThreads];
		printf("MAIN THREAD: Allocated initial array of %d character pointers.\n", INITIAL_MEMORY);
		// Go through the directory that was opened
		while((file = readdir(dir)) != NULL) {
			// Skip over . and .. directories
			if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
				continue;
			
			// Append filename to given path so it can be opened
			strcpy(filePath, path);
			strcat(filePath, "/");
			strcat(filePath, file->d_name);
			
			// Set argument struct to pass to threads
			threadArgsArray[count].filePath = filePath;
			threadArgsArray[count].fileName = file->d_name;
			rc = pthread_create(&tid[count], NULL, whattodo, (void*)&threadArgsArray[count]);
			if (rc != 0){
				fprintf(stderr, "Could not create thread\n");
			}
			printf("MAIN THREAD: Assigned %s to child thread %u.\n", file->d_name, (int)tid[count]);
			count++;
		}
		
	    // Wait for threads to terminate
	    int i;
	    for (i = 0 ; i < count ; i++ ) {
	        pthread_join( tid[i], NULL );
	  	}
	  	
		if (count == 0){		
			free(wordArray); // Free the whole array
	  		closedir(dir); // Close opened dir
			wordArray = NULL; // Set pointer to NULL
			printf("No files found in directory\n");
			return;
		}
	  	
		printf("MAIN THREAD: All done (successfully read %d words).\n", wordCount);
		printf("MAIN THREAD: Words containing substring \"%s\" are:\n", match);
		for (i = 0; i < wordCount; i++){
		    if (strstr(wordArray[i].word, match) != NULL){
		        printf("MAIN THREAD: Word: %s (from '%s')\n", wordArray[i].word, wordArray[i].fileName);
		    }
			free(wordArray[i].word); // Free the word to prevent memory leaks
		}
		
		free(wordArray); // Free the whole array
	  	closedir(dir); // Close opened dir
		wordArray = NULL; // Set pointer to NULL
	}

}

int main(int argc, char * argv[]){
	//arugment checks
	if (argc != 3){
		fprintf(stderr, "Invalid number of arguments\n");
	 	return EXIT_FAILURE;
	}
	 
	findFiles(argv[1], argv[2]);
	pthread_mutex_destroy(&lock);
	return EXIT_SUCCESS;
}
