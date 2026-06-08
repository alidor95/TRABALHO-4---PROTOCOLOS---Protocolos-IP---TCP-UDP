#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

// #include <planta.h>
// #include "struct_geral.h"
#include "servidorIP.h"

#define BUFFSIZE 255


void Die(char *mess) { perror(mess); exit(1); }

Comum* memoria_compartilhada;
BufferCircular mensagens_recebidas;

/// @brief Processa a mensagem recebida por UDP e armazena ela no buffer
/// @param msg Ponteiro para a mensagem recebida
/// @param resposta Ponteiro para a string de resposta para o cliente UDP
/// @return Resposta
char* processa_mensagem(char* buff, char* resposta){
    Mensagem mensagem_recebida;
    char comando[11];
    int val1;
    int val2;

    int itens_encontrados = sscanf(buff, "%10[^#!]#%d#%d!", comando, &val1, &val2);

    if (strcmp(comando, "OpenValve") == 0 && itens_encontrados == 3){
        mensagem_recebida.comando = ABRIR_VALVULA;
        mensagem_recebida.seq = val1;
        mensagem_recebida.valor = val2;
        if (verifica_mensagem_repetida(&mensagens_recebidas, mensagem_recebida.comando, mensagem_recebida.seq) == SEM_MENSAGEM_REPETIDA){
            adiciona_mensagem(memoria_compartilhada->filaEntrada, mensagem_recebida);
            adiciona_mensagem(&mensagens_recebidas, mensagem_recebida);
        }
        sprintf(resposta, "Open#%d!", val1);

    }else if ( strcmp(comando, "CloseValve") == 0 && itens_encontrados == 3){
        mensagem_recebida.comando = FECHAR_VALVULA;
        mensagem_recebida.seq = val1;
        mensagem_recebida.valor = val2;
        if (verifica_mensagem_repetida(&mensagens_recebidas, mensagem_recebida.comando, mensagem_recebida.seq) != MENSAGEM_REPETIDA){
            adiciona_mensagem(memoria_compartilhada->filaEntrada, mensagem_recebida);
            adiciona_mensagem(&mensagens_recebidas, mensagem_recebida);
        }
        sprintf(resposta, "Close#%d!", val1);

    }else if ( strcmp(comando, "GetLevel") == 0 && itens_encontrados == 2){
        pthread_mutex_lock(&memoria_compartilhada->travaNivel);
        int nivel = memoria_compartilhada->nivel; 
        pthread_mutex_unlock(&memoria_compartilhada->travaNivel);
        mensagem_recebida.comando = PEGAR_NIVEL;
        mensagem_recebida.seq = val1;
        if (verifica_mensagem_repetida(&mensagens_recebidas, mensagem_recebida.comando, mensagem_recebida.seq) != MENSAGEM_REPETIDA){
            adiciona_mensagem(memoria_compartilhada->filaEntrada, mensagem_recebida);
            adiciona_mensagem(&mensagens_recebidas, mensagem_recebida);
        }
        sprintf(resposta, "Level#%d#%d!", val1, nivel);

    }else if ( strcmp(comando, "CommTest") == 0 && itens_encontrados == 1){
        mensagem_recebida.comando = TESTE;
        sprintf(resposta, "CommOK!");

    }else if ( strcmp(comando, "SetMax") == 0 && itens_encontrados == 2){
        mensagem_recebida.comando = DEFINIR_MAX;
        mensagem_recebida.valor = val1;
        adiciona_mensagem(memoria_compartilhada->filaEntrada, mensagem_recebida);
        sprintf(resposta, "Max#%d!", val1);
            
    }else if ( strcmp(comando, "Start") == 0 && itens_encontrados == 1){
        mensagem_recebida.comando = INICIAR;
        adiciona_mensagem(memoria_compartilhada->filaEntrada, mensagem_recebida);
        sprintf(resposta, "StartOK!");

    }else {
        sprintf(resposta, "Err!");
    }

    return resposta;
}

int roda_servidorIP(int porta, Comum* ponteiro_memoria_compartilhada) {
    int sock;
    struct sockaddr_in server;
    struct sockaddr_in client;
    char buffer[BUFFSIZE];
    char resposta[BUFFSIZE];
    unsigned int echolen, clientlen, serverlen;
    int received = 0;

    memoria_compartilhada = ponteiro_memoria_compartilhada;

     /* Create the UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        Die("Failed to create socket");
    }
    /* Construct the server sockaddr_in structure */
    memset(&server, 0, sizeof(server));       /* Clear struct */
    server.sin_family = AF_INET;                  /* Internet/IP */
    server.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any IP address */
    server.sin_port = htons(porta);       /* server port */
    
    /* Bind the socket */
    serverlen = sizeof(server);
    if (bind(sock, (struct sockaddr *) &server, serverlen) < 0) {
        Die("Failed to bind server socket");
    }

    
    /* Run until cancelled */
    while (1) {
        /* Receive a message from the client */
        clientlen = sizeof(client);
        if ((received = recvfrom(sock, buffer, BUFFSIZE, 0,
                                (struct sockaddr *) &client,
                                &clientlen)) < 0) {
        Die("Failed to receive message");
        }
        fprintf(stderr,
                "Client connected: %s\n", inet_ntoa(client.sin_addr));
        /* Send the message back to client */

        processa_mensagem(buffer, resposta);
        
        if (sendto(sock, resposta, sizeof(resposta), 0,
                (struct sockaddr *) &client,
                sizeof(client)) == 0) {
        Die("Mismatch in number of echo'd bytes");
        }

    }
}