//
// Created by ekank on 5/13/23.
//

#ifndef TDC_INPUT_H
#define TDC_INPUT_H

#include <stdio.h>
#include "../machine/machine.h"

//TODO colocar os erros de leitura aqui
typedef enum {
    OK,
    FILE_DOESNT_EXIST,
    ALLOCATION_ERROR,
    INVALID
} error_t;

error_t ReadFile(char* path);
error_t Validate();
void InitMachine(Machine_t *machine);


#endif //TDC_INPUT_H
