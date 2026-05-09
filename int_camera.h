#ifndef INT_CAMERA_H
#define INT_CAMERA_H

typedef struct IntCamera IntCamera;

IntCamera *makeIntCamera(int screen_width, int screen_height);
void freeIntCamera(IntCamera *int_camera);

void intCameraMove(IntCamera *int_camera, int x_delta, int y_delta);
int screenToWorldX(IntCamera *int_camera, int screen_x);
int screenToWorldY(IntCamera *int_camera, int screen_y);

int intCameraGetX(IntCamera *int_camera);
int intCameraGetY(IntCamera *int_camera);

#endif
