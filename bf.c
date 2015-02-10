 /**
 * gcc -g -ansi -Wall -Wextra -Werror -pedantic -pedantic-errors -o bf bf.c
 * Run as:
 * ./bf somestring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define PROGRAM_SIZE_BYTES 30000
#define CALL_STACK_SIZE 25000
#define ARG_STACK_SIZE 25000
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
	size_t nmemb;
};

struct STACK *init_stack(struct STACK *stack, size_t nmemb) {
	stack = malloc(sizeof(struct STACK));
	stack->_stack = calloc(nmemb, sizeof(unsigned int));
	stack->nmemb = nmemb;
	stack->ebp = stack->esp = stack->_stack;
	return stack;
}

void destroy_stack(struct STACK *stack) {
	free(stack->_stack);
	free(stack);
}

int pop_stack(struct STACK *stack) {
	//printf("Pop stack\n");
	if (stack->esp == NULL) {
		return STACK_EMPTY;
	}
	int item = *stack->esp;
	*stack->esp = '\0';
	if (stack->esp == stack->ebp) {
		stack->esp = NULL;		
	} else {
		--(stack->esp);
	}
	//printf("Popped item: %d %c\n", item, item);
	return item;
}

int push_stack(struct STACK *stack, int item) {
	//printf("Push stack\n");
	if ((size_t)(stack->esp - stack->ebp) >= stack->nmemb)
		return STACK_FULL;
	if (stack->esp != NULL)
		++(stack->esp);
	*stack->esp = item;
	//printf("Pushed item: %d %c\n", item, item);
	return EXIT_SUCCESS;
}

void _fail_cleanly(struct STACK *_call_stack, struct STACK *_arg_stack, char *message) {
	fprintf(stderr, "%s\n", message);
	destroy_stack(_call_stack);
	destroy_stack(_arg_stack);
	exit(EXIT_FAILURE);
}

int parse(struct STACK *_call_stack, struct STACK *_arg_stack, 
		unsigned char *mem, char *src)
{
	printf("Entered parse\n");
	int eip = pop_stack(_arg_stack);
	int offset = pop_stack(_arg_stack);

	while (src[eip]) {
		//printf("eip: %d *eip: %c\n", eip, src[eip]);
		//write(STDOUT_FILENO, mem, PROGRAM_SIZE_BYTES);
		switch (src[eip]) {
		    case '>':
			++offset;
			break;
		    case '<':
			--offset;
			break;
		    case '+':
			++mem[offset];
			break;
		    case '-':
			--mem[offset];
			break;
		    case '.':
			putchar(mem[offset]);
			break;
		    case ',':
			mem[offset] = getchar();
			break;
		    case '[':
			if (mem[offset]) {
				if (push_stack(_call_stack, ENTER_LOOP_SYM) == STACK_FULL) {
					_fail_cleanly(_call_stack, _arg_stack,
						       	"Max recursion depth exceeded");
				}

				push_stack(_arg_stack, offset);
				push_stack(_arg_stack, ++eip);

				parse(_call_stack, _arg_stack, mem, src);
			} else { 
				while (src[eip] && src[eip] != ']') 
					++eip;
				if (!src[eip]) {
					return -1;
				}
			}
			break;
		    case ']':
			if (!mem[offset]) {
				if (pop_stack(_call_stack) == STACK_EMPTY) {
					_fail_cleanly(_call_stack, _arg_stack,
						       	"Program terminated early. Unmatched exit loop brace ( ] )\n");
				}
				return EXIT_LOOP;
			} else {
				while (src[eip] && src[eip] != '[')
					--eip;
				if (!src[eip]) {
					return -1;	
				}
			}
			break;
		}
		++eip;
	}

	if (!CHECK_STACK_EMPTY(_call_stack)) {
		_fail_cleanly(_call_stack, _arg_stack,
			       	"Program terminated early. Unmatched enter loop brace ( [ )\n");
	}

	printf("exiting parse normally\n");
	return EXIT_SUCCESS;
}

int init_program(unsigned char *mem, char *src) {
	struct STACK *call_stack = NULL;
	struct STACK *arg_stack = NULL;
	unsigned int status = 0;

	call_stack = init_stack(call_stack, CALL_STACK_SIZE);
	arg_stack = init_stack(arg_stack, ARG_STACK_SIZE);

	/* Push our ebp and eip */
	push_stack(arg_stack, 0);
	push_stack(arg_stack, 0);

	status = parse(call_stack, arg_stack, mem, src);

	destroy_stack(call_stack);
	destroy_stack(arg_stack);

	return status;
}

int main(int argc, char *argv[])
{
	unsigned char memory[PROGRAM_SIZE_BYTES] = {'\0'};
	char source[MAX_INPUT_LEN + 1] = {'\0'};

	if ((argc == 2) && argv[1]) {
		strncpy(source, argv[1], MAX_INPUT_LEN);
	} else if (argc != 2) {
		printf("You must provide one argument\n");
		return EXIT_FAILURE;
	}

	return init_program(memory, source);
}
