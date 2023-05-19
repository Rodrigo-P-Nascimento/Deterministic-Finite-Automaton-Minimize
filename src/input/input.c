//
// Created by ekank on 5/13/23.
//

#include "input.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define BUFFER_SIZE 100
#define STR_SIZE 20

#define char_size sizeof(char)
#define String char*

typedef struct{
    String* alphabet;    //str[]
    String* states;      //str[]
    String q0;           //str
    String* F;           //str[]
    String* transitions; //str[]
} DFA_file;

static DFA_file a = {};

static inline void Strip(char* src, String* restrict dst){

    strtok(src, ":");
    *dst = strtok(NULL, ":");
}

static void Tokenize(char* src, String** dst){

    uint8_t size = strcspn(src, ",");
    uint8_t elem_qtty = ((strlen(src)+1)/(size+1));
    String token;

    if (strlen(src)%2 != 1)return;
    if(*dst) return;

    *dst = calloc(sizeof(char), elem_qtty);
    for(uint8_t i=0; i<elem_qtty; ++i){
        *dst[i] = calloc(char_size, size+1);
    }

    uint8_t i = 0;
    while((token = strtok_r(src, ",", &src))){
        strcpy(*dst[i++], token);
    }

}

static inline void Assign(char* buf, String** restrict dst){

    String data = NULL;

    Strip(buf, &data);

    if(!data){
        //TODO tratar valor
    }

    Tokenize(data, dst);
}

error_t ReadFile(char* path){

    error_t err = OK;

    char *buffer = calloc(BUFFER_SIZE, char_size);
    if (!buffer){
        err = ALLOCATION_ERROR;
        goto mem;
    }

    FILE* fp = fopen(path, "r");
    if (!fp){
        err = FILE_DOESNT_EXIST;
        goto file;
    }

    fread(buffer, BUFFER_SIZE, 1, fp);
    Assign(buffer, &a.alphabet);

    fread(buffer, BUFFER_SIZE, 1, fp);
    Assign(buffer, &a.states);


    fread(buffer, BUFFER_SIZE, 1, fp);
    {
        strtok(buffer, ":");
        char* tkn = strtok(NULL, ":");
        a.q0 = calloc(sizeof(String), strlen(tkn));
        strcpy(a.q0, tkn);
    }


    fread(buffer, BUFFER_SIZE, 1, fp);
    Assign(buffer, &a.F);

    fread(buffer, BUFFER_SIZE, 1, fp);

    if(!(buffer = reallocarray(buffer, BUFFER_SIZE*10, char_size))){
        err = ALLOCATION_ERROR;
        goto file;
    }

    assert(fread(buffer, char_size, 2000, fp) < BUFFER_SIZE*10);

    char aux[BUFFER_SIZE][20];
    char *tkn = strtok(buffer, "\n");
    {
        uint8_t read = 0;

        if (!tkn){
            do {
                memcpy(aux[read++], tkn, strlen(tkn) + 1);
                tkn = strtok(buffer, "\n");
                assert(read < BUFFER_SIZE);
            } while (tkn);
        }
        a.transitions = calloc(char_size, read * STR_SIZE);
        memcpy(a.transitions, aux, read * STR_SIZE);
    }


    file:
    fclose(fp);
    mem:
    free(buffer);

    return err;
}

error_t Validate(){

}

void InitMachine(Machine machine){


}
