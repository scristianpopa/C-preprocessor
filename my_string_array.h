#pragma once

typedef struct My_string_array {
	int size, capacity;
	char **strings;
} My_string_array;

int my_string_array_new(My_string_array **array);

void my_string_array_delete(My_string_array *array);

int push_back(My_string_array *array, char *string);

int change_at(My_string_array *array, int index, char *string);
