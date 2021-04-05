#include "input_processor.h"

int main(int argc, char *argv[])
{
	int code;
	Arg_processor *arg_processor;
	Input_processor *input_processor;

	code = arg_processor_new(&arg_processor);
	if (code)
		return -code;

	code = process_args(arg_processor, argc, argv);
	if (code)
		return -code;

	code = input_processor_new(&input_processor, arg_processor);
	if (code)
		return -code;

	code = process_input(input_processor);
	if (code)
		return -code;

	arg_processor_delete(arg_processor);
	input_processor_delete(input_processor);
	return 0;
}
