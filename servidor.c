/*#include "servidor.h"
#include <stdio.h>

int main(int argc, char *argv[]) {    
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        exit(1);
    }

    int porta = atoi(argv[1]);

    BufferCircular lista_comandos;
    Comum memoria_compartilhada;
    
    lista_comandos.i_escrita = 0;
    lista_comandos.i_leitura = 0;
    memoria_compartilhada.filaEntrada = &lista_comandos;

    pthread_mutex_init(&memoria_compartilhada.travaAngulo, NULL);
    pthread_mutex_init(&memoria_compartilhada.travaNivel, NULL);
    pthread_mutex_init(&memoria_compartilhada.travaFilaEntrada, NULL);

    pthread_t thread_planta;
    pthread_t thread_grafico;

    int ret1, ret2;

    ret1 = pthread_create( &thread_planta, NULL, planta, (void*) &memoria_compartilhada );
    if(ret1){
	    fprintf(stderr,"Error - pthread_create() return code: %d\n",ret1);
	    exit(EXIT_FAILURE);
	}

    ret2 = pthread_create( &thread_grafico, NULL, grafico, (void*) &memoria_compartilhada );
    if(ret2){
	    fprintf(stderr,"Error - pthread_create() return code: %d\n",ret2);
	    exit(EXIT_FAILURE);
	}

    roda_servidorIP(porta, &memoria_compartilhada);

    return 0;
}*/

#include "servidor.h"
#include <stdio.h>
#include <stdlib.h>

/* Wrapper para rodar a rede em background sem alterar o servidorIP.c */
void* roda_rede(void* arg) {
    Comum* dados = (Comum*)arg;
    int porta_int = atoi(dados->porta);
    roda_servidorIP(porta_int, dados);
    return NULL;
}

int main(int argc, char *argv[]) {    
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        exit(1);
    }

    BufferCircular lista_comandos;
    Comum memoria_compartilhada;
    
    lista_comandos.i_escrita = 0;
    lista_comandos.i_leitura = 0;
    memoria_compartilhada.filaEntrada = &lista_comandos;

    /* Valores iniciais para não mostrar lixo no ecrã */
    memoria_compartilhada.iniciado = 0;
    memoria_compartilhada.nivel = 40.0;
    memoria_compartilhada.anguloEntrada = 50.0;
    memoria_compartilhada.anguloSaida = 50.0;
    memoria_compartilhada.porta = argv[1];

    pthread_mutex_init(&memoria_compartilhada.travaAngulo, NULL);
    pthread_mutex_init(&memoria_compartilhada.travaNivel, NULL);
    pthread_mutex_init(&memoria_compartilhada.travaFilaEntrada, NULL);

    pthread_t thread_planta;
    pthread_t thread_rede;

    /* Planta e Rede vão para background */
    pthread_create( &thread_planta, NULL, planta, (void*) &memoria_compartilhada );
    pthread_create( &thread_rede, NULL, roda_rede, (void*) &memoria_compartilhada );

    /* Gráfico assume a linha principal (Exigência do Linux) */
    grafico(&memoria_compartilhada);

    return 0;
}