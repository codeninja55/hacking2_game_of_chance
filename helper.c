//
// Created by codeninja on 23/08/18.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// A function to display an error message and then exit
void fatal(char *message) {
    char error_message[200];

    strcpy(error_message, "[!!] Fatal Error ");
    strncat(error_message, message, 83);
    perror(error_message);
    exit(-1);
}

// An error-checked malloc() wrapper function
void *ec_malloc(unsigned int size) {
    void *ptr;
    ptr = malloc(size);
    if(ptr == NULL)
        fatal("in ec_malloc() on memory allocation");
    return ptr;
}