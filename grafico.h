#ifndef GRAFICO_H
#define GRAFICO_H

#include <stdio.h>
#include <SDL/SDL.h>
#include <math.h>
#include <pthread.h>

#include "cronometro.h"
#include "struct_geral.h"

#define BPP 32
typedef Uint32 PixelType;


#define INTERVALO_CICLO_MS 100

typedef struct canvas {
  SDL_Surface *canvas;
  int Height; // canvas height
  int Width;  // canvas width
  int Xoffset; // X off set, in canvas pixels
  int Yoffset; // Y off set, in canvas pixels
  int Xext; // X extra width
  int Yext; // Y extra height
  double Xmax;
  double Ymax;
  double Xstep; // half a distance between X pixels in 'Xmax' scale

  PixelType *zpixel;

} Tcanvas;

typedef struct dataholder {
  Tcanvas *canvas;
  double   Tcurrent;
  double   Lcurrent;
  PixelType Lcolor;
  double   INcurrent;
  PixelType INcolor;
  double   OUTcurrent;
  PixelType OUTcolor;

} Tdataholder;

void* grafico(void* ponteiroDados);

#endif