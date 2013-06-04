/**
 * @file bf.c
 * @author Cody Harrington
 * @details Simple brainf**k parser written in ANSI C. Compiled with
 * gcc -g -ansi -Wall -Wextra -Werror -pedantic -pedantic-errors -o bf bf.c
 * Run as:
 * ./bf somestring
 */

#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_SIZE_BYTES 30000

void parse(unsigned char *buffer, char *src)
{
	unsigned char *ebp = buffer;
	char *eip = src;
    char *esp = eip;

	while (*eip) {
		switch (*eip) {
			case '>':
				++ebp;
				break;
			case '<':
				--ebp;
				break;
			case '+':
				++*ebp;
				break;
			case '-':
				--*ebp;
				break;
			case '.':
				putchar(*ebp);
				break;
			case ',':
				*ebp = getchar();
				break;
			case '[':
                /* Set a pointer to the start of the loop */
                esp = eip;
				break;
			case ']':
                /* If the current value is not null, 
                 * go back to the start of the loop, stored in esp
                 */
                if (*ebp) {
                    eip = esp;
                }
                break;
		}
		++eip;
	}	
}

int main(int argc, char *argv[])
{
	unsigned char memory[PROGRAM_SIZE_BYTES] = {'\0'};
	char *bf_string = '\0';

	if ((argc == 2) && argv[1]) {
		bf_string = argv[1];
		parse(memory, bf_string);
	} 
	else {
        return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
