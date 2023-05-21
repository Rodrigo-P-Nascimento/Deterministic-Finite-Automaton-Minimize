//
// Created by ekank on 5/20/23.
//

#include "dictionary.h"

#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char* key){
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
    new_entry->arrSize = 1;
    new_entry->value = calloc(sizeof(Transition*), arrSize);
    new_entry->value[new_entry->arrSize] = value;
    new_entry->next = NULL;

    return new_entry;
}

void Insert(Dictionary* dictionary, const char* key, Transition* value){
    unsigned int index = hash(key);

    if (dictionary->table[index] != NULL) {
        Entry* current = dictionary->table[index];

        pthread_mutex_lock(&(dictionary->mutex));
        while (current->next != NULL) {
            current = current->next;
            if(strcmp(current->key, key) == 0){
                current->value[current->arrSize++] = value;
                break;
            }
        }

        if (current->next == NULL){
            Entry* entry = NewEntry(key, value);
            pthread_mutex_lock(&(dictionary->mutex));
            current->next = entry;
        }
    } else {
        Entry* entry = NewEntry(key, value);

        pthread_mutex_lock(&(dictionary->mutex));
        dictionary->table[index] = entry;
    }

    pthread_mutex_unlock(&(dictionary->mutex));
}

Transition** Find(Dictionary* dictionary, const char* key){
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

    return NULL;
}

void DestroyDictionary(Dictionary* dictionary){
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Entry* current = dictionary->table[i];
        while (current != NULL) {
            Entry* next = current->next;
            free(current->key);
            for(uint32_t j =0 ; i < arrSize || current->value[j]->symbol[0] == '\0'; ++j ){
                free(current->value[j]->state);
                free(current->value[j]->symbol);
            }
            free(current->value);
            free(current);
            current = next;
        }
    }
    pthread_mutex_destroy(&(dictionary->mutex));
    free(dictionary);
}