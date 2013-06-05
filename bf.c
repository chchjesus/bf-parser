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
#include <string.h>

#define PROGRAM_SIZE_BYTES 30000

int parse(unsigned char *buffer, const char *src)
{
    unsigned char *ebp = buffer;
    int srclen = strlen(src);
    int eip = 0;
    int depth = 0;

    while (src[eip]) {
        /*printf("depth %d, eip %d, ebp: %d\n", depth, eip, *ebp);*/
        switch (src[eip]) {
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
                    while ((eip < srclen) && (depth > 0)) {
                        ++eip;
                        if (src[eip] == '[') { depth++; }
                        else if (src[eip] == ']') { depth--; }
                    }
                }
                break;
            case ']':
                if (*ebp) {
                    depth = -1;
                    while (eip && (depth < 0)) {
                        --eip;
                        if (src[eip] == ']') { depth--; }
                        else if (src[eip] == '[') { depth++; }
                    }
                }
                break;
        }
        
        if (depth < 0) {
            printf("Error: Unclosed ]");
            return EXIT_FAILURE;
        }
        else if (depth > 0) {
            printf("Error: Unclosed [");
            return EXIT_FAILURE;
        }
        else { ++eip; }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    unsigned char memory[PROGRAM_SIZE_BYTES] = {'\0'};
    char *bf_string = '\0';

    if ((argc == 2) && argv[1]) {
        bf_string = argv[1];
    } 
    else {
        bf_string = "";
    }

    return parse(memory, bf_string);
}
