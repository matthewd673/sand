#include <stdlib.h>
#include <stdio.h> // For debugging
#include "world.h"
#include "cell.h"

// TODO: Infinite?
const int CHUNK_SIZE = 256;
const int X_CHUNKS = 20;
const int Y_CHUNKS = 5;

// Physics
const float GRAVITY_ACC = .2f;
const float SPAWN_VEL = 1.f;
const float TERMINAL_VEL = 6.f;

struct World {
  int *active_chunks;
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

// Assumes x and y are in bounds
void worldSet(World *world, int x, int y, int kind) {
  world->active_chunks[xyToChunkInd(x, y)] = 1;

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

int *worldGetActiveChunks(World *world) {
  return world->active_chunks;
}

// Assumes inds in bounds, target is empty
void swap(World *world, int cell_x, int cell_y, int target_x, int target_y) {
  int cell_ind = xyToInd(cell_x, cell_y);
  int target_ind = xyToInd(target_x, target_y);

  // Swap
  int cell_kind = world->cells[cell_ind];
  float cell_vel = world->vels[cell_ind];
  world->cells[cell_ind] = world->cells[target_ind];
  world->cells[target_ind] = cell_kind;
  world->vels[target_ind] = cell_vel;

  // Activate chunks
  world->active_chunks[xyToChunkInd(cell_x, cell_y)] = 1;
  world->active_chunks[xyToChunkInd(target_x, target_y)] = 1;
}

int velCollisionDetect(World *world, int cell_x, int cell_y, float vel) {
  for (int i = 0; i < (int) vel; i += 1) {
    if (cell_y + i == Y_CHUNKS * CHUNK_SIZE - 1) {
      return cell_y + i;
    }

    if (i != 0 && worldGet(world, cell_x, cell_y + i) != EMPTY) {
      return cell_y + i;
    }
  }

  return cell_y + (int) vel;
}

void worldTick(World *world) {
  for (int chunk_x = 0; chunk_x < X_CHUNKS; chunk_x += 1) {
    for (int chunk_y = Y_CHUNKS - 1; chunk_y >= 0; chunk_y -= 1) {
      int chunk_ind = chunkXYToChunkInd(chunk_x, chunk_y);
      if (world->active_chunks[chunk_ind] == 0) {
        continue;
      }

      // Deactivate this chunk, it will be reactivated if something happens
      world->active_chunks[chunk_ind] = 0;

      for (int local_x = 0; local_x < CHUNK_SIZE; local_x += 1) {
        for (int local_y = CHUNK_SIZE - 1; local_y >= 0; local_y -= 1) {
          int cell_x = chunk_x * CHUNK_SIZE + local_x;
          int cell_y = chunk_y * CHUNK_SIZE + local_y;
          int ind = xyToInd(cell_x, cell_y);

          int cell = worldGet(world, cell_x, cell_y);

          if (cell == EMPTY) {
            continue;
          }

          // TODO: Ugly and repetitive
          if (cell == SAND) {
            // Apply acceleration
            if (world->vels[ind] < TERMINAL_VEL) {
              world->vels[ind] += GRAVITY_ACC;
            }
            float cell_vel = world->vels[ind];

            int below_x = cell_x;
            int below_y = velCollisionDetect(world, cell_x, cell_y, cell_vel);

            // Cell is stationary
            if (below_y == cell_y) {
              continue;
            }

            if (worldInBounds(world, below_x, below_y)) {
              int below = worldGet(world, below_x, below_y);
              if (below == EMPTY) {
                // Move cell
                swap(world, cell_x, cell_y, below_x, below_y);
                continue;
              }
            }

            int left_x = cell_x - 1;
            int left_y = cell_y + 1;
            if (worldInBounds(world, left_x, left_y)) {
              int left = worldGet(world, left_x, left_y);
              if (left == EMPTY) {
                swap(world, cell_x, cell_y, left_x, left_y);
                continue;
              }
            }

            int right_x = cell_x + 1;
            int right_y = cell_y + 1;
            if (worldInBounds(world, right_x, right_y)) {
              int right = worldGet(world, right_x, right_y);
              if (right == EMPTY) {
                swap(world, cell_x, cell_y, right_x, right_y);
                continue;
              }
            }
          }
        }
      }
    }
  }
}
