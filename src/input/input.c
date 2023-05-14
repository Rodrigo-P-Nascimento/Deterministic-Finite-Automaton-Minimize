//
// Created by ekank on 5/13/23.
//

#include "input.h"
#include <string.h>
#include <stdlib.h>

#define char_size sizeof(char)

typedef struct{
    char* alphabet;
    char* states;
    char* q0;
    char* F;
    char* transitions;
} DFA_file;

static DFA_file a = {};

static inline void Strip(char* src, char** restrict dst){

    strtok(src, ":");
    *dst = strtok(NULL, ":");
}

static inline void Assign(char* buf, char** const restrict dst){

    char* data = NULL;

    Strip(buf, &data);

    if(!data){
        //TODO tratar valor
    }

    *dst = malloc(strlen(data)+1);
    strcpy(*dst, data);
}

error_t ReadFile(char* path){

    error_t err = OK;

    char *buffer = calloc(100, char_size);
    if (!buffer){
        err = ALLOCATION_ERROR;
        goto mem;
    }

    FILE* fp = fopen(path, "r");
    if (!fp){
        err = FILE_DOESNT_EXIST;
        goto file;
    }

    fread(buffer, 100, 1, fp);
    Assign(buffer, &a.alphabet);

    fread(buffer, 100, 1, fp);
    Assign(buffer, &a.states);


    fread(buffer, 100, 1, fp);
    Assign(buffer, &a.q0);

    fread(buffer, 100, 1, fp);
    Assign(buffer, &a.F);

    fread(buffer, 100, 1, fp);

    if(!(buffer = reallocarray(buffer, 1000, char_size))){
        err = ALLOCATION_ERROR;
        goto file;
    }

    size_t size = fread(buffer, char_size, 1000, fp);
    a.transitions = calloc(char_size, size+1);
    memcpy(a.transitions, buffer, size+1);

    file:
    fclose(fp);
    mem:
    free(buffer);

    return err;
}

void InitMachine(Machine machine){

}
