#include "template.h"

extern int addTplVar(const char *varName, const char *varValue){
	tplVar *newElement = NULL;
	size_t nameSize, valueSize;
	nameSize = (strlen(varName) + 1) * sizeof(char);
	valueSize = (strlen(varValue) + 1) * sizeof(char);
	if(tplVarRoot == NULL){
		if(initTplList() != 0){
			fprintf(stderr, "Could not initialize the template variable list.");
			return 1;
		}
		newElement = tplVarRoot;
	}else{
		newElement = tplVarRoot;
		while(newElement->next != NULL) newElement = newElement->next;
		newElement = newElement->next = (tplVar*)malloc(sizeof(tplVar));
		if(newElement == NULL){
			fprintf(stderr, "Could not allocate enought memory to save the template variable.");
			return 1;
		}
		newElement->next = NULL;
	}
	newElement->varName = (char*)malloc(nameSize * sizeof(char));
	if(newElement->varName == NULL){
		fprintf(stderr, "Could not allocate enought memory to save the template variable.");
		return 1;
	}
	newElement->varValue = (char*)malloc(valueSize * sizeof(char));
	if(newElement->varValue == NULL){
		free(newElement->varName);
		newElement->varName = NULL;
		fprintf(stderr, "Could not allocate enought memory to save the template variable.");
		return 1;
	}
	strncpy(newElement->varName, varName, nameSize);
	strncpy(newElement->varValue, varValue, valueSize);
	newElement->varName[nameSize-1] = '\0';
	newElement->varValue[valueSize-1] = '\0';
	return 0;
}

int initTplList(void){
	if(tplVarRoot != NULL){
		fprintf(stderr, "Variable list alreade initialized thought.");
		return 1;
	}
	tplVarRoot = (tplVar*)malloc(sizeof(tplVar));
	if(tplVarRoot == NULL){
		fprintf(stderr, "Could not allocate enought memory to save the template variable.");
		return 1;
	}
	tplVarRoot->next = NULL;
	return 0;
}

int displayTpl(char *filename){
	FILE *tpl;
	char puffer[TPL_BUFFER];
	char rePuffer[TPL_BUFFER];
	char replaceVar[REPLACE_BUFFER];
	int pufferlen, i, j, countBackSlash;
	int pufferCount = 0;
	tpl = fopen(filename, "r");
	if(tpl == NULL){
		perror("Could not open template file");
		return 1;
	}
	while(!feof(tpl)){
		fgets(puffer, TPL_BUFFER, tpl);
		puffer[TPL_BUFFER-1] = '\0';
		pufferlen = strlen(puffer);
		countBackSlash = 0;
		for(i = 0; i < pufferlen; i++){
			if(puffer[i] == '{'){
				if(puffer[i+1] == '$'){
					if(countBackSlash % 2 != 0)
						continue;
					for(j = 2; j < pufferlen - i; j++){
						if(puffer[i+j] == '}' || puffer[i+j] == '\0'){
							break;
						}
						replaceVar[j-2] = puffer[i+j];
						replaceVar[j-1] = '\0';
					}
					inputVar(rePuffer, &pufferCount, replaceVar);
					i = i+j;
				}
			}else if(puffer[i] == '\\'){
				countBackSlash++;
			}else{
				for(; countBackSlash > 0; countBackSlash--){
					addRePuffer(rePuffer, &pufferCount, '\\');
				}
				addRePuffer(rePuffer, &pufferCount, puffer[i]);
			}
		}
		printf("%s\n", rePuffer);
		rePuffer[0] = '\0';
		pufferCount = 0;
	}
	fclose(tpl);
}

int addRePuffer(char *puffer, int *count, char sign){
	if(*count >= TPL_BUFFER - 1)
		return 1;
	puffer[*count] = sign;
	(*count)++;
	puffer[*count] = '\0';
	return 0;
}

int inputVar(char *puffer, int *count, const char *varName){
	char *searchPuffer;
	int pufferlen = 0, i;
	searchPuffer = searchTplVar(varName);
	if(searchPuffer == NULL)
		return 1;
	pufferlen = strlen(searchPuffer);
	for(i = 0; i < pufferlen; i++){
		addRePuffer(puffer, count, searchPuffer[i]);
	}
	return 0;
}

char *searchTplVar(const char *varName){
	tplVar *var = tplVarRoot;
	while(var != NULL){
		if(strcmp(var->varName, varName) == 0)
			return var->varValue;
		var = var->next;
	}
	return NULL;
}
