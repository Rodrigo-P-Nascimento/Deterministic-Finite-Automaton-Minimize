//
// Created by ekank on 27/05/23.
//

#include "minimization.h"

#include <stdlib.h>
#include <assert.h>

#define TransitionDest(state) machine->states.data[state].transitions.data

static inline Machine_stateID_t isIn(Machine_stateID_t item, Machine_stateID_t * restrict arr, uint32_t size){
    for(uint32_t i = 0; i < size; ++i){
        if(item.S == arr[i].S1 || item.S == arr[i].S2 ) {
            return arr[i];
        }
    }

    return (Machine_stateID_t){.S = 0};
}

void Minimize(Machine_t* machine){

    uint16_t statesDiff = 0;
    //create table
    bool** table = calloc(sizeof(bool*), machine->states.len);
    assert(table[0] == NULL);

    for (uint32_t i = 1; i < machine->states.len; ++i){
        table[i] = calloc(sizeof(bool), i);
        statesDiff += i;
    }

    //fill table
    for(uint32_t i = 1; i < machine->states.len; ++i){
        for(uint32_t j = 0; j < i; ++j){
            if(machine->states.data[i].isFinal != machine->states.data[j].isFinal) {
                table[i][j] = true;
                --statesDiff;
            }
        }
    }


    //check the unchecked pairs
    bool minimal = false;
    while (!minimal) {
        minimal = true;
        for (uint32_t i = 1; i < machine->states.len; ++i) {
            for (uint32_t j = 0; j < i; ++j) {
                if (!table[i][j]) {
                    for (uint32_t k = 0; k < machine->alphabet.len; ++k) {
                        Machine_stateID_t id1 = TransitionDest(i)[k].destState;
                        Machine_stateID_t id2 = TransitionDest(j)[k].destState;

                        if (table[id1.S][id2.S] || table[id2.S][id1.S]) {
                            table[i][j] = true;
                            --statesDiff;
                            minimal = false;
                            break;
                        }
                    }
                }
            }
        }
    }

    //machine is already minimal
    if(statesDiff == 0) return;

    Machine_stateID_t combined[statesDiff];
    uint16_t idx = 0;

    //combine unmarked pairs
    for(uint32_t i = 1; i < machine->states.len; ++i) {
        for (uint32_t j = 0; j < i; ++j) {
            if(!table[i][j]){
                combined[idx++] = (Machine_stateID_t){.S1 = j, .S2 = i};
            }
        }
    }
    assert(idx == (statesDiff));

    assert(statesDiff < machine->states.len);
    Machine_state_t* aux = calloc(sizeof(Machine_state_t),machine->states.len-statesDiff);
    idx = 0;

    //iterating through the states
    for(uint32_t i = 0; i < machine->states.len; ++i) {
        //if state already processed, skip
        Machine_state_t* data = machine->states.data;
        if(data[i].transitions.data == NULL){
            continue;
        }

        //if state combined
        Machine_stateID_t id  = isIn(machine->states.data[i].stateID, combined, statesDiff);
        if(id.S){
            aux[idx].stateID = id;
            if(data[i].stateID.S == (uint32_t)id.S1){
                free(data[id.S2].transitions.data);
                data[id.S2].transitions.data = NULL;
            }else{
                free(data[id.S1].transitions.data);
                data[id.S1].transitions.data = NULL;
            }
            aux[idx].isFinal = data[id.S1].isFinal || data[id.S2].isFinal?
                    true:
                    false;
        }else{
            aux[idx].stateID = data[i].stateID;
            aux[idx].isFinal = data[i].isFinal;
        }

        //iterating each transition based on the alphabet
        for(uint32_t k = 0; k < machine->alphabet.len; ++k) {
            id = TransitionDest(i)[k].destState;

            //if the transition leads to a combined state, swap for the combined state
            id = isIn(id, combined, statesDiff);
            if(id.S){
                TransitionDest(i)[k].destState = id;
            }
        }

        aux[idx].transitions =  data[i].transitions;
        assert(idx < machine->states.len-statesDiff);
        ++idx;
    }

    //change the machine
    free(machine->states.data);
    machine->states.data = aux;
    machine->states.len = idx;

    for (uint32_t i = 1; i < machine->states.len; ++i){
        free(table[i]);
    }
    free(table);
}
