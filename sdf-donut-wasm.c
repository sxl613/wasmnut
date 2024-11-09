#include <emscripten.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define RADIUS 0.5f
#define THICKNESS 0.3f
#define HEIGHT 40
#define WIDTH 120
#define MAX_STEPS 30
#define SM 0.01f
#define EPSILON 0.001f

// Global buffer for output
char donut_buffer[HEIGHT * (WIDTH + 1) + 1];
char sphere_buffer[HEIGHT * (WIDTH + 1) + 1];
char octa_buffer[HEIGHT * (WIDTH + 1) + 1];
char sierpinski_buffer[HEIGHT * (WIDTH + 1) + 1];

typedef struct Vec3 {
  float x;
  float y;
  float z;
} Vec3;

int c = 0;

Vec3 normal(float (*f)(float x, float y, float z), float x, float y, float z) {
  float dx = f(x + EPSILON, y, z) - f(x - EPSILON, y, z);
  float dy = f(x, y + EPSILON, z) - f(x, y - EPSILON, z);
  float dz = f(x, y, z + EPSILON) - f(x, y, z - EPSILON);
  double norm = sqrtf(dx * dx + dy * dy + dz * dz);
  Vec3 r = {.x = dx / norm, .y = dy / norm, .z = dz / norm};
  return r;
}

float circle(float x, float y) { return sqrtf(x * x + y * y) - RADIUS; }

float donut_2d(float x, float y) {

  return fabsf(sqrtf(x * x + y * y) - RADIUS) - ((float)THICKNESS / 2);
}

float sphere(float x, float y, float z) {

  return sqrtf(x * x + y * y + z * z) - RADIUS;
}
float donut_3d(float x, float y, float z) {
  float xy_d = sqrtf(x * x + y * y) - RADIUS;
  float d = sqrtf(xy_d * xy_d + z * z);
  return d - (float)THICKNESS / 2;
}

char sample(float x, float y) {
  if (circle(x, y) <= 0) {
    return '#';
  }
  return ' ';
}

char samplef(float x, float y, float (*f)(float x, float y)) {
  if ((*f)(x, y) <= 0) {
    return '#';
  }
  return ' ';
}

char surface(float (*f)(float x, float y, float z), float x, float y, float z) {
  Vec3 r = normal(f, x, y, z);
  int is_lit = r.y < -0.25f;
  int is_frosted = r.z < -0.5;
  if (is_frosted) {
    if (is_lit)
      return '@';
    else
      return '#';
  } else {
    if (is_lit)
      return '=';
    else
      return '.';
  }
}

char samplef3d(float x, float y, float (*f)(float x, float y, float z)) {
  float z = -10.0f;
  for (int st = 0; st < MAX_STEPS; st++) {
    float theta = fmod(c * 0.003f, 2 * M_PI);
    float xx = x * cosf((float)theta) - z * sinf((float)theta);
    float zz = x * sinf((float)theta) + z * cosf((float)theta);
    float d = (*f)(xx, y, zz);
    if (d <= SM) {
      return surface(f, xx, y, zz);
    } else
      z += d;
  }
  return ' ';
}

// Octahedron looks great with ASCII shading
float octahedron(float x, float y, float z) {
  x = fabsf(x);
  y = fabsf(y);
  z = fabsf(z);
  return (x + y + z - RADIUS) * 0.57735027f;
}

void loop(char *buffer, float (*f)(float, float y, float z)) {
  printf("\033[H");
  char *ptr = buffer;
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      float rx = ((float)x / WIDTH) * 2 - 1;
      float ry = (((float)y / HEIGHT) * 2 - 1) * (2 * (float)HEIGHT / WIDTH);
      *ptr++ = samplef3d(rx, ry, f);
    }
    *ptr++ = '\n';
  }
  *ptr = '\0';
  c++;
}

// Capsule - looks nice when rotated
float capsule(float x, float y, float z) {
  // Line segment from p1 to p2
  float p1x = -0.5f, p1y = 0, p1z = 0;
  float p2x = 0.5f, p2y = 0, p2z = 0;

  // Vector arithmetic
  float dx = p2x - p1x, dy = p2y - p1y, dz = p2z - p1z;
  float px = x - p1x, py = y - p1y, pz = z - p1z;
  float h = fmaxf(0, fminf(1, (px * dx + py * dy + pz * dz) /
                                  (dx * dx + dy * dy + dz * dz)));

  return sqrtf((px - dx * h) * (px - dx * h) + (py - dy * h) * (py - dy * h) +
               (pz - dz * h) * (pz - dz * h)) -
         THICKNESS;
}

// Wave deformation
float wave(float x, float y, float z) {
  float wave = 0.1f * sinf(3.0f * x + c * 0.1f);
  return sphere(x, y - wave, z);
}

EMSCRIPTEN_KEEPALIVE
const char *get_donut_frame() {
  loop(donut_buffer, &donut_3d);
  return donut_buffer;
}

EMSCRIPTEN_KEEPALIVE
const char *get_sphere_frame() {
  loop(sphere_buffer, &sphere);
  return sphere_buffer;
}

EMSCRIPTEN_KEEPALIVE
const char *get_octa_frame() {
  loop(octa_buffer, &octahedron);
  return octa_buffer;
}

EMSCRIPTEN_KEEPALIVE
const char *get_sierpinski_frame() {
  loop(sierpinski_buffer, &wave);
  return sierpinski_buffer;
}

int main() { return 0; }
