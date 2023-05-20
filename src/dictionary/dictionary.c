//
// Created by ekank on 5/20/23.
//

#include "dictionary.h"

#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char* key){
    unsigned int hash_value = 0;
    u_int32_t len = strlen(key);
    for (u_int32_t i = 0; i < len; ++i) {
        hash_value += key[i];
        hash_value = (hash_value << 5) ^ (hash_value >> 27);
    }
    return hash_value % TABLE_SIZE;
}

Dictionary* CreateDictionary(){
    Dictionary* dictionary = (Dictionary*)malloc(sizeof(Dictionary));
    memset(dictionary, 0, sizeof(Dictionary));
    pthread_mutex_init(&(dictionary->mutex), NULL);
    return dictionary;
}

void Insert(Dictionary* dictionary, const char* key, Transition* value){
    unsigned int index = hash(key);
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = NULL;

    pthread_mutex_lock(&(dictionary->mutex));

    if (dictionary->table[index] == NULL) {
        dictionary->table[index] = new_entry;
    } else {
        Entry* current = dictionary->table[index];
        while (current->next != NULL)
            current = current->next;
        current->next = new_entry;
    }

    pthread_mutex_unlock(&(dictionary->mutex));
}

const Transition* find(Dictionary* dictionary, const char* key){
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
            free(current->value->state);
            free(current->value->symbol);
            free(current->value);
            free(current);
            current = next;
        }
    }
    pthread_mutex_destroy(&(dictionary->mutex));
    free(dictionary);
}