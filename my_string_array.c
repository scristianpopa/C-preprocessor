#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_string_array.h"

#define ENOMEM 12
#define INDEX_OUT_OF_BOUNDS 1

int my_string_array_new(My_string_array **array)
{
	*array = (My_string_array *)malloc(sizeof(My_string_array));
	if (!(*array)) {
		printf("Array malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	(*array)->strings = (char **)malloc(sizeof(char *));
	if (!((*array)->strings)) {
		printf("Array strings malloc");
		fflush(stdout);
		return -ENOMEM;
	}

	(*array)->strings[0] = NULL;
	(*array)->size = 0;
	(*array)->capacity = 1;
	return 0;
}

void my_string_array_delete(My_string_array *array)
{
	int i;

	for (i = 0; i < array->size; ++i)
		free(array->strings[i]);
	free(array->strings);
	free(array);
}

int push_back(My_string_array *array, char *string)
{
	if (array->size == array->capacity) {
		array->capacity <<= 1;
		array->strings = (char **)realloc(array->strings,
				sizeof(char *) * array->capacity);
		if (!(array->strings)) {
			printf("Array strings realloc");
			fflush(stdout);
			return -ENOMEM;
		}
	}

	array->strings[array->size] = (char *)malloc((strlen(string) + 1) *
							sizeof(char));
	if (!(array->strings[array->size])) {
		printf("Array push malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(array->strings[array->size], string);
	++array->size;
	return 0;
}

int change_at(My_string_array *array, int index, char *string)
{
	if (index < 0 || index >= array->size)
		return -INDEX_OUT_OF_BOUNDS;
	free(array->strings[index]);
	array->strings[index] = (char *)malloc((strlen(string) + 1) *
							sizeof(char));
	if (!(array->strings[index])) {
		printf("Array change malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(array->strings[index], string);
	return 0;
}
