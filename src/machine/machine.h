//
// Created by ekank on 5/13/23.
//

#ifndef TDC_MACHINE_H
#define TDC_MACHINE_H

#include <stdint.h>

#define state_t uint8_t

typedef struct {
    state_t stateID;
    state_t* transitions;
} State;

typedef State* Machine;

#endif //TDC_MACHINE_H
