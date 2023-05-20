//
// Created by ekank on 5/20/23.
//

#pragma once

#include <pthread.h>

#define TABLE_SIZE 100

typedef struct {char* state; char* symbol;} Transition;

typedef struct Entry {
    char* key;
    Transition* value;
    struct Entry* next;
} Entry;

typedef struct Dictionary {
    Entry* table[TABLE_SIZE];
    pthread_mutex_t mutex;
} Dictionary;

Dictionary* CreateDictionary();
void Insert(Dictionary* dictionary, const char* key, Transition* value);
const Transition* find(Dictionary* dictionary, const char* key);
void DestroyDictionary(Dictionary* dictionary);
