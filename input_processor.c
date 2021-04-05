#include "input_processor.h"

#define IF_LEN 3
#define ELSE_LEN 5
#define ELIF_LEN 5
#define IFDEF_LEN 6
#define IFNDEF_LEN 7
#define ENDIF_LEN 6
#define DEF_LEN 7
#define UNDEF_LEN 6
#define INCL_LEN 8
#define DEF_VAL_MAX_SIZE 1024
#define UNDEFINED_LINE_MAX_SIZE 5000
#define FILE_NOT_FOUND -7
#define NOW_WRITING 1
#define WROTE_BEFORE 2
#define ENOENT 2
#define FPRINTF_ERR 17

/* opens input and output files at the beginning */
static int open_in_out(Input_processor *input_processor,
			Arg_processor *arg_processor)
{
	if (!arg_processor->infile)
		input_processor->in = stdin;
	else {
		input_processor->in = fopen(arg_processor->infile, "r");
		if (!input_processor->in) {
			printf("Open infile");
			return -ENOENT;
		}
	}
	if (!arg_processor->outfile)
		input_processor->out = stdout;
	else {
		input_processor->out = fopen(arg_processor->outfile, "w");
		if (!input_processor->out) {
			printf("Open outfile");
			return -ENOENT;
		}
	}
	return 0;
}

/* processes a string and replaces all defined values */
static int undefine_vals(Input_processor *input_processor,
			char *line, char **undefined_line)
{
	static char delim[28];
	char *key, *val, *start_word, *end_word,
		local_undefined[UNDEFINED_LINE_MAX_SIZE];
	int between_quotes;

	strcpy(delim, "\n\t []{}<>=+-*/%!&|^.,:;()\\");
	strcpy(local_undefined, "");
	start_word = line;
	between_quotes = 0;  /* don't want to replace "#define" */ 
	while (start_word && *start_word) {
		end_word = strpbrk(start_word, delim);
		/* in case of a delimiter just paste it */
		if (end_word == start_word) {
			local_undefined[strlen(local_undefined) + 1] = 0;
			local_undefined[strlen(local_undefined)] = *start_word;

			if (*start_word == '"')
				between_quotes = 1 - between_quotes;
			++start_word;
		} else {
			if (!end_word)
				key = start_word;
			else {
				key = (char *)malloc(
						(end_word - start_word + 1) *
						sizeof(char));
				if (!key) {
					printf("Undefine tokens malloc");
					fflush(stdout);
					return -ENOMEM;
				}

				strncpy(key, start_word,
					end_word - start_word);
				key[end_word - start_word] = 0;
			}
			/* undefine the key until we have the final value */
			val = get(input_processor->hashmap, key);
			if (val && !between_quotes) {
				if (end_word)
					free(key);
				while (val) {
					key = val;
					val = get(input_processor->hashmap,
							key);
				}
				strcat(local_undefined, key);
			} else {
				strcat(local_undefined, key);
				if (end_word)
					free(key);
			}
			start_word = end_word;
		}
	}

	*undefined_line = (char *)malloc((strlen(local_undefined) + 1) *
						sizeof(char));
	if (!(*undefined_line)) {
		printf("Undefined line malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strcpy(*undefined_line, local_undefined);
	return 0;
}

/* gets the first parameter of commands (e.g. define, if) */
static int command_first_param(Input_processor *input_processor, int offset,
				char **param)
{
	int start;
	static char delim[28];

	strcpy(delim, "\n\t []{}<>=+-*/%!&|^.,:;()\\");
	while (input_processor->line[offset] == ' ')
		++offset;
	start = offset;
	offset = strcspn(input_processor->line + start, delim);
	*param = (char *)malloc((offset + 1) * sizeof(char));
	if (!(*param)) {
		printf("Command parameter malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	strncpy(*param, input_processor->line + start, offset);
	(*param)[offset] = 0;
	return 0;
}

static int process_define(Input_processor *input_processor, int offset)
{
	int start, backslash_pos, code, trailing_space;
	char *key, val[DEF_VAL_MAX_SIZE], *undefined_val;

	if (input_processor->write->val != (char *)NOW_WRITING)
		return 0;

	strcpy(val, "");

	code = command_first_param(input_processor, offset + DEF_LEN, &key);
	if (code)
		return code;

	start = strstr(input_processor->line, key) - input_processor->line +
			strlen(key);
	while (input_processor->line[start] == ' ' ||
		input_processor->line[start] == '\t')
		++start;

	/* check for multi-line define */
	backslash_pos = strlen(input_processor->line) - 2;
	while (backslash_pos >= 0 &&
		input_processor->line[backslash_pos] == '\\') {
		/* remove white spaces from the back */
		trailing_space = backslash_pos - 1;
		while (trailing_space > start &&
			(input_processor->line[trailing_space] == ' ' ||
			input_processor->line[trailing_space] == '\t'))
			--trailing_space;
		if (trailing_space == backslash_pos - 1) {
			strncat(val, input_processor->line + start,
				backslash_pos - start);
			val[strlen(val) + 1] = 0;
			val[strlen(val)] = ' ';
		} else
			strncat(val, input_processor->line + start,
				trailing_space + 2 - start);

		fgets(input_processor->line, MAX_LINE_SIZE,
			input_processor->in);

		/* remove white spaces from the beginning */
		start = 0;
		while (input_processor->line[start] == ' ' ||
			input_processor->line[start] == '\t')
			++start;
		backslash_pos = strlen(input_processor->line) - 2;
	}
	strcat(val, input_processor->line + start);
	val[strlen(val) - 1] = 0;

	code = undefine_vals(input_processor, val, &undefined_val);
	if (code)
		return code;
	code = put(input_processor->hashmap, key, undefined_val);
	if (code)
		return code;

	free(undefined_val);
	free(key);
	return 0;
}

static int process_undefine(Input_processor *input_processor, int offset)
{
	char *key;
	int code;

	if (input_processor->write->val != (char *)NOW_WRITING)
		return 0;

	code = command_first_param(input_processor, offset + UNDEF_LEN, &key);
	if (code)
		return code;

	delete(input_processor->hashmap, key);
	free(key);
	return 0;
}

static int process_include(Input_processor *input_processor)
{
	char *first_quote, *second_quote, *file_name, *file_path, *curr_dir;
	FILE *old_in, *new_in;
	int i, code, add_terminal_slash;

	if (input_processor->write->val != (char *)NOW_WRITING)
		return 0;

	/* some directories might not end in a '/' */
	add_terminal_slash = 0;
	old_in = input_processor->in;
	first_quote = strchr(input_processor->line, '"');
	++first_quote;
	second_quote = strchr(first_quote, '"');
	*second_quote = 0;

	file_name = first_quote;
	for (i = 0; i < input_processor->include_dirs->size; ++i) {
		curr_dir = input_processor->include_dirs->strings[i];
		if (curr_dir[strlen(curr_dir) - 1] != '/')
			add_terminal_slash = 1;
		file_path = (char *)malloc(strlen(curr_dir) +
					strlen(file_name) +
					add_terminal_slash + 1);
		if (!file_path) {
			printf("Include file path malloc");
			fflush(stdout);
			return -ENOMEM;
		}
		strcpy(file_path, curr_dir);
		if (add_terminal_slash)
			strcat(file_path, "/");
		strcat(file_path, file_name);

		new_in = fopen(file_path, "r");
		if (new_in) {
			input_processor->in = new_in;
			/* recurively process all included files */
			code = process_input(input_processor);
			if (code)
				return code;

			if (fprintf(input_processor->out, "%c", '\n') < 0) {
				printf("fprintf error");
				return -FPRINTF_ERR;
			}
			fclose(new_in);
			input_processor->in = old_in;
			free(file_path);
			return 0;
		}
		free(file_path);
	}
	return FILE_NOT_FOUND;
}

static int process_if(Input_processor *input_processor, int offset)
{
	char *expression, *val;
	List *write;
	int code;

	/* add a new "context" from now on until the next #endif */
	code = simple_list_new(&write);
	if (code)
		return code;
	write->next = input_processor->write;
	input_processor->write = write;

	if (input_processor->write->next->val == (char *)WROTE_BEFORE) {
		input_processor->write->val = (char *)WROTE_BEFORE;
		return 0;
	}

	code = command_first_param(input_processor, offset + IF_LEN,
					&expression);
	if (code)
		return code;

	val = get(input_processor->hashmap, expression);

	if (val) {
		free(expression);
		expression = val;
		val = get(input_processor->hashmap, expression);
		while (val) {
			expression = val;
			val = get(input_processor->hashmap, expression);
		}
		/* espression is evaluated to nonzero so we're doing
		 * whatever we were doing in the previous context
		 */
		if (strcmp(expression, "0"))
			input_processor->write->val =
				input_processor->write->next->val;
		return 0;
	}
	if (strcmp(expression, "0"))
		input_processor->write->val =
				input_processor->write->next->val;
	free(expression);
	return 0;
}

static int process_elif(Input_processor *input_processor, int offset)
{
	char *expression, *val;
	int code;

	if (input_processor->write->val == (char *)WROTE_BEFORE)
		return 0;
	if (input_processor->write->val == (char *)NOW_WRITING) {
		input_processor->write->val = NULL;
		return 0;
	}

	code = command_first_param(input_processor, offset + ELIF_LEN,
					&expression);
	if (code)
		return code;

	val = get(input_processor->hashmap, expression);
	if (val) {
		free(expression);
		expression = val;
		val = get(input_processor->hashmap, expression);
		while (val) {
			expression = val;
			val = get(input_processor->hashmap, expression);
		}

		if (strcmp(expression, "0"))
			input_processor->write->val = (char *)NOW_WRITING;
		return 0;
	}
	if (strcmp(expression, "0"))
		input_processor->write->val = (char *)NOW_WRITING;
	free(expression);
	return 0;
}

static void process_else(Input_processor *input_processor)
{
	if (input_processor->write->val == NULL) {
		input_processor->write->val = (char *)NOW_WRITING;
		return;
	}
	input_processor->write->val = (char *)WROTE_BEFORE;
}

static void process_endif(Input_processor *input_processor)
{
	List *prev_context;

	prev_context = input_processor->write->next;
	input_processor->write->val = NULL;
	list_delete(input_processor->write);
	input_processor->write = prev_context;
}

static int process_ifdef(Input_processor *input_processor, int offset)
{
	char *expression;
	List *write;
	int code;

	code = simple_list_new(&write);
	if (code)
		return code;
	write->next = input_processor->write;
	input_processor->write = write;
	if (input_processor->write->next->val == NULL)
		return 0;

	code = command_first_param(input_processor, offset + IFDEF_LEN,
					&expression);
	if (code)
		return code;
	if (get(input_processor->hashmap, expression))
		input_processor->write->val = (char *)NOW_WRITING;
	free(expression);
	return 0;
}

static int process_ifndef(Input_processor *input_processor, int offset)
{
	char *expression;
	List *write;
	int code;

	code = simple_list_new(&write);
	if (code)
		return code;
	write->next = input_processor->write;
	input_processor->write = write;

	if (input_processor->write->next->val == NULL)
		return 0;

	code = command_first_param(input_processor, offset + IFNDEF_LEN,
					&expression);
	if (code)
		return code;
	if (!get(input_processor->hashmap, expression))
		input_processor->write->val = (char *)NOW_WRITING;
	free(expression);
	return 0;
}

int input_processor_new(Input_processor **input_processor,
			Arg_processor *arg_processor)
{
	int code;

	*input_processor = (Input_processor *)malloc(sizeof(Input_processor));
	if (!(*input_processor)) {
		printf("Input processor malloc");
		fflush(stdout);
		return -ENOMEM;
	}

	(*input_processor)->hashmap = arg_processor->hashmap;
	(*input_processor)->include_dirs = arg_processor->include_dirs;
	code = simple_list_new(&((*input_processor)->write));
	if (code)
		return code;
	(*input_processor)->write->val = (char *)NOW_WRITING;

	code = open_in_out(*input_processor, arg_processor);
	if (code)
		return code;
	return 0;
}

void input_processor_delete(Input_processor *input_processor)
{
	fclose(input_processor->in);
	fclose(input_processor->out);
	free(input_processor->write);
	free(input_processor);
}

int process_input(Input_processor *input_processor)
{
	int code;
	char *hashtag, *undefined_line;

	while (fgets(input_processor->line, MAX_LINE_SIZE,
			input_processor->in)) {
		hashtag = input_processor->line;
		while (*hashtag == ' ' || *hashtag == '\t')
			++hashtag;
		if (*hashtag != '#') {
			if (input_processor->write->val ==
				(char *)NOW_WRITING) {
				/* undefine any defined values and print
				 * them to the output file
				 */
				code = undefine_vals(input_processor,
							input_processor->line,
							&undefined_line);
				if (code)
					return code;
				if (fprintf(input_processor->out, "%s",
					undefined_line) < 0) {
					printf("fprintf error");
					return -FPRINTF_ERR;
				}
				free(undefined_line);
			}
		} else if (!strncmp(hashtag, "#define", DEF_LEN)) {
			code = process_define(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#undef", UNDEF_LEN)) {
			code = process_undefine(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#include", INCL_LEN)) {
			code = process_include(input_processor);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#ifdef", IFDEF_LEN)) {
			code = process_ifdef(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#ifndef", IFNDEF_LEN)) {
			code = process_ifndef(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#if", IF_LEN)) {
			code = process_if(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#elif", ELIF_LEN)) {
			code = process_elif(input_processor, hashtag -
						input_processor->line);
			if (code)
				return code;
		} else if (!strncmp(hashtag, "#else", ELSE_LEN)) {
			process_else(input_processor);
		} else if (!strncmp(hashtag, "#endif", ENDIF_LEN)) {
			process_endif(input_processor);
		}
	}
	return 0;
}
