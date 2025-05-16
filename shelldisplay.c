#include "shelldisplay.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

_Noreturn void raiseError(char* fmt, ...){
  fprintf(stderr, "Err: ");
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  exit(EXIT_FAILURE);
}

void setColor(bool background, Color color){
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

void clearFormatting(){
  printf("\x1b[0m");
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
