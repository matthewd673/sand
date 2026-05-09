#include <stdio.h>
#include "raylib.h"
#include "world.h"

typedef struct IntCamera {
  int x;
  int y;
  int width;
  int height;
  int half_width;
  int half_height;
} IntCamera;

int main(void) {
  // Make world
  const int world_width = 1600;
  const int world_height = 800;

  World *world = makeWorld(world_width, world_height);

  // Raylib window
  const int window_width = 1600;
  const int window_height = 800;

  InitWindow(window_width, window_height, "sand");
  SetTargetFPS(60);

  // To stringify metrics for rendering
  char fps_str[32];
  char frame_time_str[32];
  char camera_str[32];

  IntCamera camera = {
    .x = world_width / 2,
    .y = world_height / 2,
    .width = window_width,
    .height = window_height,
    .half_width = window_width / 2,
    .half_height = window_height / 2,
  };

  while (!WindowShouldClose()) {
    // Input
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        worldInBounds(world, GetMouseX(), GetMouseY())) {
      worldSet(world, GetMouseX(), GetMouseY(), 1);
    }

    if (IsKeyDown(KEY_LEFT)) {
      camera.x -= 1;
    }
    if (IsKeyDown(KEY_RIGHT)) {
      camera.x += 1;
    }
    if (IsKeyDown(KEY_UP)) {
      camera.y -= 1;
    }
    if (IsKeyDown(KEY_DOWN)) {
      camera.y += 1;
    }

    // Update
    for (int y = world_height - 1; y > 0; y -= 1) {
      for (int x = 0; x < world_width; x += 1) {
        if (worldGet(world, x, y) != EMPTY) {
          continue;
        }

        // Sand above, pull down
        if (worldGet(world, x, y - 1) == SAND) {
          worldSet(world, x, y, SAND);
          worldSet(world, x, y - 1, EMPTY);
          continue;
        }

        // Sand piling from the left
        if (x > 0 &&
            worldGet(world, x - 1, y) != EMPTY &&
            worldGet(world, x - 1, y - 1) != EMPTY) {
          worldSet(world, x - 1, y - 1, EMPTY);
          worldSet(world, x, y, SAND);
          continue;
        }

        // Sand piling from the right
        if (x < world_width - 1 &&
            worldGet(world, x + 1, y) != EMPTY &&
            worldGet(world, x + 1, y - 1) != EMPTY) {
          worldSet(world, x + 1, y - 1, EMPTY);
          worldSet(world, x, y, SAND);
          continue;
        }
      }
    }

    // Draw
    BeginDrawing();
    ClearBackground(BLACK);

    for (int y = 0; y < camera.height; y += 1) {
      for (int x = 0; x < camera.width; x += 1) {
        if (!worldInBounds(world, x, y)) {
          DrawPixel(x, y, GRAY);
          continue;
        }

        if (worldGet(world, x, y) == SAND) {
          DrawPixel(x, y, YELLOW);
        }
      }
    }

    sprintf(fps_str, "FPS: %d", GetFPS());
    sprintf(frame_time_str, "Frame time: %.4f", GetFrameTime());
    sprintf(camera_str, "Camera: %d, %d", camera.x, camera.y);
    DrawText(fps_str, 0, 0, 8, WHITE);
    DrawText(frame_time_str, 0, 8, 8, WHITE);
    DrawText(camera_str, 0, 16, 8, WHITE);

    EndDrawing();
  }

  // Cleanup
  CloseWindow();
  freeWorld(world);

  return 0;
}
