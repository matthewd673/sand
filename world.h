#ifndef WORLD_H
#define WORLD_H

#define EMPTY 0
#define SAND  1

typedef struct World World;

World *makeWorld();
void freeWorld(World *world);

void worldSet(World *world, int x, int y, int value);
int worldGet(World *world, int x, int y);
int worldInBounds(World *world, int x, int y);

int *worldGetActiveChunks(World *world);

void worldTick(World *world);

#endif
