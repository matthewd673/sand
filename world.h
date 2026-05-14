#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>

typedef struct World World;

World *makeWorld();
void freeWorld(World *world);

void worldSet(World *world, int x, int y, int kind);
int worldGet(World *world, int x, int y);
int worldInBounds(World *world, int x, int y);

bool *worldGetActiveChunks(World *world);

void worldTick(World *world);

#endif
