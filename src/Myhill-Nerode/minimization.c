//
// Created by ekank on 27/05/23.
//

#include "minimization.h"

#include <stdlib.h>
#include <assert.h>

#define TransitionDest(state) machine->states.data[state].transitions.data

static inline Machine_stateID_t isIn(Machine_stateID_t item, Machine_stateID_t * restrict arr, uint32_t size){
    for(uint32_t i = 0; i < size; ++i){
        if(item.S == arr[i].S1) {
            assert(item.S != arr[i].S2);
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
        for (uint32_t j = 0; j < i; ++j){
            table[i] = calloc(sizeof(bool), i);
            statesDiff += i;
        }
    }

    //fill table
    for(uint32_t i = 1; i < machine->states.len; ++i){
        if(machine->states.data[i].isFinal){
            for(uint32_t j = 0; j < i; ++j){
                if(!machine->states.data[j].isFinal) {
                    table[i][j] = true;
                    --statesDiff;
                }
            }
        }
    }


    //check the unchecked pairs
    for(uint32_t i = 1; i < machine->states.len; ++i) {
        for (uint32_t j = 0; j < i; ++j) {
            if(!table[i][j]){
                for(uint32_t k = 0; k < machine->alphabet.len; ++k){
                    Machine_stateID_t id1 = TransitionDest(i)[k].destState;
                    Machine_stateID_t id2 = TransitionDest(j)[k].destState;

                    if(id2.S < id1.S && table[id1.S][id2.S]){
                        table[i][j] = true;
                        --statesDiff;
                        break;
                    }
                }
            }
        }
    }

    //machine is already minimal
    if(statesDiff == 0) return;

    Machine_stateID_t combined[statesDiff];
    uint32_t idx = 0;

    //combine unmarked pairs
    for(uint32_t i = 1; i < machine->states.len; ++i) {
        for (uint32_t j = 0; j < i; ++j) {
            if(!table[i][j]){
                combined[idx++] = (Machine_stateID_t){.S1 = i, .S2 = j};
            }
        }
    }
    assert(idx == (statesDiff-1));

    assert(statesDiff < machine->states.len);
    Machine_state_t aux[machine->states.len-statesDiff];
    idx = 0;

    //iterating through the states
    for(uint32_t i = 1; i < machine->states.len; ++i) {
        //if state already processed, skip
        if(machine->states.data[i].transitions.data == NULL){
            continue;
        }

        //if state combined
        Machine_stateID_t id  = isIn(machine->states.data[i].stateID, combined, statesDiff);
        if(id.S){
            aux[idx].stateID = id;
            free(machine->states.data[id.S2].transitions.data);
            machine->states.data[id.S2].transitions.data = NULL;
            aux[idx].isFinal = machine->states.data[id.S1].isFinal || machine->states.data[id.S2].isFinal?
                    true:
                    false;
        }else{
            aux[idx].stateID = machine->states.data[i].stateID;
            aux[idx].isFinal = machine->states.data[i].isFinal;
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

        aux[idx].transitions =  machine->states.data[i].transitions;
        ++idx;
    }

    //change the machine
    free(machine->states.data);
    machine->states.data = aux;
    machine->states.len = machine->states.len-statesDiff;

    free(table);
}
