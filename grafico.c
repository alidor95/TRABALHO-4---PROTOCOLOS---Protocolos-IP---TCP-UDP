/* 
Codigo fornecido pelo professor n aqui sera feito as correções para fazer o grafico do nosso trabalho. 
primeiro passo é retirar o int
*/

#include "grafico.h"

#define SCREEN_W 640 //tamanho da janela que sera criada
#define SCREEN_H 640

//#define BPP 8
//typedef Uint8 PixelType;
//#define BPP 16
//typedef Uint16 PixelType;


 void c_pixeldraw(Tcanvas *canvas, int x, int y, PixelType color)
{
  *( ((PixelType*)canvas->canvas->pixels) + ((-y+canvas->Yoffset) * canvas->canvas->w + x+ canvas->Xoffset)) = color;
}

 void c_hlinedraw(Tcanvas *canvas, int xstep, int y, PixelType color)
{
  int offset =  (-y+canvas->Yoffset) * canvas->canvas->w;
  int x;

  for (x = 0; x< canvas->Width+canvas->Xoffset ; x+=xstep) {
        *( ((PixelType*)canvas->canvas->pixels) + (offset + x)) = color;
  }
}
 void c_vlinedraw(Tcanvas *canvas, int x, int ystep, PixelType color)
{
  int offset = x+canvas->Xoffset;
  int y;
  int Ystep = ystep*canvas->canvas->w;

  for (y = 0; y< canvas->Height+canvas->Yext ; y+=ystep) {
    *( ((PixelType*)canvas->canvas->pixels) + (offset + y*canvas->canvas->w)) = color;
  }
}


 void c_linedraw(Tcanvas *canvas, double x0, double y0, double x1, double y1, PixelType color) {
  double x;

  for (x=x0; x<=x1; x+=canvas->Xstep) {
    c_pixeldraw(canvas, (int)(x*canvas->Width/canvas->Xmax+0.5), (int)((double)canvas->Height/canvas->Ymax*(y1*(x1-x)+y1*(x-x0))/(x1-x0)+0.5),color);
  }
}


Tcanvas *c_open(int Width, int Height, double Xmax, double Ymax)
{
  int x,y;
  Tcanvas *canvas;
  canvas = malloc(sizeof(Tcanvas));

  canvas->Xoffset = 10;
  canvas->Yoffset = Height;

  canvas->Xext = 10;
  canvas->Yext = 10;



  canvas->Height = Height;
  canvas->Width  = Width; 
  canvas->Xmax   = Xmax;
  canvas->Ymax   = Ymax;

  canvas->Xstep  = Xmax/(double)Width/2;

  //  canvas->zpixel = (PixelType *)canvas->canvas->pixels +(Height-1)*canvas->canvas->w;

  SDL_Init(SDL_INIT_VIDEO); //SDL init
  canvas->canvas = SDL_SetVideoMode(canvas->Width+canvas->Xext, canvas->Height+canvas->Yext, BPP, SDL_SWSURFACE); 

  c_hlinedraw(canvas, 1, 0, (PixelType) SDL_MapRGB(canvas->canvas->format,  255, 255,  255));
  for (y=10;y<Ymax;y+=10) {
    c_hlinedraw(canvas, 3, y*Height/Ymax , (PixelType) SDL_MapRGB(canvas->canvas->format,  220, 220,  220));
  }
  c_vlinedraw(canvas, 0, 1, (PixelType) SDL_MapRGB(canvas->canvas->format,  255, 255,  255));
  for (x=10;x<Xmax;x+=10) {
    c_vlinedraw(canvas, x*Width/Xmax, 3, (PixelType) SDL_MapRGB(canvas->canvas->format,  220, 220,  220));
  }

  return canvas;
}

Tdataholder *datainit(int Width, int Height, double Xmax, double Ymax, double Lcurrent, double INcurrent, double OUTcurrent) {
  Tdataholder *data = malloc(sizeof(Tdataholder));


  data->canvas=c_open(Width, Height, Xmax, Ymax);
  data->Tcurrent=0;
  data->Lcurrent=Lcurrent;
  data->Lcolor= (PixelType) SDL_MapRGB(data->canvas->canvas->format,  255, 0,  0);
  data->INcurrent=INcurrent;
  data->INcolor=(PixelType) SDL_MapRGB(data->canvas->canvas->format,  0, 255,  0);
  data->OUTcurrent=OUTcurrent;
  data->OUTcolor=(PixelType) SDL_MapRGB(data->canvas->canvas->format,  0, 0,  255);


  return data;
}

void setdatacolors(Tdataholder *data, PixelType Lcolor, PixelType INcolor, PixelType OUTcolor) {
  data->Lcolor=Lcolor;
  data->INcolor=INcolor;
  data->OUTcolor=OUTcolor;
}


void datadraw(Tdataholder *data, double time, double level, double inangle, double outangle) {
  c_linedraw(data->canvas,data->Tcurrent,data->Lcurrent,time,level,data->Lcolor);
  c_linedraw(data->canvas,data->Tcurrent,data->INcurrent,time,inangle,data->INcolor);
  #ifndef CLIENTE
  c_linedraw(data->canvas,data->Tcurrent,data->OUTcurrent,time,outangle,data->OUTcolor);
#endif
  data->Tcurrent = time;
  data->Lcurrent = level;
  data->INcurrent = inangle;
  data->OUTcurrent = outangle;

  SDL_Flip(data->canvas->canvas);
}

void quitevent() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) { 
    if(event.type == SDL_QUIT) { 
      // close files, etc...

      SDL_Quit();
      exit(1); // this will terminate all threads !
    }
  }

}
/*
void tryquit() {
  SDL_Event event;

  if(SDL_PollEvent(&event)) { 
    if(event.type == SDL_QUIT) { 
      // close files, etc...

      SDL_Quit();
      exit(1); // this will terminate all threads !
    }
  }

}*/
void* grafico(void* ponteiroDados){
    Comum* dados = (Comum*)ponteiroDados;
    Tdataholder *data = datainit(640,240,500,110,0,0,0);
    double valorAnguloEntrada, valorAnguloSaida, valorNivelAtual;

    while (!dados->iniciado) { quitevent(); SDL_Delay(100); }

    Cronometro cron_tela;
    iniciar_cronometro(&cron_tela); 

    while (1) {
        obterTempoAtual(&cron_tela);
        double segundos = calcularDeltaMs(&cron_tela) / 1000.0;

        /* CORREÇÃO: Limpa a tela exatamente nos 100s e pinta o fundo de branco */
        if (segundos >= 100.0) {
            iniciar_cronometro(&cron_tela);
            segundos = 0.0;
            data->Tcurrent = 0.0; /* Impede que uma linha cruze a tela inteira */

            
            SDL_FillRect(data->canvas->canvas, NULL, SDL_MapRGB(data->canvas->canvas->format, 0, 0, 0));
            
            /* Redesenha a grade do professor */
            int x, y;
            for (y=10; y < data->canvas->Ymax; y+=10) 
                c_hlinedraw(data->canvas, 3, y * data->canvas->Height / data->canvas->Ymax, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 220, 220, 220));
            c_vlinedraw(data->canvas, 0, 1, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 255, 255, 255));
            for (x=10; x < data->canvas->Xmax; x+=10) 
                c_vlinedraw(data->canvas, x * data->canvas->Width / data->canvas->Xmax, 3, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 220, 220, 220));
        }

        pthread_mutex_lock(&dados->travaNivel);
        valorNivelAtual = dados->nivel; 
        pthread_mutex_unlock(&dados->travaNivel);

        pthread_mutex_lock(&dados->travaAngulo);
        valorAnguloEntrada = dados->anguloEntrada;
        valorAnguloSaida   = dados->anguloSaida;
        pthread_mutex_unlock(&dados->travaAngulo);

        datadraw(data, segundos, valorNivelAtual, valorAnguloEntrada, valorAnguloSaida);

        quitevent(); 
        SDL_Delay(100); 
    }
    return NULL;
}
