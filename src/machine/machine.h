//
// Created by ekank on 5/13/23.
//

#ifndef TDC_MACHINE_H
#define TDC_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

typedef union {
    uint32_t S;
    struct {
        uint16_t S1;
        uint16_t S2;
    };
} Machine_stateID_t;

typedef uint32_t Machine_alphabet_t;

typedef struct{
    Machine_stateID_t destState;
    Machine_alphabet_t symbol;
} Machine_Transition_t;

typedef struct {
    Machine_stateID_t stateID;
    struct {Machine_Transition_t* data; uint32_t len;} transitions;
    bool isFinal;
} Machine_state_t;

typedef struct {
    struct {Machine_alphabet_t* data; uint32_t len;} alphabet;
    struct {Machine_state_t* data; uint32_t len;} states;
    Machine_stateID_t initial;
}Machine_t;

#endif //TDC_MACHINE_H
