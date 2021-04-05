#pragma once
#include "arg_processor.h"

#define MAX_LINE_SIZE 256

typedef struct Input_processor {
	char line[MAX_LINE_SIZE + 1];
	FILE *in, *out;
	Hashmap *hashmap;
	My_string_array *include_dirs;
	List *write;
} Input_processor;

int input_processor_new(Input_processor **input_processor,
			Arg_processor *arg_processor);

void input_processor_delete(Input_processor *input_processor);

int process_input(Input_processor *input_processor);
