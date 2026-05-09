#include <stdlib.h>
#include "world.h"

typedef struct World {
  int width;
  int height;
  int *cells;
} World;

World *makeWorld(int width, int height) {
  World *world = (World *) malloc(sizeof(World));
  if (world == NULL) {
    return NULL;
  }

  int *cells = (int *) calloc(width * height, sizeof(int));
  if (cells == NULL) {
    return NULL;
  }

  world->width = width;
  world->height = height;
  world->cells = cells;
  return world;
}

void freeWorld(World *world) {
  free(world->cells);
  free(world);
}

int coordToInd(World *world, int x, int y) {
  return y * world->width + x;
}

void worldSet(World *world, int x, int y, int value) {
  // Assumes x and y are in bounds
  int index = coordToInd(world, x, y);
  world->cells[index] = value;
}

int worldGet(World *world, int x, int y) {
  // Assumes x and y are in bounds
  int index = coordToInd(world, x, y);
  return world->cells[index];
}

int worldInBounds(World *world, int x, int y) {
  return x > 0 && x < world->width && y > 0 && y < world->height;
}
