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
	int pufferlen, i, countBackSlash;
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
					getVarName(&puffer[i+2], replaceVar, REPLACE_BUFFER * sizeof(char), &i);
					i++;
					if(inputVar(rePuffer, &pufferCount, replaceVar) == 0);
				}else{
					if(countBackSlash % 2 != 0)
						continue;
					getVarName(&puffer[i+1], replaceVar, REPLACE_BUFFER * sizeof(char), &i);
					//i++;
					callPlugin(rePuffer, &pufferCount, replaceVar);
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

void getVarName(const char *start, char *varName, size_t puffersize, int *i){
	int j;
	for(j = 0; j < puffersize - 1; j++){
		if(start[j] == '}' || start[j] == '\0'){
			break;
		}
		varName[j] = start[j];
		varName[j+1] = '\0';
	}
	*i += j + 1;
}

int callPlugin(char *puffer, int *count, char *pluginName){
	void *dynPlugin;
	char *error;
	char *(*callPlugin)(char*, size_t);
	char *pluginPath;
	int i, j;
	char pufferReturn[REPLACE_BUFFER]  = "\0";
	int pufferlen;
	pluginPath = (char*)malloc((strlen("plugins/ .so") + strlen(pluginName)) * sizeof(char));
	strcpy(pluginPath, "plugins/");
	j = strlen("plugins/");
	for(i = 0; pluginName[i] != '\0'; i++, j++){
		pluginPath[j] = pluginName[i];
	}
	strcpy(&pluginPath[j], ".so\0");
	dynPlugin = dlopen(pluginPath, RTLD_LAZY);
	free(pluginPath);
	if(dynPlugin == NULL){
		fprintf(stderr, "Faild while loading plugin: %s\n", dlerror());
		return 1;
	}
	dlerror();
	*(void**)(&callPlugin) = dlsym(dynPlugin, "callPlugin");
	if ((error = dlerror()) != NULL)  {
		fprintf(stderr, "%s\n", error);
		return 1;
	}
	(*callPlugin)(pufferReturn, REPLACE_BUFFER);
	pufferlen = strlen(pufferReturn);
	for(i = 0; i < pufferlen; i++){
		addRePuffer(puffer, count, pufferReturn[i]);
	}
	dlclose(dynPlugin);
	return 0;
}

int deleteVarList(tplVar *root){
	tplVar *i, *j;
	i = root;
	j = NULL;
	if(i == NULL)
		return 0;
	do{
		j = i->next;
		free(i->varName);
		free(i->varValue);
		free(i);
		i = j;
	}while(i->next != NULL);
	if(j != NULL){
		free(j->varName);
		free(j->varValue);
		free(j);
	}
	return 0;
}
