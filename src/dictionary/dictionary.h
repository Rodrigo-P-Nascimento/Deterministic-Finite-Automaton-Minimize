//
// Created by ekank on 5/20/23.
//

#pragma once

#include <pthread.h>
#include <stdint.h>

#define TABLE_SIZE 100

typedef struct {char* state; char* symbol;} Transition;

typedef struct Entry {
    char* key;
    Transition** value;
    uint16_t arrSize;
    struct Entry* next;
} Entry;

typedef struct Dictionary {
    Entry* table[TABLE_SIZE];
    pthread_mutex_t mutex;
} Dictionary;

Dictionary* CreateDictionary(uint32_t nArr);
void Insert(Dictionary* dictionary, const char* key, Transition* value);
Transition** Find(Dictionary* dictionary, const char* key);
void DestroyDictionary(Dictionary* dictionary);
