#ifndef CRONOMETRO_H
#define CRONOMETRO_H

#include <time.h>

typedef struct
{
    struct timespec marcadorInicio; 
    struct timespec marcadorAtual;
}Cronometro;

void obterTempoAtual(Cronometro* cron);
long tempoDecorridoMs(Cronometro* cron);
long calcularDeltaMs(Cronometro* cron);
void iniciar_cronometro(Cronometro* cron);

#endif