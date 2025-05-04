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
Color sampleTexture(Texture *t, double x, double y, int method);
Color lerpColors(Color a, Color b, double x);

//shellpng.c
Texture loadPng(char* filename);

_Noreturn void raiseError(char* fmt, ...);
