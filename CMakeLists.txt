cmake_minimum_required(VERSION 3.25)
project(TDC C)

set(CMAKE_C_STANDARD 11)

add_executable(TDC
        src/main.c src/input/input.c src/input/input.h src/machine/machine.h src/Threading/threading.c src/Threading/threading.h src/dictionary/dictionary.c src/dictionary/dictionary.h src/Threading/wait_group.c src/Threading/wait_group.h src/Myhill-Nerode/minimization.c src/Myhill-Nerode/minimization.h)
