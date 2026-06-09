#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "struct_geral.h"
#include "controle.h"
#include "grafico.h"

int main(int argc, char* argv[]) {
    Comum dadosCliente = {0};

    if (argc != 3) {
        fputs("Uso: ./cliente <ip_servidor> <porta>\n", stderr);
        exit(EXIT_FAILURE);
    }

    dadosCliente.endereco = argv[1];
    dadosCliente.porta    = argv[2];
    dadosCliente.iniciado = 0;
    
    pthread_mutex_init(&(dadosCliente.travaNivel),  NULL);
    pthread_mutex_init(&(dadosCliente.travaAngulo), NULL);

    pthread_t threadControle;

    pthread_create(&threadControle, NULL, controle, &dadosCliente);
    grafico(&dadosCliente); 

    return 0;
}