#include "servidor.h"
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
    int ret1;

    ret1 = pthread_create( &thread_planta, NULL, planta, (void*) &memoria_compartilhada );
    if(ret1){
	    fprintf(stderr,"Error - pthread_create() return code: %d\n",ret1);
	    exit(EXIT_FAILURE);
	}

    roda_servidorIP(porta, &memoria_compartilhada);

    return 0;
}