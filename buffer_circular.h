#include "struct_geral.h"

#define BUFFSIZE                 50

#define SEM_MENSAGEM_REPETIDA     0
#define MENSAGEM_REPETIDA         1

typedef struct 
{
    Mensagem lista_mensagens[BUFFSIZE];
    int i_leitura;
    int i_escrita;    
}buffer_circular;


void adiciona_mensagem(buffer_circular* buff, Mensagem msg);
Mensagem adiciona_mensagem(buffer_circular* buff);
int novas_mensagens(buffer_circular* buff);
int verifica_mensagem_repetida(buffer_circular* buff, int comando, int seq);