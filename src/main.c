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

    switch(err){
        case OK:
            printf("File opened!"); // This case is executed when the file is successfully opened.
            break;
        case FILE_DOESNT_EXIST:
            printf("File does not exist!"); // This case is executed when the file does not exist.
            return -1; //Returning -1 indicates an error occurred due to the file not existing.
        case ALLOCATION_ERROR:
            printf("Allocation Memory Error!"); // This case is executed when there is a memory allocation error.
            return -2; //Returning -2 indicates an error occurred due to memory allocation failure.
        case INVALID:
            printf("This file is invalid!"); // This case is executed when the file is invalid.
            return -3; //Returning -3 indicates an error occurred due to an invalid file.
    }

    printf("input file read");
    InitMachine(&machine);

    return 0;
}