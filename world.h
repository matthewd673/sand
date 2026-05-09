#ifndef WORLD_H
#define WORLD_H

#define EMPTY 0
#define SAND  1

typedef struct World World;

World *makeWorld(int width, int height);
void freeWorld(World *world);

void worldSet(World *world, int x, int y, int value);
int worldGet(World *world, int x, int y);
int worldInBounds(World *world, int x, int y);

#endif
