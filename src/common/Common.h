#pragma once

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#ifndef MERLIN_LIGHTS

#define MERLIN_LIGHTS  11
#define MERLIN_BUTTONS 30
#define maxNotes 11

#define NEW_GAME       20
#define SAME_GAME      21
#define HIT_ME         22
#define COMP_TURN      23

#define OFF         0
//#define RED      88
//#define GREEN   248  
//#define BLUE    168
#define CYAN      216 
#endif

#ifndef NULL
#define NULL 0
#endif

extern bool debug;

void startGame(int game);

unsigned long long currentTimeMillis();

int random(int low, int high);

