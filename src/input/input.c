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
#define Wlen(strct) struct{strct data; uint32_t size;}
#define len(data) data.size

typedef Wlen(String*) String_l;

typedef struct {
    String_l alphabet;    // Array of strings representing the alphabet
    String_l states;      // Array of strings representing the states
    String q0;            // String representing the initial state
    String_l F;           // Array of strings representing the final states
    Dictionary* transitions;  // Dictionary storing the transitions
    /*
     *  Transition[q0] = [(q0, 0), (q1, 1), ...]
     *  Transition[q1] = [(q1, 0), (q1, 1), ...]
     */
} DFA_file;

static DFA_file a = {};

static inline void Strip(char* src, String* restrict dst) {
    // Extracts the string after ':' in each line of the file
    assert(strtok(src, ":") != NULL);
    *dst = strtok(NULL, ":");
}

static void Tokenize(char* src, String_l* dst) {
    // Splits a CSV string into an array of strings
    // "a,b,c" => ["a", "b", "c"]
    // "a1,b1,c1" => ["a1", "b1", "c1"]

    uint8_t size = strcspn(src, ",");
    dst->size = ((strlen(src) + 1) / (size + 1));
    String token;

//    if (strlen(src) % 2 != 1)
//        return;

    // Check if String[] is NULL
    assert(dst->data == NULL);

    // Allocate String[]
    dst->data = calloc(sizeof(String*), dst->size);

    //iterate through the CSV and put them in the array
    uint16_t i = 0;
    while ((token = strtok_r(src, ",", &src))) {
        dst->data[i++] = strdup(token);
    }
}

static inline void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

static inline void Assign(char* buf, String_l * restrict dst, bool allowEmpty){

    String data = NULL;

    remove_spaces(buf);

    Strip(buf, &data);

    if(!data){
        if(allowEmpty){
            printf("a maquina não possui estados finais, não faz sentido minimizar\n");
            exit(0);
        }
        fprintf(stderr, "entrada inválida, dados insuficientes");
        exit(EXIT_FAILURE);
    }

    Tokenize(data, dst);
}

static inline bool isIn(char* restrict s, char** arr, uint32_t size){
    for(uint32_t i = 0; i < size; ++i){
           if(strcmp(s, arr[i]) == 0){
               return true;
           }
    }

    return false;
}

static inline void Dispatch(void** arg){    // [Dictionary, key, Transition]
    Insert((Dictionary*)arg[0], (char*)arg[1], (Transition*)arg[2]);

    printf("transition %s -> %s @ %s processed\n", ((char*)arg[1]),
           ((Transition*)arg[2])->state, ((Transition*)arg[2])->symbol);

    free(arg[1]);
}

#define readLine(buffer, size, fp)          \
    do {                                    \
         fgets(buffer, BUFFER_SIZE, fp);    \
         buffer[strlen(buffer)-1] = '\0';   \
    }while(0) 

/**
 * Reads the DFA file from the given path and assigns values to the DFA_file structure.
 * Creates transitions in the dictionary using multithreading.
 *
 * @param path The path to the DFA file.
 * @return An error code indicating the success or failure of the operation.
 */
error_t ReadFile(char* path) {
    error_t err = OK;

    // Allocate buffer for reading lines from the file
    char *buffer = calloc(BUFFER_SIZE, char_size);
    if (!buffer) {
        err = ALLOCATION_ERROR;
        goto ret;
    }

    // Open the file for reading
    FILE* fp = fopen(path, "r");
    if (!fp) {
        err = FILE_DOESNT_EXIST;
        goto mem;
    }

    // Read the alphabet from the file and assign it to DFA_file.alphabet
    readLine(buffer, BUFFER_SIZE, fp);
    Assign(buffer, &a.alphabet, false);
    printf("alphabet read\n");

    // Read the states from the file and assign them to DFA_file.states
    readLine(buffer, BUFFER_SIZE, fp);
    Assign(buffer, &a.states, false);
    printf("states read\n");

    // Read the initial state from the file and assign it to DFA_file.q0
    readLine(buffer, BUFFER_SIZE, fp);
    {
        assert(strtok(buffer, ":") != NULL);
        char* tkn = strtok(NULL, ":");
        if (tkn == NULL){
            fprintf(stderr, "Maquina não possui estado incial");
            exit(EXIT_FAILURE);
        }
        a.q0 = strdup(tkn);
    }
    printf("initial state read\n");

    // Read the final states from the file and assign them to DFA_file.F
    readLine(buffer, BUFFER_SIZE, fp);
    Assign(buffer, &a.F, true);
    printf("final states read\n");

    // Read the transitions from the file and create dictionary entries using multithreading
    readLine(buffer, BUFFER_SIZE, fp);

    // Increase the buffer size if needed
    if (!(buffer = reallocarray(buffer, BUFFER_SIZE * 10, char_size))) {
        err = ALLOCATION_ERROR;
        goto file;
    }

    // Read the transitions from the file into the buffer
    uint32_t nread = fread(buffer, char_size, 2000, fp);
    assert( nread < BUFFER_SIZE * 10);
    if(nread < 11){
        fprintf(stderr, "Maquina não possui transições\n");
        exit(EXIT_FAILURE);
    }

    // Create the transitions dictionary
    a.transitions = CreateDictionary(len(a.alphabet));
    printf("transitions created\n");

    // Prepare the task for multithreading
    String reent;
    String token = strtok_r(buffer, "\n", &reent);

    // Process each line of the buffer as a transition and add it to the dictionary
     do{
        // Extract the state0, state1, and symbol from the token
        String state0 = strdup(strtok(token, ",")),
                state1 = strdup(strtok(NULL, ",")),
                symbol = strdup(strtok(NULL, ","));

         if (!isIn(state0, a.states.data, len(a.states)) ||
                 !isIn(state1, a.states.data, len(a.states)) ||
                 !isIn(symbol, a.alphabet.data, len(a.alphabet))) {

             fprintf(stderr, "transição inválida\n");
             exit(EXIT_FAILURE);
         }

         printf("read transition %s -> %s @ %s\n", state0, state1, symbol);
        // Create a Transition object
        Transition* tr = malloc(sizeof(Transition));
        tr->state = state1;
        tr->symbol = symbol;

        // Set the arguments for the Dispatch task
        void* args[] = {(void*)a.transitions,(void*)strdup(state0), (void*)tr};

        // Add the task to the task queue
        Dispatch(args);
    }while ((token = strtok_r(NULL, "\n", &reent)));

    file:
    // Close the file
    fclose(fp);

    mem:
    // Free the buffer memory
    free(buffer);

    ret:
    // Return the error code
    return err;
}

error_t Validate(void){

    return OK;
}

static inline uint32_t hash(const char* key){

    return key[0];
}

static inline Machine_stateID_t idx(char* src){
    for(uint32_t i = 0; i < len(a.states); ++i){
        if (!strcmp(a.states.data[i], src))
            return (Machine_stateID_t)i;
    }

    return (Machine_stateID_t)UINT32_MAX;
}

static inline void populateStates(Machine_state_t* state){

    assert(state->isFinal == false);
    //if the current state is in the final states array, sets the boolean to true
    for(uint32_t j = 0; j < len(a.F); ++j)
        if(strcmp(a.F.data[j], a.states.data[state->stateID.S]) == 0){
            state->isFinal = true;
            break;
        }

    //retrieve the transitions array and allocate the machine transitions array
    Transition_t transitions = Find(a.transitions, a.states.data[state->stateID.S]);
    if(transitions.arrSize != len(a.alphabet)){
        fprintf(stderr, "estado %s não possui todas as transições\n", a.states.data[state->stateID.S]);
        exit(EXIT_FAILURE);
    }
    assert(transitions.array != NULL);
    state->transitions.data = calloc(sizeof(Machine_Transition_t), transitions.arrSize);
    state->transitions.len = transitions.arrSize;

    //populate the machine transitions array
    for (uint32_t j = 0; j < transitions.arrSize; ++j){
        state->transitions.data[j].symbol = hash(transitions.array[j]->symbol);
        state->transitions.data[j].destState = idx(transitions.array[j]->state);

        if (state->transitions.data[j].destState.S == UINT32_MAX)
            //error, o estado destino da transição não está na lista de estados
            exit(EXIT_FAILURE);
    }

}

void InitMachine(Machine_t* machine){
    uint32_t i;

    //allocate the alphabet array and populate it with the hash of the string
    machine->alphabet.len = len(a.alphabet);
    machine->alphabet.data = calloc(sizeof(Machine_alphabet_t), len(a.alphabet));
    for(i = 0; i < len(a.alphabet); ++i){
        machine->alphabet.data[i] = hash(a.alphabet.data[i]);
    }

    //allocate the states array
    machine->states.data = calloc(sizeof(Machine_state_t), len(a.states));
    machine->states.len = len(a.states);

    for(i = 0; i < len(a.states); ++i){
        machine->states.data[i].stateID = (Machine_stateID_t)i;

        populateStates(&machine->states.data[i]);
    }
}

static inline void Destroy(String_l * str){

    for(uint32_t i = 0; i <= str->size; ++i){
        free((str->data)[i]);
    }
    free(str->data);
}

void DestroyDFA(void){
    Destroy(&a.alphabet);
    Destroy(&a.states);
    Destroy(&a.F);
    free(a.q0);

    DestroyDictionary(a.transitions);
}
