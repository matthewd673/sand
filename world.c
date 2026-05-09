#include <stdlib.h>
#include "world.h"

const int CHUNK_SIZE = 256;

// TODO: Infinite
const int X_CHUNKS = 10;
const int Y_CHUNKS = 10;

struct World {
  int *active_chunks;
  int **chunks;
};

World *makeWorld() { World *world = (World *) malloc(sizeof(World));
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
  world->chunks[chunk_ind][xyToLocalInd(x, y)] = SAND;
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

void worldTick(World *world) {
  for (int chunk_x = 0; chunk_x < X_CHUNKS; chunk_x += 1) {
    for (int chunk_y = Y_CHUNKS - 1; chunk_y >= 0; chunk_y -= 1) {
      int did_update = 0;

      int chunk_ind = chunkXYToInd(chunk_x, chunk_y);
      if (world->active_chunks[chunk_ind] == 0) {
        continue;
      }

      // Deactivate this chunk, it will be reactivated if something happens
      world->active_chunks[chunk_ind] = 0;

      for (int local_x = 0; local_x < CHUNK_SIZE; local_x += 1) {
        for (int local_y = CHUNK_SIZE - 1; local_y >= 0; local_y -= 1) {
          int local_ind = xyToLocalInd(local_x, local_y);
          int *cell = &world->chunks[chunk_ind][local_ind];

          if (cell == EMPTY) {
            continue;
          }

          if (*cell == SAND) {
            int below_chunk_ind = chunk_ind;
            int below_ind = xyToLocalInd(local_x, local_y + 1);
            int *below = NULL;

            // Fall to the next chunk
            if (local_y + 1 >= CHUNK_SIZE) {
              // Bottom of the world
              if (chunk_y == Y_CHUNKS - 1) {
                continue;
              }

              below_ind = xyToLocalInd(local_x, 0);
              below_chunk_ind = chunkXYToInd(chunk_x, chunk_y + 1);
            }

            below = &world->chunks[below_chunk_ind][below_ind];

            if (*below == EMPTY) {
              *below = SAND;
              *cell = EMPTY;
              world->active_chunks[chunk_ind] = 1;
              world->active_chunks[below_chunk_ind] = 1;
            }
          }
        }
      }
    }
  }
}
