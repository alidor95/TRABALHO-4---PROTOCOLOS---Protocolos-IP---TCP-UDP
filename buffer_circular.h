#ifndef BUFFER_CIRC_H
#define BUFFER_CIRC_H

#define CIRCBUFFSIZE                 50

#define SEM_MENSAGEM_REPETIDA     0
#define MENSAGEM_REPETIDA         1


typedef struct
{
    int comando;
    int seq;
    int valor;
} Mensagem;

typedef struct 
{
    Mensagem lista_mensagens[CIRCBUFFSIZE];
    int i_leitura;
    int i_escrita;    
}BufferCircular;



void adiciona_mensagem(BufferCircular* buff, Mensagem msg);
Mensagem le_mensagem(BufferCircular* buff);
int novas_mensagens(BufferCircular* buff);
int verifica_mensagem_repetida(BufferCircular* buff, int comando, int seq);

#endif