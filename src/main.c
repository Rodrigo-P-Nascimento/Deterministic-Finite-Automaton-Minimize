//
// Created by ekank on 5/13/23.
//

#include <stdlib.h>
#include <stdio.h>

#include "machine/machine.h"
#include "input/input.h"
#include "Threading/threading.h"

int main(int argc, char** argv){

    if(argc != 2){
        //sem arquivo
        exit(-1);
    }

    CreateThreadPool();

    Machine_t machine = {};
    error_t err = ReadFile(argv[1]);
    if(err){
        printf("%d", err);
    }
    printf("input file read");
    InitMachine(&machine);

    return 0;
}