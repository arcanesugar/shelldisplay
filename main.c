#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "shelldisplay.h"

#define PI 3.1415

typedef struct Vec3{
  double x, y, z;
}Vec3;

typedef struct Mat4x4{
  double m[4][4];//row, col (y, x)
}Mat4x4;

typedef struct Mesh{
  int vertexCount;
  int indexCount;
  Vec3 *vertices;
  int *indices;
}Mesh;

Vec3 multVecMat(Vec3 v, Mat4x4 m){
  Vec3 product;
  product.x = (m.m[0][0]*v.x) + (m.m[1][0]*v.y) + (m.m[2][0]*v.z) + m.m[3][0];
  product.y = m.m[0][1]*v.x + m.m[1][1]*v.y + m.m[2][1]*v.z + m.m[3][1];
  product.z = m.m[0][2]*v.x + m.m[1][2]*v.y + m.m[2][2]*v.z + m.m[3][2];
  double w  = m.m[0][3]*v.x + m.m[1][3]*v.y + m.m[2][3]*v.z + m.m[3][3];

  if(w!=0){
    product.x /= w;
    product.y /= w;
    product.z /= w;
  }

  return product;
}

Mat4x4 genProjMatrix(double fovRad){
  // 0 1 2 3
  // ---------
  // x 0 0 0 | 0
  // 0 y 0 0 | 1
  // 0 0 1 1 | 2
  // 0 0 0 0 | 3

  Mat4x4 m = {0};
  m.m[0][0] = 1.0/(tan(fovRad/2.0));
  m.m[1][1] = 1.0/(tan(fovRad/2.0));
  m.m[2][3] = 1.0;
  return m;
}

void initMesh(Mesh* m, int numVertices, int numIndices){
  m->vertexCount = numVertices;
  m->indexCount = numIndices;
  m->vertices = malloc(sizeof(Vec3)*numVertices);
  m->indices = malloc(sizeof(int)*numIndices);
}

void delay(double sec){
  usleep(sec*1000000);
}

void drawScreen(Texture* screen){
  printTexture(screen, 60, 60, UPPER_HALF_BLOCK, SAMPLE_NEAREST);
  printf("width: %d height: %d\n", screen->width, screen->height);
};

void drawPixel(Texture* screen, Color c, Vec3 a){
  a.x+=1.0;
  a.x/=2.0;
  a.y+=1.0;
  a.y/=2.0;
  int sx = a.x*screen->width;
  int sy = a.y*screen->height;
  if (sx<0) {printf("aah\n"); return;}
  if (sy<0) {printf("aah\n"); return;}
  if (sx>=screen->width) {printf("aah\n"); return;}
  if (sx>=screen->height) {printf("aah\n"); return;}
  screen->pixels[sy*screen->width+sx] = c;
}

void printMat(Mat4x4 m){
  for(int y = 0; y<4; y++){
    printf("|");
    for(int x = 0; x<4; x++){
      printf(" %f ", m.m[y][x]);
    }
    printf("| %d\n", y);
  }
}
void drawTriangle(Texture* screen, Color col, Vec3 a, Vec3 b, Vec3 c){
  drawPixel(screen, col, a);
  drawPixel(screen, col, b);
  drawPixel(screen, col, c);
}

int main(void) {
  Texture screen;
  screen.width = 60;
  screen.height = 60;
  screen.pixels = malloc(sizeof(Color)*screen.width*screen.height);
  //screen coordinate system
  //0,0------1,0
  //|         |
  //|         |
  //|         |
  //|         |
  //0,1------1,1
  
  Mat4x4 projMat = genProjMatrix(90*(PI/180));

  Mesh pyramid;
  initMesh(&pyramid, 4, 6);
  pyramid.vertices[0] = (Vec3){0, 0, 0};
  pyramid.vertices[1] = (Vec3){1, 0, 0};
  pyramid.vertices[2] = (Vec3){0, 1, 0};
  pyramid.vertices[3] = (Vec3){1, 1, 0};

  pyramid.indices[0] = 0;
  pyramid.indices[1] = 1;
  pyramid.indices[2] = 2;

  pyramid.indices[3] = 1;
  pyramid.indices[4] = 2;
  pyramid.indices[5] = 3;

  double offset = 2.3;
  double fTheta = 0;
  while(1){ 
    memset(screen.pixels, 0, sizeof(Color)*screen.width*screen.height);
    Mat4x4 matRotZ = {{0}};
    Mat4x4 matRotX = {{0}};
    
    fTheta+=0.3;
    
    matRotZ.m[0][0] = cos(fTheta);
    matRotZ.m[0][1] = sin(fTheta);
    matRotZ.m[1][0] = -sin(fTheta);
    matRotZ.m[1][1] = cos(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cos(fTheta * 0.5f);
    matRotX.m[1][2] = sin(fTheta * 0.5f);
    matRotX.m[2][1] = -sin(fTheta * 0.5f);
    matRotX.m[2][2] = cos(fTheta * 0.5f);
    matRotX.m[3][3] = 1;
    
    for(int i = 0; i<pyramid.indexCount; i+=3){
      int ai = pyramid.indices[i];
      int bi = pyramid.indices[i+1];
      int ci = pyramid.indices[i+2];

      Vec3 a = pyramid.vertices[ai];
      Vec3 b = pyramid.vertices[bi];
      Vec3 c = pyramid.vertices[ci];

      a = multVecMat(a, matRotX);
      b = multVecMat(b, matRotX);
      c = multVecMat(c, matRotX);

      a = multVecMat(a, matRotZ);
      b = multVecMat(b, matRotZ);
      c = multVecMat(c, matRotZ);

      a.z+=offset;
      b.z+=offset;
      c.z+=offset;

      a = multVecMat(a, projMat);
      b = multVecMat(b, projMat);
      c = multVecMat(c, projMat);

      drawTriangle(&screen, (Color){0, 1, 1}, a, b, c);
      if(i>=6){
        drawTriangle(&screen, (Color){1, 0, 1}, a, b, c);
      }
    }
    clearScreen();
    drawScreen(&screen);
    delay(0.1);
  }
  return 0;
}
