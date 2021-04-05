
	Implemented a C preprocessor, with support for inclusion of header files, 
all #if commands and multiple line defines, with the exception of functions.
	Can be run with the following arguments:
	- -D <SYMBOL>[=<MAPPING>]/-D<SYMBOL>[=<MAPPING>]: will define SYMBOL as
		MAPPING, or "" in case MAPPING is missing
	- -I <DIR>/-I<DIR>: adds directories in which to look for the included
		header files
	- <INFILE>: specifies the file to be processed. In case this is missing,
		input will be given from STDIN until EOF
	- -o <OUTFILE>/-o<OUTFILE>: specifies the file in which to write the 
		preprocessed input. In case this is missing, output will be shown to
		STDOUT. Can also be used as <OUTFILE> if there was a previous <INFILE>
		defined

	Linux Makefile commands:
	-make build: builds the so-cpp executable
	-make run ARGS="your arguments here": runs the preprocessor using ARGS
	