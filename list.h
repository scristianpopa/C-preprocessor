#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENOMEM 12

typedef struct List {
	char *key, *val;
	struct List *next;
} List;

int list_new(List **l, char *key, char *val, List *next);

int simple_list_new(List **l);

void list_delete(List *list);
