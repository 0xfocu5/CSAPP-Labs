#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <math.h>
#include <errno.h>

typedef struct {
	char valid_bit;
	unsigned long tag;
	int LRU_count;
} Cache_line;

typedef struct {
	Cache_line* lines;
} Cache_set;

typedef struct {
	int S;
	int E;
	Cache_set* sets;
} Cache;

int hit_count=0, miss_count=0,eviction_count=0;
/*
	s -- the number of sets
	E -- the number of cache lines in one set
	b -- the size of one block in one cache line
*/
void printUsage(char* argv[]);  //print the help messages
void initCache(int s, int E, int b, Cache* cache); //init caches
void freeCache(Cache* cache); //free caches
int getHitIndex(Cache *cache, int setIndex, int tag); // if hit return the index of memory
int getEmptyIndex(Cache *cache, int setIndex, int tag); // if there is any empty memort return it's index
void load(Cache *cache, int setIndex, int tag, int verbosity); //load 
void store(Cache *cache, int setIndex, int tag, int verbosity); //store
void modify(Cache *cache, int setIndex, int tag, int verbosity); //modify: once store and once load 
void replayTrace(int s, int E, int b, char* buf, int verbosity, Cache* cache);

int main(int argc, char *argv[])
{
	int s, E, b;
	char buf[100];   //store the name of the file
	int verbosity = 0;
	Cache cache;
	char ch;
	while ((ch = getopt(argc, argv, "vs:E:b:t:")) != -1) {
		switch (ch) {
			case 'v':
				verbosity = 1;
				break;
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 'h':
				printUsage(argv);
				break;
			case 't':
				strcpy(buf, optarg);//copy the address of trace to file
				break;
			default:
				break;
		}
	}
	if ( !s || !E || !b ) {
		printf("%s: Missing required command line argument\n", *argv);
		printUsage(argv);
	}
	initCache(s, E, b, &cache);
	replayTrace(s, E, b, buf, verbosity, &cache);
	freeCache(&cache);
	printSummary(hit_count, miss_count, eviction_count);
	return 0;
}

void printUsage(char* argv[]) {
	printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", *argv);
	puts("Options:");
	puts("  -h         Print this help message.");
	puts("  -v         Optional verbose flag.");
	puts("  -s <num>   Number of set index bits.");
	puts("  -E <num>   Number of lines per set.");
	puts("  -b <num>   Number of block offset bits.");
	puts("  -t <file>  Trace file.");
	puts("\nExamples:");
	printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", *argv);
	printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", *argv);
	exit(0);
}

void initCache(int s, int E, int b, Cache* cache) 
{
	cache->S = pow(2.0, s); // get the sets
	cache->E = E;
	cache->sets = (Cache_set*)malloc(cache->S * sizeof(Cache_set));

	int i, j;
	for (i = 0; i < cache->S; i++) {//init every cache line
		cache->sets[i].lines = (Cache_line*)malloc(E * sizeof(Cache_line));
		for (j = 0; j < cache->E; j++) 
		{//init every cache line
			cache->sets[i].lines[j].valid_bit = 0;
			cache->sets[i].lines[j].LRU_count = 0;
		}
	}
	return;
}

void freeCache(Cache* cache) {
	for (int i = 0; i < cache->S; i++) {//init every cache line
		free(cache->sets[i].lines); 
		cache->sets[i].lines = 0;
	}
	free(cache->sets);
	cache->sets = 0;
}
int getHitIndex(Cache *cache, int setIndex, int tag){ //whether there is a hit
    int hitIndex = -1;
    for (int i = 0; i < cache->E; i++) {
        if (cache->sets[setIndex].lines[i].valid_bit == 1 && cache->sets[setIndex].lines[i].tag == tag){ // valid and the tag matches
            hitIndex = i;
            break;
        }
    }
    return hitIndex;
}

int getEmptyIndex(Cache *cache, int setIndex, int tag){//find whether there is an empty line in the given set
    int i;
    int emptyIndex = -1;
    for (i = 0; i < cache->E; ++i) {
        if (cache->sets[setIndex].lines[i].valid_bit == 0) {
            emptyIndex = i;
            break;
        }
    }
    return emptyIndex;
}
void load(Cache *cache, int setIndex, int tag, int verbosity) {
	int hitIndex = getHitIndex(cache, setIndex, tag);//whether there is a hit
    if (hitIndex == -1) { //one miss
        miss_count++;
        if (verbosity) {
            printf("miss ");
        } 
        int emptyIndex = getEmptyIndex(cache, setIndex, tag);     
        if (emptyIndex == -1) {//need eviction 
            eviction_count++;
            if (verbosity){
                printf("eviction ");
            }
			int flag = 1;
            for (int i = 0; i < cache->E; i++){
                if (cache->sets[setIndex].lines[i].LRU_count == cache->E - 1 && flag==1){ //find the least recent used line, and other line LRU_count++
                    cache->sets[setIndex].lines[i].valid_bit = 1;
                    cache->sets[setIndex].lines[i].LRU_count = 0;
                    cache->sets[setIndex].lines[i].tag = tag;
                    flag = 0;
                } 
				else{
                    cache->sets[setIndex].lines[i].LRU_count++;//it is not used this time
                }
            }
        }
		else {  // don't need eviction
            for (int i = 0; i < cache->E; i++) {
                if (i != emptyIndex){
                    cache->sets[setIndex].lines[i].LRU_count++;//it is not used this time
                } 
				else {
                    cache->sets[setIndex].lines[i].valid_bit = 1;
                    cache->sets[setIndex].lines[i].tag = tag;
                    cache->sets[setIndex].lines[i].LRU_count = 0;
                }
            }
        }
    } 
	else {//one hit                        
        hit_count++;
        if (verbosity){
            printf("hit ");
        }
        int tempLRU_count = cache->sets[setIndex].lines[hitIndex].LRU_count;
        for (int i = 0; i < cache->E; i++) {
            if (i != hitIndex) {
                if (cache->sets[setIndex].lines[i].LRU_count <  tempLRU_count) {//less than the hit one's LRU
                    cache->sets[setIndex].lines[i].LRU_count++;
                }
            } 
			else {
                cache->sets[setIndex].lines[i].LRU_count = 0;// the hit one's LRU is set to zero
            }
        }
    }
}

void store(Cache *cache, int setIndex, int tag, int verbosity) {//store is just like a load
    load(cache, setIndex, tag, verbosity);
}



void modify(Cache *cache, int setIndex, int tag, int verbosity) {// a write is just like one load then one store
    load(cache, setIndex, tag, verbosity);
    store(cache, setIndex, tag, verbosity);
}

void replayTrace(int s, int E, int b, char* buf, int verbosity, Cache* cache) {
	FILE *file;                        // pointer to FILE object 
	file = fopen(buf, "r");
	char type;                          // L-load S-store M-modify 
	unsigned long address;              // 64-bit  memory address 
	int size;                           //number of bytes accessed by operation 

	int tag_move_bits = b + s;

	while (fscanf(file, " %c %lx,%d", &type, &address, &size) > 0) {//for every line in the file
		if (type == 'I') {
			continue;//if it is an instruction, do nothing
		}
		else {
			int tag = address >> tag_move_bits;//get the tag
			int setIndex = (address >> b) & ((1 << s) - 1);//get the index
			if (verbosity == 1) {//print detailed info
				printf("%c %lx,%d ", type, address, size);
			}
			if (type == 'S') {
				store(cache, setIndex, tag, verbosity);
			}
			if (type == 'M') {
				modify(cache, setIndex, tag, verbosity);
			}
			if (type== 'L') {
				load(cache, setIndex, tag, verbosity);
			}
			if (verbosity == 1) {
				printf("\n");
			}
		}
	}
	fclose(file);
	return;
}


