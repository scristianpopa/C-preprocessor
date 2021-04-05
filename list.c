#include "list.h"

int list_new(List **l, char *key, char *val, List *next)
{
	*l = (List *)malloc(sizeof(List));
	if (!(*l)) {
		printf("List malloc");
		fflush(stdout);
		return -ENOMEM;
	}

	(*l)->key = malloc((strlen(key) + 1) * sizeof(char));
	if (!((*l)->key)) {
		printf("List key malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy((*l)->key, key);

	(*l)->val = malloc((strlen(val) + 1) * sizeof(char));
	if (!((*l)->val)) {
		printf("List val malloc");
		fflush(stdout);
	return -ENOMEM;
	}
	strcpy((*l)->val, val);
	(*l)->next = next;
	return 0;
}

/* used to simulate a stack of #if levels */
int simple_list_new(List **l)
{
	*l = (List *)malloc(sizeof(List));
	if (!(*l)) {
		printf("List malloc");
		fflush(stdout);
		return -ENOMEM;
	}

	(*l)->key = NULL;
	(*l)->val = NULL;
	(*l)->next = NULL;

	return 0;
}

void list_delete(List *list)
{
	free(list->key);
	free(list->val);
	free(list);
}
