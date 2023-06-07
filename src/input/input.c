//
// Created by ekank on 5/13/23.
//

#include "input.h"
#include "../dictionary/dictionary.h"
#include "../Threading/threading.h"
#include "../Threading/wait_group.h"

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

static inline void Assign(char* buf, String_l * restrict dst){

    String data = NULL;

    Strip(buf, &data);

    if(!data){
        //TODO tratar valor
    }

    Tokenize(data, dst);
}

static inline void Dispatch(void** arg){    // [Dictionary, key, Transition]
    Insert((Dictionary*)arg[0], (char*)arg[1], (Transition*)arg[2]);

    printf("transition %s -> %s @ %s processed\n", ((char*)arg[1]),
           ((Transition*)arg[2])->state, ((Transition*)arg[2])->symbol);

    free(arg[1]);
    WG_Done(arg[3]);
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
    Assign(buffer, &a.alphabet);
    printf("alphabet read\n");

    // Read the states from the file and assign them to DFA_file.states
    readLine(buffer, BUFFER_SIZE, fp);
    Assign(buffer, &a.states);
    printf("states read\n");

    // Read the initial state from the file and assign it to DFA_file.q0
    readLine(buffer, BUFFER_SIZE, fp);
    {
        assert(strtok(buffer, ":") != NULL);
        char* tkn = strtok(NULL, ":");
        a.q0 = strdup(tkn);
    }
    printf("initial state read\n");

    // Read the final states from the file and assign them to DFA_file.F
    readLine(buffer, BUFFER_SIZE, fp);
    Assign(buffer, &a.F);
    printf("final states read\n");

    // Read the transitions from the file and create dictionary entries using multithreading
    readLine(buffer, BUFFER_SIZE, fp);

    // Increase the buffer size if needed
    if (!(buffer = reallocarray(buffer, BUFFER_SIZE * 10, char_size))) {
        err = ALLOCATION_ERROR;
        goto file;
    }

    // Read the transitions from the file into the buffer
    assert(fread(buffer, char_size, 2000, fp) < BUFFER_SIZE * 10);

    // Create the transitions dictionary
    a.transitions = CreateDictionary(len(a.alphabet));
    printf("transitions created\n");

    // Prepare the task for multithreading
    String reent;
    String token = strtok_r(buffer, "\n", &reent);
    ThreadTask task = {.task = Dispatch, .args = calloc(4, sizeof(void*))};
    WaitGroup* wg = WG_New(0);

    // Process each line of the buffer as a transition and add it to the dictionary
     do{
        // Handle cases where the input is not in the form "state, state, symbol"
        // Handle cases where the symbol is not in the alphabet
        if (0) {
            goto waitgroup;
        }

        // Extract the state0, state1, and symbol from the token
        String state0 = strdup(strtok(token, ",")),
                state1 = strdup(strtok(NULL, ",")),
                symbol = strdup(strtok(NULL, ","));

         printf("read transition %s -> %s @ %s\n", state0, state1, symbol);
        // Create a Transition object
        Transition* tr = malloc(sizeof(Transition));
        tr->state = state1;
        tr->symbol = symbol;

        // Set the arguments for the Dispatch task
        task.args[0] = (void*)a.transitions;
        task.args[1] = (void*)strdup(state0);
        task.args[2] = (void*)tr;
        task.args[3] = (void*)wg;

        // Increment the WaitGroup counter
        WG_Add(wg, 1);

        // Add the task to the task queue
        Dispatch(task.args);
//        AddTask(task);
    }while ((token = strtok_r(NULL, "\n", &reent)));

    // Wait for all tasks to complete
    WG_Wait(wg);

    waitgroup:
    // Destroy the WaitGroup and free allocated resources
    WG_Destroy(wg);

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
    uint32_t hash = 0;
    const uint8_t factor = 31;

    while (*key) {
        hash = hash * factor + (*key);
        key++;
    }

    return hash % UINT32_MAX;
}

static inline Machine_stateID_t idx(char* src){
    for(uint32_t i = 0; i < len(a.states); ++i){
        if (!strcmp(a.states.data[i], src))
            return (Machine_stateID_t)i;
    }

    return (Machine_stateID_t)UINT32_MAX;
}

static inline void populateStates(void** args){
    Machine_state_t* state = (Machine_state_t*)args[0];

    //if the current state is in the final states array, sets the boolean to true
    for(uint32_t j = 0; j < len(a.F); ++j)
        state->isFinal = strcmp(a.F.data[j], a.states.data[state->stateID.S]) == 0? true: false;

    //retrieve the transitions array and allocate the machine transitions array
    Transition_t transitions = Find(a.transitions, a.states.data[state->stateID.S]);
    assert(transitions.array != NULL);
    state->transitions.data = calloc(sizeof(Machine_Transition_t), transitions.arrSize);
    state->transitions.len = transitions.arrSize;

    //populate the machine transitions array
    for (uint32_t j = 0; j < transitions.arrSize; ++j){
        state->transitions.data[j].symbol = hash(transitions.array[j]->symbol);
        state->transitions.data[j].destState = idx(transitions.array[j]->state);

        if (state->transitions.data[j].destState.S == UINT32_MAX)
            //error, o estado destino da transição não está na lista de estados
            exit(-1);
    }

    WG_Done(args[1]);
}

void InitMachine(Machine_t* machine){
    uint32_t i;

    //allocate the alphabet array and populate it with the hash of the string
    machine->alphabet.len = len(a.alphabet);
    machine->alphabet.data = calloc(sizeof(Machine_alphabet_t), len(a.alphabet));
    for(i = 0; i < len(a.alphabet); ++i){
        machine->alphabet.data[i] = hash(a.alphabet.data[i]);
    }

    ThreadTask task = {.task = populateStates, .args = calloc(2, sizeof(void*))};
    WaitGroup* wg = WG_New(0);

    //allocate the states array
    machine->states.data = calloc(sizeof(Machine_state_t), len(a.states));
    machine->states.len = len(a.states);

    for(i = 0; i < len(a.states); ++i){
        machine->states.data[i].stateID = (Machine_stateID_t)i;
        task.args[0] = &machine->states.data[i];
        task.args[1] = wg;

        WG_Add(wg, 1);
        populateStates(task.args);
//        AddTask(task);
    }

    WG_Wait(wg);
    WG_Destroy(wg);
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
