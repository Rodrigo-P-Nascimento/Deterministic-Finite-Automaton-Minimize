//
// Created by ekank on 5/20/23.
//

#pragma once

#include <pthread.h>
#include <stdint.h>

#define TABLE_SIZE 100

typedef struct {char* state; char* symbol;} Transition;

typedef struct {
    Transition** array;
    uint16_t arrSize;
}Transition_t;

typedef struct Entry {
    char* key;
    Transition_t value;
    struct Entry* next;
} Entry;

typedef struct Dictionary {
    pthread_mutex_t mutex;
    Entry* table[TABLE_SIZE];
} Dictionary;

Dictionary* CreateDictionary(uint32_t nArr);
void Insert(Dictionary* dictionary, const char* key, Transition* value);
Transition_t Find(Dictionary* dictionary, const char* key);
void DestroyDictionary(Dictionary* dictionary);
