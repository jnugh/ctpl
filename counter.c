#include "counter.h"

int displayCounter(int countUp){
	FILE *counter;
	int currentn;
	counter = fopen(FCOUNTER, "r");
	if(counter == NULL){
		if(errno == ENOENT){
			counter = fopen(FCOUNTER, "w");
			if(counter == NULL){
				perror("Counter file could not be opened");
				return 0;
			}
			fputc(48, counter);
			freopen(FCOUNTER, "r", counter);
			if(counter == NULL){
				perror("Counter file could not be opened");
				return 0;
			}
		}
	}
	if(!fscanf(counter, "%i", &currentn))
		return 0;
	if(countUp){
		freopen(FCOUNTER, "w", counter);
		if(counter == NULL){
			perror("Counter file could not be opened");
			return 0;
		}
		fprintf(counter, "%i", ++currentn);
	}else
		currentn++;
	return currentn;
}
