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

int parse(unsigned char *buffer, char *src)
{
    unsigned char *ebp = buffer;
    char *eip = src;
    int depth = 0;

    while (*eip) {
        /*printf("depth %d, eip %c, ebp: %d\n", depth, *eip, *ebp);*/
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
                if (!*ebp) {
                    depth = 1;
                    while (depth != 0) {
                        ++eip;
                        if (*eip == '[') { depth++; }
                        else if (*eip == ']') { depth--; }
                    }
                }
                break;
            case ']':
                if (*ebp) {
                    depth = -1;
                    while (depth != 0) {
                        --eip;
                        if (*eip == ']') { depth--; }
                        else if (*eip == '[') { depth++; }
                    }
                }
                break;
        }
        ++eip;
    }
    
    if (depth > 0) {
        printf("Error: Unclosed [");
        return EXIT_FAILURE;
    }
    else if (depth < 0) {
        printf("Error: Unclosed ]");
        return EXIT_FAILURE;
    }
    else {
        return EXIT_SUCCESS;
    }
}

int main(int argc, char *argv[])
{
    unsigned char memory[PROGRAM_SIZE_BYTES] = {'\0'};
    char *bf_string = '\0';
    int exit_code = 0;

    if ((argc == 2) && argv[1]) {
        bf_string = argv[1];
        exit_code = parse(memory, bf_string);
    } 
    else {
        return EXIT_FAILURE;
    }

    return exit_code;
}
