//
// Created by ekank on 5/13/23.
//

#ifndef TDC_MACHINE_H
#define TDC_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#define Machine_stateID_t uint32_t
#define Machine_alphabet_t uint32_t

typedef struct{
    Machine_stateID_t destState;
    Machine_alphabet_t symbol;
} Machine_Transition_t;

typedef struct {
    Machine_stateID_t stateID;
    Machine_Transition_t* transitions;
    bool isFinal;
} Machine_state_t;

typedef struct {
    Machine_alphabet_t* alphabet;
    Machine_state_t* states;
    Machine_stateID_t initial;
}Machine_t;

#endif //TDC_MACHINE_H
