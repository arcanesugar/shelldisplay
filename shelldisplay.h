#pragma once


#include <stdbool.h>

typedef struct Color{
  double r,g,b;
}Color;

typedef struct Texture{
  unsigned int width, height;
  Color* pixels;
}Texture;

enum SampleMethods{
  SAMPLE_NEAREST,
  SAMPLE_BILINEAR
};

//shelldisplay.c
void setColor(bool background, Color color);
void truecolor(bool background, Color color);
void clearFormatting();
void clearScreen();
Color lerpColors(Color a, Color b, double x);
bool compareColors(Color a, Color b);

//texture.c
Texture createTexture(int width, int height);
void freeTexture(Texture t);
void setPixel(Texture t, int x, int y, Color c);
Color getPixel(Texture t, int x, int y);
void fillTexture(Texture t, Color c);
Color sampleTexture(Texture t, double x, double y, int method);
Texture resampleTexture(Texture t, int width, int height, int method);

enum PixelTypes{
  //pixel mapping descriptions assume texture size is the same as display size
  PIX_DOUBLE_SPACE,//2x1char maps 1->1
  PIX_SINGLE_SPACE,//1x1 char, pixels are rectangular (horizontal res is double vertical res)
  PIX_UPPER_HALF_BLOCK,//uses unicode to divide one character into two pixels
};

void printTexture(Texture t, int pixelType);

//shellpng.c
Texture loadPng(char* filename);

_Noreturn void raiseError(char* fmt, ...);