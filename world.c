#include <stdlib.h>
#include "world.h"

const int CHUNK_SIZE = 256;

// TODO: Infinite
const int X_CHUNKS = 20;
const int Y_CHUNKS = 5;

struct World {
  int *active_chunks;
  int **chunks;
};

World *makeWorld() {
  World *world = (World *) malloc(sizeof(World));
  if (world == NULL) {
    return NULL;
  }

  world->chunks = calloc(X_CHUNKS * Y_CHUNKS, sizeof(int *));
  if (world->chunks == NULL) {
    return NULL;
  }

  for (int i = 0; i < X_CHUNKS * Y_CHUNKS; i += 1) {
    world->chunks[i] = calloc(CHUNK_SIZE * CHUNK_SIZE, sizeof(int));
    if (world->chunks[i] == NULL) {
      return NULL;
    }
  }

  world->active_chunks = calloc(X_CHUNKS * Y_CHUNKS, sizeof(int));
  if (world->active_chunks == NULL) {
    return NULL;
  }

  return world;
}

void freeWorld(World *world) {
  for (int i = 0; i < X_CHUNKS * Y_CHUNKS; i += 1) {
    free(world->chunks[i]);
  }
  free(world->chunks);
  free(world->active_chunks);
  free(world);
}

int chunkXYToInd(int x, int y) {
  return y * X_CHUNKS + x;
}

int coordToChunk(int x_or_y) {
  return x_or_y / CHUNK_SIZE;
}

int xyToLocalInd(int x, int y) {
  return (y % CHUNK_SIZE) * CHUNK_SIZE + (x % CHUNK_SIZE);
}

// Assumes x and y are in bounds
void worldSet(World *world, int x, int y, int value) {
  // Activate the chunk
  int chunk_ind = chunkXYToInd(
      coordToChunk(x),
      coordToChunk(y));

  world->active_chunks[chunk_ind] = 1;
  world->chunks[chunk_ind][xyToLocalInd(x, y)] = value;
}

// Assumes x and y are in bounds
int worldGet(World *world, int x, int y) {
  int chunk_ind = chunkXYToInd(
      coordToChunk(x),
      coordToChunk(y));
  return world->chunks[chunk_ind][xyToLocalInd(x, y)];
}

int worldInBounds(World *world, int x, int y) {
  return
    x > 0 && x < X_CHUNKS * CHUNK_SIZE &&
    y > 0 && y < Y_CHUNKS * CHUNK_SIZE;
}

int *worldGetActiveChunks(World *world) {
  return world->active_chunks;
}

// Assumes in bounds
int *getRef(World *world, int chunk_x, int chunk_y, int local_x, int local_y) {
  int chunk_ind = chunkXYToInd(chunk_x, chunk_y);
  int local_ind = xyToLocalInd(local_x, local_y);
  return &world->chunks[chunk_ind][local_ind];
}

void worldTick(World *world) {
  for (int chunk_x = 0; chunk_x < X_CHUNKS; chunk_x += 1) {
    for (int chunk_y = Y_CHUNKS - 1; chunk_y >= 0; chunk_y -= 1) {
      int chunk_ind = chunkXYToInd(chunk_x, chunk_y);
      if (world->active_chunks[chunk_ind] == 0) {
        continue;
      }

      // Deactivate this chunk, it will be reactivated if something happens
      world->active_chunks[chunk_ind] = 0;

      for (int local_x = 0; local_x < CHUNK_SIZE; local_x += 1) {
        for (int local_y = CHUNK_SIZE - 1; local_y >= 0; local_y -= 1) {
          int cell_x = chunk_x * CHUNK_SIZE + local_x;
          int cell_y = chunk_y * CHUNK_SIZE + local_y;
          int cell = worldGet(world, cell_x, cell_y);

          if (cell == EMPTY) {
            continue;
          }

          if (cell == SAND) {
            // TODO: Repetitive
            int below_x = cell_x;
            int below_y = cell_y + 1;
            if (worldInBounds(world, below_x, below_y)) {
              int below = worldGet(world, below_x, below_y);
              if (below == EMPTY) {
                worldSet(world, below_x, below_y, SAND);
                worldSet(world, cell_x, cell_y, EMPTY);
                continue;
              }
            }

            int left_x = cell_x - 1;
            int left_y = cell_y + 1;
            if (worldInBounds(world, left_x, left_y)) {
              int left = worldGet(world, left_x, left_y);
              if (left == EMPTY) {
                worldSet(world, left_x, left_y, SAND);
                worldSet(world, cell_x, cell_y, EMPTY);
                continue;
              }
            }

            int right_x = cell_x + 1;
            int right_y = cell_y + 1;
            if (worldInBounds(world, right_x, right_y)) {
              int right = worldGet(world, right_x, right_y);
              if (right == EMPTY) {
                worldSet(world, right_x, right_y, SAND);
                worldSet(world, cell_x, cell_y, EMPTY);
                continue;
              }
            }
          }
        }
      }
    }
  }
}
