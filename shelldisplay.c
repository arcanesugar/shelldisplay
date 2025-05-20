#include "shelldisplay.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

static int colorMode = CM_ANSICOLOR;

void setColorMode(int mode){
  colorMode = mode;
}

void setStyle(int style){
  printf("\x1b[0");
  if(style&S_BOLD) printf(";1");
  if(style&S_DIM) printf(";2");
  if(style&S_ITALIC) printf(";3");
  if(style&S_UNDERLINE) printf(";4");
  if(style&S_BLINK) printf(";5");
  if(style&S_INVERSE) printf(";7");
  if(style&S_HIDDEN) printf(";8");
  if(style&S_STRIKETHROUGH) printf(";9");
  printf("m");
}

_Noreturn void raiseError(char* fmt, ...){
  fprintf(stderr, "Err: ");
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  exit(EXIT_FAILURE);
}

static void truecolor(bool background, Color color);
static void ansicolor(bool background, Color color);

void setColor(bool background, Color color){
  switch(colorMode){
    case CM_ANSICOLOR:
      ansicolor(background, color);
    break;
    case CM_TRUECOLOR:
      truecolor(background, color);
    break;
    default:
    raiseError("Invalid color mode\n");
  }
}

void ansicolor(bool background, Color color){
  int r = color.r*5.0;
  int g = color.g*5.0;
  int b = color.b*5.0;
  int colorID = 16 + 36*r + 6*g + b;
  if(background) printf("\x1b[48;5;%dm", colorID);
  else printf("\x1b[38;5;%dm", colorID);
}

void truecolor(bool background, Color color){
  int r = color.r*255.0;
  int g = color.g*255.0;
  int b = color.b*255.0;
  if(background) printf("\x1b[48;2;%d;%d;%dm", r, g, b);
  else printf("\x1b[38;2;%d;%d;%dm", r, g, b);
}

void resetColor(){
  printf("\x1b[39;49;m");
}

void clearScreen(){
  printf("\033[2J");//clear screen
  printf("\033[H");//return cursor home
}

Color lerpColors(Color a, Color b, double x){
  if(x<0 || x>1) raiseError("lerpColors(): x out of range (%f)", x);
  Color c;
  c.r = (a.r+(b.r-a.r)*x);
  c.g = (a.g+(b.g-a.g)*x);
  c.b = (a.b+(b.b-a.b)*x);
  return c;
}

bool compareColors(Color a, Color b){
  if(a.r != b.r) return false;
  if(a.g != b.g) return false;
  if(a.b != b.b) return false;
  return true;
}
