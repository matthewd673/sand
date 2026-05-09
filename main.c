#include <stdio.h>
#include "raylib.h"
#include "world.h"
#include "int_camera.h"

const int CAMERA_SPEED = 4;

int main(void) {
  World *world = makeWorld();

  // Raylib window
  const int window_width = 800;
  const int window_height = 800;

  InitWindow(window_width, window_height, "sand");
  SetTargetFPS(60);

  // To stringify metrics for rendering
  char fps_str[32];
  char frame_time_str[32];
  char camera_str[32];

  IntCamera *camera = makeIntCamera(window_width, window_height);

  while (!WindowShouldClose()) {
    // Input
    int mouse_world_x = screenToWorldX(camera, GetMouseX());
    int mouse_world_y = screenToWorldY(camera, GetMouseY());
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        worldInBounds(world, mouse_world_x, mouse_world_y)) {
      worldSet(world, mouse_world_x, mouse_world_y, 1);
    }

    if (IsKeyDown(KEY_LEFT)) {
      intCameraMove(camera, -CAMERA_SPEED, 0);
    }
    if (IsKeyDown(KEY_RIGHT)) {
      intCameraMove(camera, CAMERA_SPEED, 0);
    }
    if (IsKeyDown(KEY_UP)) {
      intCameraMove(camera, 0, -CAMERA_SPEED);
    }
    if (IsKeyDown(KEY_DOWN)) {
      intCameraMove(camera, 0, CAMERA_SPEED);
    }

    // Update
    worldTick(world);

    // Draw
    BeginDrawing();
    ClearBackground(BLACK);

    for (int screen_y = 0; screen_y < window_height; screen_y += 1) {
      for (int screen_x = 0; screen_x < window_width; screen_x += 1) {
        int world_x = screenToWorldX(camera, screen_x);
        int world_y = screenToWorldY(camera, screen_y);

        if (!worldInBounds(world, world_x, world_y)) {
          continue;
        }

        if (worldGet(world, world_x, world_y) == SAND) {
          DrawPixel(screen_x, screen_y, YELLOW);
        }
      }
    }

    sprintf(fps_str, "FPS: %d", GetFPS());
    sprintf(frame_time_str, "Frame time: %.2fms", GetFrameTime() * 1000);
    sprintf(
        camera_str,
        "Camera: %d, %d",
        intCameraGetX(camera),
        intCameraGetY(camera));
    DrawText(fps_str, 0, 0, 8, WHITE);
    DrawText(frame_time_str, 0, 8, 8, WHITE);
    DrawText(camera_str, 0, 16, 8, WHITE);

    // TODO: Hard-coded assuming we know world chunk size
    int *active_chunks = worldGetActiveChunks(world);
    int chunk_y = 0;
    int chunk_x = 0;
    for (int i = 0; i < 100; i += 1) {
      DrawRectangle(
          chunk_x * 10,
          32 + chunk_y * 10,
          10,
          10,
          active_chunks[i] == 0 ? DARKGRAY: GRAY);

      chunk_x += 1;
      if (chunk_x == 10) {
        chunk_y += 1;
        chunk_x = 0;
      }
    }

    EndDrawing();
  }

  // Cleanup
  CloseWindow();
  freeWorld(world);
  freeIntCamera(camera);

  return 0;
}
