#ifndef GERAL_H
#define GERAL_H

#include <pthread.h>

//#include "rede.h"
#include "planta.h"


typedef struct {
    char* endereco; 
    char* porta; 
    int   iniciado;   
    BufferCircular* filaSaida;   

    pthread_mutex_t travaNivel;   
    Nivel           nivel;        

    pthread_mutex_t travaAngulo; 
    Angulo          anguloEntrada; 
    Angulo          anguloSaida;   
} Comum;

typedef struct
{
    int comando;
    int seq;
    int valor;
} Mensagem;

#endif
