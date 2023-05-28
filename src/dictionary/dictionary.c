//
// Created by ekank on 5/20/23.
//

#include "dictionary.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static inline unsigned int hash(const char* key){
    unsigned int hash = 0;
    unsigned int factor = 31;

    while (*key) {
        hash = hash * factor + (*key);
        key++;
    }

    return hash % TABLE_SIZE;
}

uint32_t arrSize = 0;

Dictionary* CreateDictionary(uint32_t nArr){

    arrSize = nArr;

    Dictionary* dictionary = (Dictionary*)malloc(sizeof(Dictionary));
    memset(dictionary, 0, sizeof(Dictionary));
    pthread_mutex_init(&(dictionary->mutex), NULL);
    return dictionary;
}

static inline Entry* NewEntry(const char* key, Transition* value) {
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value.arrSize = 1;
    new_entry->value.array = calloc(sizeof(Transition*), arrSize);
    new_entry->value.array[new_entry->value.arrSize-1] = value;
    new_entry->next = NULL;

    return new_entry;
}

void Insert(Dictionary* dictionary, const char* key, Transition* value){
    unsigned int index = hash(key);

    Entry** current = &dictionary->table[index];

    if (*current) {
        pthread_mutex_lock(&(dictionary->mutex));
        do {
            //if found the key, update the array
            if (strcmp((*current)->key, key) == 0) {
                (*current)->value.array[(*current)->value.arrSize++] = value;
                pthread_mutex_unlock(&(dictionary->mutex));
                return;
            }
            //else, continue going through de list
        } while ((*current = (*current)->next));
        pthread_mutex_unlock(&(dictionary->mutex));
    }

    assert((*current) == NULL);
    Entry* entry = NewEntry(key, value);

    pthread_mutex_lock(&(dictionary->mutex));
    *current = entry;
    pthread_mutex_unlock(&(dictionary->mutex));
}

Transition_t Find(Dictionary* dictionary, const char* key){
    unsigned int index = hash(key);

    pthread_mutex_lock(&(dictionary->mutex));

    Entry* current = dictionary->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            pthread_mutex_unlock(&(dictionary->mutex));
            return current->value;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&(dictionary->mutex));

    Transition_t error_ret = {.array = NULL, .arrSize = 0};
    return error_ret;
}

void DestroyDictionary(Dictionary* dictionary){
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Entry* current = dictionary->table[i];
        while (current != NULL) {
            Entry* next = current->next;
            free(current->key);
            for(uint32_t j =0 ; i < arrSize || current->value.array[j]->symbol[0] == '\0'; ++j ){
                free(current->value.array[j]->state);
                free(current->value.array[j]->symbol);
            }
            free(current->value.array);
            free(current);
            current = next;
        }
    }
    pthread_mutex_destroy(&(dictionary->mutex));
    free(dictionary);
}