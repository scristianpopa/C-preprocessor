#pragma once
#include "hashmap.h"
#include "my_string_array.h"

#define MULTIPLE_FILES 3

typedef struct Arg_processor {
	char *infile, *outfile;
	My_string_array *include_dirs;
	Hashmap *hashmap;
} Arg_processor;

int arg_processor_new(Arg_processor **arg_processor);

void arg_processor_delete(Arg_processor *arg_processor);

int process_args(Arg_processor *arg_processor, int argc, char *argv[]);
