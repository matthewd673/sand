#include <stdlib.h>
#include "int_camera.h"

struct IntCamera {
  int x;
  int y;
  int width;
  int height;
  int half_width;
  int half_height;
};

IntCamera *makeIntCamera(int screen_width, int screen_height) {
  IntCamera *int_camera = (IntCamera *) malloc(sizeof(struct IntCamera));

  int half_width = screen_width / 2;
  int half_height = screen_height / 2;

  int_camera->x = half_width;
  int_camera->y = half_height;
  int_camera->width = screen_width;
  int_camera->height = screen_height;
  int_camera->half_width = half_width;
  int_camera->half_height = half_height;

  return int_camera;
}

void freeIntCamera(IntCamera *int_camera) {
  free(int_camera);
}

void intCameraMove(IntCamera *int_camera, int x_delta, int y_delta) {
  int_camera->x += x_delta;
  int_camera->y += y_delta;
}

int screenToWorldX(IntCamera *int_camera, int screen_x) {
  return screen_x + int_camera->x - int_camera->half_width;
}

int screenToWorldY(IntCamera *int_camera, int screen_y) {
  return screen_y + int_camera->y - int_camera->half_height;
}

int intCameraGetX(IntCamera *int_camera) {
  return int_camera->x;
}

int intCameraGetY(IntCamera *int_camera) {
  return int_camera->y;
}
