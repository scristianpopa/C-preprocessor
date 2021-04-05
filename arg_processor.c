#include "arg_processor.h"

/* processes a -d argument */
static int process_d(Arg_processor *arg_processor, char *arg)
{
	char *pos;
	int code;

	pos = strtok(arg, "=");
	pos = strtok(NULL, "=");
	if (!pos) {
		code = put(arg_processor->hashmap, arg, "");
		if (code)
			return code;
	} else {
		code = put(arg_processor->hashmap, arg, pos);
		if (code)
			return code;
	}
	return 0;
}

/* processes a -i argument */
static int process_i(Arg_processor *arg_processor, char *arg)
{
	int code;

	code = push_back(arg_processor->include_dirs, arg);
	if (code)
		return code;
	return 0;
}

/* processes the output file */
static int process_o(Arg_processor *arg_processor, char *arg)
{
	arg_processor->outfile = (char *)malloc((strlen(arg) + 1) *
						sizeof(char));
	if (!(arg_processor->outfile)) {
		printf("Outfile malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(arg_processor->outfile, arg);
	return 0;
}

/* processes the input file */
static int process_in(Arg_processor *arg_processor, char *arg)
{
	int code;
	char *in_dir;
	char *last_slash;

	arg_processor->infile = (char *)malloc((strlen(arg) + 1) *
						sizeof(char));
	if (!(arg_processor->infile)) {
		printf("Infile malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(arg_processor->infile, arg);

	/* get the infile directory */
	last_slash = strrchr(arg, '/');
	if (!last_slash)
		return 0;
	*(last_slash + 1) = 0;

	in_dir = (char *)malloc((strlen(arg) + 1) * sizeof(char));
	if (!in_dir) {
		printf("In file directory malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(in_dir, arg);

	code = change_at(arg_processor->include_dirs, 0, in_dir);
	if (code)
		return code;
	free(in_dir);
	return 0;
}

int arg_processor_new(Arg_processor **arg_processor)
{
	int code;

	*arg_processor = (Arg_processor *)malloc(sizeof(Arg_processor));
	if (!(*arg_processor)) {
		printf("Arg processor malloc");
		fflush(stdout);
		return -ENOMEM;
	}

	code = my_string_array_new(&(*arg_processor)->include_dirs);
	if (code)
		return code;
	code = push_back((*arg_processor)->include_dirs, "./");
	if (code)
		return code;

	code = hashmap_new(&(*arg_processor)->hashmap);
	if (code)
		return code;

	(*arg_processor)->infile = NULL;
	(*arg_processor)->outfile = NULL;
	return 0;
}

void arg_processor_delete(Arg_processor *arg_processor)
{
	free(arg_processor->infile);
	free(arg_processor->outfile);
	my_string_array_delete(arg_processor->include_dirs);
	hashmap_delete(arg_processor->hashmap);
	free(arg_processor);
}

/* processed the entire list of arguments */
int process_args(Arg_processor *arg_processor, int argc, char *argv[])
{
	int i, code;

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'D': {
				/* check whether -D is attached to argument */
				if (strlen(argv[i]) == 2)
					code = process_d(arg_processor,
							argv[++i]);
				else
					code = process_d(arg_processor,
							argv[i] + 2);
				if (code)
					return code;
				break;
			}
			case 'I': {
				if (strlen(argv[i]) == 2)
					code = process_i(arg_processor,
							argv[++i]);
				else
					code = process_i(arg_processor,
							argv[i] + 2);
				if (code)
					return code;
				break;
			}
			case 'o': {
				if (strlen(argv[i]) == 2)
					code = process_o(arg_processor,
							argv[++i]);
				else
					code = process_o(arg_processor,
							argv[i] + 2);
				if (code)
					return code;
				break;
			}
			default:
				break;
			}
		} else {
			if (!arg_processor->infile) {
				code = process_in(arg_processor, argv[i]);
				if (code)
					return code;
			} else {
				if (!arg_processor->outfile) {
					code = process_o(arg_processor,
							argv[i]);
					if (code)
						return code;
				} else
					return MULTIPLE_FILES;
			}
		}
	}
	return 0;
}
