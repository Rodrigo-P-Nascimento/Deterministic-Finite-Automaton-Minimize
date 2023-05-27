//
// Created by ekank on 5/13/23.
//

#include "machine/machine.h"
#include "input/input.h"

int main(int argc, char** argv){

    if(argc != 2){
        //sem arquivo
    }

    Machine_t machine = {};
    ReadFile(argv[1]);
    InitMachine(&machine);

    return 0;
}