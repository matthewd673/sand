#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"
#include "world.h"
#include "cell.h"
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
  char mouse_pos_str[32];
  char hover_str[32];

  float update_time = 1.f;
  float render_time = 1.f;

  IntCamera *camera = makeIntCamera(window_width, window_height);

  int brush_size = 1;

  while (!WindowShouldClose()) {
    // Input
    int mouse_world_x = screenToWorldX(camera, GetMouseX());
    int mouse_world_y = screenToWorldY(camera, GetMouseY());
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      for (int i = 0; i <= brush_size; i += 1) {
        for (int j = 0; j <= brush_size; j += 1) {
          if (worldInBounds(world, mouse_world_x + i, mouse_world_y + j)) {
            worldSet(world, mouse_world_x + i, mouse_world_y + j, 1);
          }
        }
      }
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

    if (IsKeyPressed(KEY_EQUAL)) {
      brush_size += 1;
    }

    // Update
    update_time = GetTime();
    worldTick(world);
    update_time = GetTime() - update_time;

    // Draw
    render_time = GetTime();
    BeginDrawing();
    ClearBackground(BLACK);

    // Particles
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
        // Chunk viz; TODO: Hardcoded assuming chunk size
        else if (world_x % 32 == 0 || world_y % 32 == 0) {
          DrawPixel(screen_x, screen_y, DARKGRAY);
        }
      }
    }

    render_time = GetTime() - render_time;

    // Metrics
    sprintf(fps_str, "FPS: %d", GetFPS());
    sprintf(frame_time_str, "Frame time: %.2fms", GetFrameTime() * 1000);
    sprintf(
        camera_str,
        "Camera: %d, %d",
        intCameraGetX(camera),
        intCameraGetY(camera));
    sprintf(mouse_pos_str, "Mouse: %d, %d", mouse_world_x, mouse_world_y);
    if (worldInBounds(world, mouse_world_x, mouse_world_y)) {
      sprintf(
          hover_str,
          "Hovering on: %s",
          cellGetName(worldGet(world, mouse_world_x, mouse_world_y)));
    } else {
      sprintf(hover_str, "Out of bounds");
    }

    DrawText(fps_str, 0, 0, 8, WHITE);
    DrawText(frame_time_str, 0, 8, 8, WHITE);
    DrawText(camera_str, 0, 16, 8, WHITE);
    DrawText(mouse_pos_str, 0, 24, 8, WHITE);
    DrawText(hover_str, 0, 32, 8, WHITE);

    // TODO: Hard-coded assuming we know world chunk size
    bool *active_chunks = worldGetActiveChunks(world);
    int chunk_y = 0;
    int chunk_x = 0;
    for (int i = 0; i < 6400; i += 1) {
      DrawPixel(
          chunk_x,
          64 + chunk_y,
          active_chunks[i] == 0 ? DARKGRAY: GRAY);

      chunk_x += 1;
      if (chunk_x == 160) {
        chunk_y += 1;
        chunk_x = 0;
      }
    }

    int time_scaling = 2000;
    DrawRectangle(0, 128, (int) (update_time * time_scaling), 10, DARKBLUE);
    DrawRectangle((int) (update_time * time_scaling), 128, (int) (render_time * 1000), 10, MAROON);
    DrawRectangle((int) (.016f * time_scaling), 128, 1, 10, GREEN);

    EndDrawing();
  }

  // Cleanup
  CloseWindow();
  freeWorld(world);
  freeIntCamera(camera);

  return 0;
}
