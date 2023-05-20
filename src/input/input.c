//
// Created by ekank on 5/13/23.
//

#include "input.h"
#include "../dictionary/dictionary.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define BUFFER_SIZE 100

#define char_size sizeof(char)
typedef char* String;
#define Wlen(strct) struct{strct data; uint16_t size;}
#define len(data) data.size
typedef Wlen(String*) String_l;

typedef struct{
    String_l alphabet;    //str[]
    String_l states;      //str[]
    String q0;                 //str
    String_l F;           //str[]
    Dictionary* transitions;
    /*
     *  Transition[q0] = [(q0, 0), (q1,1), ...]
     *  Transition[q1] = [(q1, 0), (q1,1), ...]
     */
} DFA_file;

static DFA_file a = {};

static inline void Strip(char* src, String* restrict dst){

    assert(strtok(src, ":") != NULL);
    *dst = strtok(NULL, ":");
}

static void Tokenize(char* src, String_l* dst){

    uint8_t size = strcspn(src, ",");
    dst->size = ((strlen(src)+1)/(size+1));
    String token;

    if (strlen(src)%2 != 1)return;
    //String[] == NULL
    assert(dst->data == NULL);

    //alloc String[]
    dst->data = calloc(sizeof(String_l), dst->size);

    uint16_t i = 0;
    while((token = strtok_r(src, ",", &src))){
        dst->data[i++] = strdup(token);
    }
}

static inline void Assign(char* buf, String_l * restrict dst){

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
        assert(strtok(buffer, ":") != NULL);
        char* tkn = strtok(NULL, ":");
        a.q0 = strdup(tkn);
    }


    fread(buffer, BUFFER_SIZE, 1, fp);
    Assign(buffer, &a.F);

    fread(buffer, BUFFER_SIZE, 1, fp);

    if(!(buffer = reallocarray(buffer, BUFFER_SIZE*10, char_size))){
        err = ALLOCATION_ERROR;
        goto file;
    }

    assert(fread(buffer, char_size, 2000, fp) < BUFFER_SIZE*10);
    a.transitions = CreateDictionary();

    String token;
    while ((token = strtok_r(buffer, "\n", &buffer))){
        //tratar quando a entrada não for: state, state, symbol
        //tratar quando o símbolo não estiver no alfabeto
        String state0 = strdup(strtok(token, ",")),
            state1 = strdup(strtok(token, ",")),
            symbol = strdup(strtok(token, ","));

        Transition* tr = malloc(sizeof(Transition));
        tr->state = state1; tr->symbol = symbol;

        Insert(a.transitions, state0, tr);
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

static inline void Destroy(String_l * str){

    for(uint16_t i = 0; i <= str->size; ++i){
        free((str->data)[i]);
    }
    free(str->data);
}

void DestroyDFA(){
    Destroy(&a.alphabet);
    Destroy(&a.states);
    Destroy(&a.F);
    free(a.q0);

    DestroyDictionary(a.transitions);
}