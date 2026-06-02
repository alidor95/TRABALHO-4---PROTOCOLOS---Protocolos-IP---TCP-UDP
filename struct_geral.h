#ifndef GERAL_H
#define GERAL_H

#include <pthread.h>

#include "planta.h"

#define ABRIR_VALVULA    0
#define FECHAR_VALVULA   1
#define PEGAR_NIVEL      2
#define TESTE            3
#define DEFINIR_MAX      4
#define INICIAR          5

typedef struct {
    char* endereco; 
    char* porta; 
    int   iniciado;   
  
       
    void* filaEntrada;           
    pthread_mutex_t travaFilaEntrada; 

    void* filaSaida;

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
