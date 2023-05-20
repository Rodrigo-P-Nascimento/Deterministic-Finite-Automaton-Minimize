//
// Created by ekank on 5/13/23.
//

#ifndef TDC_MACHINE_H
#define TDC_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#define State_t uint16_t
#define Alphabet_t uint32_t

typedef struct {
    Alphabet_t* alphabet;

    struct {
        State_t stateID;
        struct{State_t destState;Alphabet_t symbol;}* transitions;
        bool isFinal;
    }* states;
}Machine;

#endif //TDC_MACHINE_H
