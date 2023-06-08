cmake_minimum_required(VERSION 3.21)
project(TDC C)

set(CMAKE_C_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(TDC
        src/main.c src/input/input.c src/input/input.h src/machine/machine.h src/Threading/threading.c src/Threading/threading.h src/dictionary/dictionary.c src/dictionary/dictionary.h src/Threading/wait_group.c src/Threading/wait_group.h src/Myhill-Nerode/minimization.c src/Myhill-Nerode/minimization.h src/Graph/graphMaker.c)

target_link_libraries(TDC PRIVATE ${CMAKE_THREAD_LIBS_INIT} -fsanitize=undefined -fsanitize=bounds -fsanitize=object-size gvc cgraph)

target_compile_options(TDC PRIVATE -Wall -Wextra -Werror -fsanitize=undefined -fsanitize=bounds -fsanitize=object-size)