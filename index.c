#include <stdio.h>
#include "counter.h"
#include "template.h"

int main(void){
	printf("Content-Type: text/html\n\n");
	printf("<p>You are the %d. user here.</p>\n", displayCounter(1));
	addTplVar("Test", "Laaaaaa");
	addTplVar("Test1", "Laaaaaa1");
	printf("<p>%s, %s</p>\n", tplVarRoot->varName, tplVarRoot->varValue);
	displayTpl("tpl/test.tpl");
}
