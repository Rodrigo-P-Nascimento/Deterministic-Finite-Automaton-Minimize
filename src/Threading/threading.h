//
// Created by ekank on 5/19/23.
//

#pragma once

typedef struct {
    void (*task)(void**);
    void** args;
} ThreadTask;

void CreateThreadPool();
void DestroyThreadPool();
void AddTask(ThreadTask task);
