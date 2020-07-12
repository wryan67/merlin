#pragma once

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#ifndef MERLIN_LIGHTS

#define MERLIN_LIGHTS 11
#define maxNotes 11

#define OFF       0
#define RED      88
#define GREEN   248  
#define BLUE    168
#define CYAN    216 
#endif

#ifndef NULL
#define NULL 0
#endif

extern bool debug;

void startGame(int game);

pthread_t threadCreate(void* (*method)(void*), const char* description, void* args);
int random(int low, int high);

