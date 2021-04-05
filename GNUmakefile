CC = gcc
CFLAGS = -Wall -Wextra -g -std=c89 -ansi -pedantic
ARGS =
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all

build: so-cpp

so-cpp: main.o input_processor.o arg_processor.o my_string_array.o hashmap.o list.o 
	$(CC) $(CFLAGS) main.o input_processor.o arg_processor.o \
			my_string_array.o hashmap.o list.o -o so-cpp

run: so-cpp
	./so-cpp $(ARGS)

leak: so-cpp
	valgrind $(VALGRIND_FLAGS) ./so-cpp $(ARGS)

main.o: main.c

input_processor.o: input_processor.c

arg_processor.o: arg_processor.c

my_string_array.o: my_string_array.c

hashmap.o: hashmap.c

list.o: list.c

check:
	./_test/run_test.sh 0

.PHONY: clean
clean:
	rm -rf *.o so-cpp

