#pragma once


#include <stdbool.h>

typedef struct Color{
  double r,g,b;
}Color;

typedef struct Texture{
  unsigned int width, height;
  Color* pixels;
}Texture;

//shelldisplay.c
void drawCanvas(Color (*pixelShader)(double, double), int width, int height);
void setColor(bool background, Color color);
void truecolor(bool background, Color color);
void clearFormatting();
void drawTexture(Texture t, int displayWidth, int displayHeight);

//shellpng.c
Texture loadPng(char* filename);

_Noreturn void raiseError(char* fmt, ...);
