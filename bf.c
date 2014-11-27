 /**
 * gcc -g -ansi -Wall -Wextra -Werror -pedantic -pedantic-errors -o bf bf.c
 * Run as:
 * ./bf somestring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_SIZE_BYTES 30000
#define CALL_STACK_SIZE 25000
#define MAX_INPUT_LEN 2500
#define EXIT_LOOP -1
#define ENTER_LOOP_SYM 'S'

#define STACK_FULL -1
#define STACK_EMPTY -2
#define CHECK_STACK_EMPTY(stack) \
	(stack->ebp == stack->esp)

struct STACK {
	int *_stack;
	int *ebp;
	int *esp;
	size_t size;
};

struct STACK *init_stack(struct STACK *stack, size_t size) {
	stack = malloc(sizeof(struct STACK));
	stack->_stack = calloc(size, sizeof(int));
	stack->size = size;
	stack->ebp = stack->esp = stack->_stack;
	return stack;
}

void destroy_stack(struct STACK *stack) {
	free(stack->_stack);
	free(stack);
}

int pop_stack(struct STACK *stack) {
	if (stack->esp == stack->ebp)
		return STACK_EMPTY;
	int item = *stack->esp;
	*stack->esp = '\0';
	--(stack->esp);
	return item;
}

int push_stack(struct STACK *stack, int item) {
	if ((size_t)(stack->esp - stack->ebp) >= stack->size)
		return STACK_FULL;
	++(stack->esp);
	*stack->esp = item;
	return EXIT_SUCCESS;
}

int parse(struct STACK *_call_stack, unsigned char *buffer, char *src)
{
	unsigned char *offset = buffer;
	char *eip = src;

	while (*eip) {
		switch (*eip) {
		    case '>':
			++offset;
			break;
		    case '<':
			--offset;
			break;
		    case '+':
			++*offset;
			break;
		    case '-':
			--*offset;
			break;
		    case '.':
			putchar(*offset);
			break;
		    case ',':
			*offset = getchar();
			break;
		    case '[':
			++eip;
			if (push_stack(_call_stack, ENTER_LOOP_SYM) == STACK_FULL) {
				printf("Max recursion depth of %d exceeded.\n", CALL_STACK_SIZE);	
				goto fail_cleanup;
			}
			parse(_call_stack, offset, eip);
			break;
		    case ']':
			++eip;
			if (pop_stack(_call_stack) == STACK_EMPTY) {
				printf("Program terminated early. Unmatched exit loop brace ( ] )\n");
				goto fail_cleanup;
			}
			return EXIT_LOOP;
			break;
		}
		++eip;
	}

	if (!CHECK_STACK_EMPTY(_call_stack)) {
		printf("Program terminated early. Unmatched enter loop brace ( [ )\n");
		goto fail_cleanup;
	}

	return EXIT_SUCCESS;

fail_cleanup:
	destroy_stack(_call_stack);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	unsigned char memory[PROGRAM_SIZE_BYTES] = {'\0'};
	char bf_string[MAX_INPUT_LEN + 1] = {'\0'};
	struct STACK *call_stack = NULL;
	unsigned int status = 0;

	call_stack = init_stack(call_stack, CALL_STACK_SIZE);

	if ((argc == 2) && argv[1]) {
		strncpy(bf_string, argv[1], MAX_INPUT_LEN);
	} else if (argc != 2) {
		printf("You must provide one argument\n");
		return EXIT_FAILURE;
	}
	status = parse(call_stack, memory, bf_string);
	write(STDOUT_FILENO, memory, PROGRAM_SIZE_BYTES);

	destroy_stack(call_stack);

	return status;
}
