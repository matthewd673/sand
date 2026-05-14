#include <stdlib.h>
#include <stdio.h> // For debugging
#include <stdbool.h>
#include "world.h"
#include "cell.h"

// TODO: Infinite?
const int CHUNK_SIZE = 32;
const int X_CHUNKS = 160;
const int Y_CHUNKS = 40;

// Physics
const float GRAVITY_ACC = .2f;
const float SPAWN_VEL = 1.f;
const float TERMINAL_VEL = 6.f;

struct World {
  bool *active_chunks;
  int *cells;
  float *vels;
};

World *makeWorld() {
  World *world = (World *) malloc(sizeof(World));
  if (world == NULL) {
    return NULL;
  }

  int num_cells = X_CHUNKS * Y_CHUNKS * CHUNK_SIZE * CHUNK_SIZE;
  world->cells = (int *) calloc(num_cells, sizeof(int));
  if (world->cells == NULL) {
    return NULL;
  }

  world->vels = (float *) calloc(num_cells, sizeof(float));
  if (world->vels == NULL) {
    return NULL;
  }

  world->active_chunks = calloc(X_CHUNKS * Y_CHUNKS, sizeof(int));
  if (world->active_chunks == NULL) {
    return NULL;
  }

  return world;
}

void freeWorld(World *world) {
  free(world->cells);
  free(world->active_chunks);
  free(world);
}

int xyToLocalInd(int x, int y) {
  return (y % CHUNK_SIZE) * CHUNK_SIZE + (x % CHUNK_SIZE);
}

int xyToInd(int x, int y) {
  return y * CHUNK_SIZE * X_CHUNKS + x;
}

int xyToChunkInd(int x, int y) {
  return y / CHUNK_SIZE * X_CHUNKS + x / CHUNK_SIZE;
}

int chunkXYToChunkInd(int chunk_x, int chunk_y) {
  return chunk_y * X_CHUNKS + chunk_x;
}

bool isChunkActive(World *world, int chunk_x, int chunk_y) {
  return world->active_chunks[chunkXYToChunkInd(chunk_x, chunk_y)];
}

void activateChunk(World *world, int chunk_x, int chunk_y) {
  for (int x = chunk_x - 1; x <= chunk_x + 1; x += 1) {
    for (int y = chunk_y - 1; y <= chunk_y + 1; y += 1) {
      if (x < 0 || x >= X_CHUNKS || y < 0 || y >= Y_CHUNKS) {
        continue;
      }

      int chunk_ind = chunkXYToChunkInd(x, y);
      world->active_chunks[chunk_ind] = true;
    }
  }
}

void deactivateChunk(World *world, int chunk_x, int chunk_y) {
  world->active_chunks[chunkXYToChunkInd(chunk_x, chunk_y)] = false;
}

void activateChunkByCellXY(World *world, int cell_x, int cell_y) {
  activateChunk(world, cell_x / CHUNK_SIZE, cell_y / CHUNK_SIZE);
}

// Assumes x and y are in bounds
void worldSet(World *world, int x, int y, int kind) {
  activateChunkByCellXY(world, x, y);

  int ind = xyToInd(x, y);
  world->cells[ind] = kind;
  world->vels[ind] = SPAWN_VEL;
}

// Assumes x and y are in bounds
int worldGet(World *world, int x, int y) {
  return world->cells[xyToInd(x, y)];
}

int worldInBounds(World *world, int x, int y) {
  return
    x > 0 && x < X_CHUNKS * CHUNK_SIZE &&
    y > 0 && y < Y_CHUNKS * CHUNK_SIZE;
}

bool *worldGetActiveChunks(World *world) {
  return world->active_chunks;
}

// Assumes inds in bounds, target is empty
void swap(World *world, int cell_x, int cell_y, int target_x, int target_y) {
  // Cell is trying to swap with itself
  if (cell_x == target_x && cell_y == target_y) {
    return;
  }

  int cell_ind = xyToInd(cell_x, cell_y);
  int target_ind = xyToInd(target_x, target_y);

  // Swap
  int cell_kind = world->cells[cell_ind];
  float cell_vel = world->vels[cell_ind];
  world->cells[cell_ind] = world->cells[target_ind];
  world->cells[target_ind] = cell_kind;
  world->vels[target_ind] = cell_vel;

  // Activate chunks
  activateChunkByCellXY(world, cell_x, cell_y);
  activateChunkByCellXY(world, target_x, target_y);
}

void velCollisionDetect(
    World *world,
    int cell_x,
    int cell_y,
    float vel,
    int *out_x,
    int *out_y) {
  for (int i = 1; i <= (int) vel; i += 1) {
    // Cell will go out of bounds
    if (cell_y + i == Y_CHUNKS * CHUNK_SIZE) {
      *out_y = cell_y + i - 1; // Place it at the very bottom
      return;
    }

    if (worldGet(world, cell_x, cell_y + i) != EMPTY) {
      *out_y = cell_y + i - 1;

      // Try to fall left and right
      if (worldInBounds(world, cell_x - 1, *out_y + 1) &&
          worldGet(world, cell_x - 1, *out_y + 1) == EMPTY) {
        *out_x -= 1;
        *out_y += 1;
        return;
      }

      if (worldInBounds(world, cell_x + 1, *out_y + 1) &&
          worldGet(world, cell_x + 1, *out_y + 1) == EMPTY) {
        *out_x += 1;
        *out_y += 1;
        return;
      }

      return;
    }
  }

  *out_y = cell_y + (int) vel;
}

void worldTick(World *world) {
  for (int chunk_x = 0; chunk_x < X_CHUNKS; chunk_x += 1) {
    for (int chunk_y = Y_CHUNKS - 1; chunk_y >= 0; chunk_y -= 1) {
      // Skip if this chunk is inactive
      if (!isChunkActive(world, chunk_x, chunk_y)) {
        continue;
      }

      // This chunk will be reactivated if something happens
      deactivateChunk(world, chunk_x, chunk_y);

      for (int local_x = 0; local_x < CHUNK_SIZE; local_x += 1) {
        for (int local_y = CHUNK_SIZE - 1; local_y >= 0; local_y -= 1) {
          int cell_x = chunk_x * CHUNK_SIZE + local_x;
          int cell_y = chunk_y * CHUNK_SIZE + local_y;
          int ind = xyToInd(cell_x, cell_y);

          int cell = worldGet(world, cell_x, cell_y);

          if (cell == EMPTY) {
            continue;
          }

          if (cell == SAND) {
            // Apply acceleration
            if (world->vels[ind] < TERMINAL_VEL) {
              world->vels[ind] += GRAVITY_ACC;
            }
            float cell_vel = world->vels[ind];

            int below_x = cell_x;
            int below_y = cell_y;
            velCollisionDetect(world, cell_x, cell_y, cell_vel, &below_x, &below_y);

            swap(world, cell_x, cell_y, below_x, below_y);
          }
        }
      }
    }
  }
}
