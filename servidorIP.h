
#include <pthread.h>
#include "struct_geral.h"



#define  BUFFSIZE       255


char* processa_mensagem(char* msg, char* resposta);
int roda_servidorIP(int porta, Comum* ponteiro_memoria_compartilhada);