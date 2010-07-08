#ifndef _H_TEMPLATE_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#define TPL_BUFFER 1000
#define REPLACE_BUFFER 300
typedef struct tplVar{
	char *varName;
	char *varValue;
	struct tplVar *next;
} tplVar;
tplVar *tplVarRoot;
typedef struct tplNode{
	char *varName;
	char *varValue;
	struct tplNode *left;
	struct tplNode *right;
} tplNode;
tplNode *tplBinTree;
extern int addTplVar(const char*, const char*);
extern int initTplList(void);
extern int initTplTree(void);
extern int displayTpl(char*);
extern int addRePuffer(char*, int*, char);
extern int inputVar(char*, int*, const char*);
extern char *searchTplVar(const char*);
extern void getVarName(const char*, char*, size_t, int*);
extern int callPlugin(char*, int*, char*);
extern int deleteVarList(tplVar*);
#endif
