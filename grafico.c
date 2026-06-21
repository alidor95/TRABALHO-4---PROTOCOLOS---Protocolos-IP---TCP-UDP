#include "grafico.h"

#define SCREEN_W 640 //tamanho da janela que sera criada
#define SCREEN_H 640

// --- MATRIZ DE LETRAS (Bitmap Font 5x5) ---
// Cada linha representa uma letra. 1 = pixel pintado, 0 = vazio.
const int FONT_BITMAP[9][5][5] = {
  { // 'N' (Índice 0)
    {1,0,0,0,1}, {1,1,0,0,1}, {1,0,1,0,1}, {1,0,0,1,1}, {1,0,0,0,1}
  },
  { // 'I' (Índice 1)
    {0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,1,1,1,0}
  },
  { // 'V' (Índice 2)
    {1,0,0,0,1}, {1,0,0,0,1}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0}
  },
  { // 'E' (Índice 3)
    {1,1,1,1,1}, {1,0,0,0,0}, {1,1,1,1,0}, {1,0,0,0,0}, {1,1,1,1,1}
  },
  { // 'L' (Índice 4)
    {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}
  },
  { // 'O' (Índice 5)
    {0,1,1,1,0}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}
  },
  { // 'U' (Índice 6)
    {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {1,0,0,0,1}, {0,1,1,1,0}
  },
  { // 'T' (Índice 7)
    {1,1,1,1,1}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}
  },
  { // ' ' (Espaço - Índice 8)
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}
  }
};

// --- FUNÇÃO AUXILIAR: Desenha um caractere na tela usando a matriz ---
void c_drawchar(Tcanvas *canvas, int x0, int y0, int char_index, PixelType color) {
  int x, y;
  for (y = 0; y < 5; y++) {
    for (x = 0; x < 5; x++) {
      if (FONT_BITMAP[char_index][y][x] == 1) {
        // Multiplica por 2 para dar um "zoom" de tamanho (escala 2x2 por pixel da matriz)
        c_pixeldraw(canvas, x0 + (x * 2), y0 - (y * 2), color);
        c_pixeldraw(canvas, x0 + (x * 2) + 1, y0 - (y * 2), color);
        c_pixeldraw(canvas, x0 + (x * 2), y0 - (y * 2) - 1, color);
        c_pixeldraw(canvas, x0 + (x * 2) + 1, y0 - (y * 2) - 1, color);
      }
    }
  }
}

// --- FUNÇÃO AUXILIAR: Desenha uma string predefinida por índices ---
void c_drawstring(Tcanvas *canvas, int x, int y, int* indices, int length, PixelType color) {
  int i;
  for (i = 0; i < length; i++) {
    c_drawchar(canvas, x + (i * 14), y, indices[i], color); // 14 pixels de espaçamento entre letras
  }
}

// --- FUNÇÃO ALTERADA: Desenha os blocos e os textos de legenda ---
void desenhar_legendas(Tdataholder *data) {
  // Mapeamento das palavras pelos índices da FONT_BITMAP
  int txt_nivel[] = {0, 1, 2, 3, 4}; // N I V E L
  int txt_in[]    = {1, 0};          // I N
  int txt_out[]   = {5, 6, 7};       // O U T
  
  PixelType cor_texto = (PixelType) SDL_MapRGB(data->canvas->canvas->format, 255, 255, 255); // Branco

  // Legenda 1: "NIVEL" (Vermelho)
  c_rectdraw(data->canvas, 20, data->canvas->Height - 15, 12, 12, data->Lcolor);
  c_drawstring(data->canvas, 38, data->canvas->Height - 13, txt_nivel, 5, cor_texto);
  
  // Legenda 2: "IN" (Verde)
  c_rectdraw(data->canvas, 140, data->canvas->Height - 15, 12, 12, data->INcolor);
  c_drawstring(data->canvas, 158, data->canvas->Height - 13, txt_in, 2, cor_texto);

  #ifndef CLIENTE
  // Legenda 3: "OUT" (Azul)
  c_rectdraw(data->canvas, 220, data->canvas->Height - 15, 12, 12, data->OUTcolor);
  c_drawstring(data->canvas, 238, data->canvas->Height - 13, txt_out, 3, cor_texto);
  #endif
}

void c_rectdraw(Tcanvas *canvas, int x0, int y0, int w, int h, PixelType color) {
  int x, y;
  for (y = y0; y < y0 + h; y++) {
    for (x = x0; x < x0 + w; x++) {
      if (x >= 0 && x < canvas->canvas->w && (-y + canvas->Yoffset) >= 0) {
        c_pixeldraw(canvas, x, y, color);
      }
    }
  }
}

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

  SDL_Init(SDL_INIT_VIDEO); 
  canvas->canvas = SDL_SetVideoMode(canvas->Width+canvas->Xext, canvas->Height+canvas->Yext, BPP, SDL_SWSURFACE); 

  #ifdef CLIENTE
  SDL_WM_SetCaption("Grafico - CLIENTE", NULL);
  #else
  SDL_WM_SetCaption("Grafico - SERVIDOR", NULL);
  #endif
  
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

  if (data == NULL) {
    return NULL; 
  }

  data->canvas = c_open(Width, Height, Xmax, Ymax);
  data->Tcurrent = 0;

  data->Lindex = 1; 
  data->Lcurrent = Lcurrent;
  data->Lcolor = (PixelType) SDL_MapRGB(data->canvas->canvas->format, 255, 0, 0); 

  data->INindex = 2; 
  data->INcurrent = INcurrent;
  data->INcolor = (PixelType) SDL_MapRGB(data->canvas->canvas->format, 0, 255, 0); 

  data->OUTindex = 3; 
  data->OUTcurrent = OUTcurrent;
  data->OUTcolor = (PixelType) SDL_MapRGB(data->canvas->canvas->format, 0, 0, 255); 

  desenhar_legendas(data);

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

  // Garante que o texto continue na tela de forma estável
  desenhar_legendas(data);

  SDL_Flip(data->canvas->canvas);
}

void quitevent() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) { 
    if(event.type == SDL_QUIT) { 
      SDL_Quit();
      exit(1); 
    }
  }
}

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

        if (segundos >= 8*60.0) {
            iniciar_cronometro(&cron_tela);
            segundos = 0.0;
            data->Tcurrent = 0.0; 

            SDL_FillRect(data->canvas->canvas, NULL, SDL_MapRGB(data->canvas->canvas->format, 0, 0, 0));
            
            int x, y;
            for (y=10; y < data->canvas->Ymax; y+=10) 
                c_hlinedraw(data->canvas, 3, y * data->canvas->Height / data->canvas->Ymax, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 220, 220, 220));
            c_vlinedraw(data->canvas, 0, 1, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 255, 255, 255));
            for (x=10; x < data->canvas->Xmax; x+=10) 
                c_vlinedraw(data->canvas, x * data->canvas->Width / data->canvas->Xmax, 3, (PixelType) SDL_MapRGB(data->canvas->canvas->format, 220, 220, 220));
            
            desenhar_legendas(data);
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