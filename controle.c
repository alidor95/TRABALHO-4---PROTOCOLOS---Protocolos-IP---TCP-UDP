#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h> 

#include "controle.h"
#include "struct_geral.h"
#include "cronometro.h"

#define PERIODO_MS            100       /* Período de amostragem */
#define REFERENCIA            80.0      /* Nível alvo (%) */

/* Constantes Oficiais do Gabarito em Ponto Fixo */
#define ESCALA                1000L
#define KP_INTEIRO            (40L * ESCALA)  
#define KI_INTEIRO            70L             
#define GANHO_AW_INTEIRO      700L      /* 0.7 * ESCALA */

#define SAIDA_MAX             100L      /* Saturação superior */
#define SAIDA_MIN             0L        /* Saturação inferior */
#define TAXA_MOV_VALVULA      0.015     /* Taxa de movimento da válvula */

static inline long limitar(long v, long minimo, long maximo) {
    return (v > maximo) ? maximo : (v < minimo) ? minimo : v;
}

void* controle(void* ponteiroDados) {
    int ultimo_delta;
    int ultima_seq = 1;

    Comum* dados = (Comum*)ponteiroDados;
    
    /* Configuração do Socket UDP */
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(dados->endereco);
    servAddr.sin_port = htons(atoi(dados->porta));

    /* Timeout de 10ms para recepção não-bloqueante */
    struct timeval tv;
    tv.tv_sec = 0; tv.tv_usec = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    /* Variáveis de Estado (Tradução do Gabarito) */
    int    nivelBruto       = 40; 
    long   integral         = 0L;
    int    anguloAlvo       = 50;       /* Último set-point enviado */
    int    comandoPendente  = 0;        /* Flag de controle de fluxo de rede */
    double anguloSimulado   = 50.0;     /* Estimativa local da válvula */
    double deltaPendente    = 0.0;      /* Delta acumulado não executado */
    long int    sequencia        = 1;
    
    char bufferRede[255];
    char bufferRecebido[255];
    int  start_confirmado = 0;
    
    Cronometro cronSetup;
    iniciar_cronometro(&cronSetup);

    /* 1. HANDSHAKE CONFIÁVEL (Garante Envio) */
    sprintf(bufferRede, "CommTest!");
    sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
    
    while (tempoDecorridoMs(&cronSetup) < 50); 
    obterTempoAtual(&cronSetup);

    while (!start_confirmado) {
        sprintf(bufferRede, "Start!");
        sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        printf("[CTRL] Aguardando StartOK do Servidor...\n");
        
        int lidos = recvfrom(sock, bufferRecebido, 255, 0, NULL, NULL);
        if (lidos > 0) {
            bufferRecebido[lidos] = '\0';
            if (strstr(bufferRecebido, "StartOK") != NULL) {
                start_confirmado = 1;
                printf("[CTRL] Conectado! Planta Iniciada.\n");
            }
        }
        usleep(10000); 
    }
    dados->iniciado = 1;

    /* Define a saída inicial do tanque */
    sprintf(bufferRede, "SetMax#100!"); 
    sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

    Cronometro marcadorTempo;
    iniciar_cronometro(&marcadorTempo);

    while (1) {
        /* 2. CÁLCULO DO ERRO E MALHA PI */
        double erroDouble  = REFERENCIA - (double)nivelBruto;
        int    erroInteiro = (int)round(erroDouble);
        
        long calculoP = KP_INTEIRO * (long)erroInteiro;
        integral += KI_INTEIRO * (long)erroInteiro;

        long saidaNaoSaturada = (calculoP + integral) / ESCALA;
        long saidaSaturada    = limitar(saidaNaoSaturada, SAIDA_MIN, SAIDA_MAX);

        /* ANTI-WINDUP POR BACK-CALCULATION (Igual ao Gabarito) */
        long diferencaSaturacao = saidaSaturada - saidaNaoSaturada;
        integral += (GANHO_AW_INTEIRO * diferencaSaturacao) / ESCALA;
        integral  = limitar(integral, SAIDA_MIN * ESCALA, SAIDA_MAX * ESCALA);

        int comando = (int)saidaSaturada;

        /* 5. PROCESSAMENTO DE ACKS (Equivalente ao Loop BCdequeue) */
        int lidos;
        while ((lidos = recvfrom(sock, bufferRecebido, 255, 0, NULL, NULL)) > 0) {
            bufferRecebido[lidos] = '\0';
            char cmdString[20]; int val1, val2;
            
            if (sscanf(bufferRecebido, "%10[^#!]#%d#%d!", cmdString, &val1, &val2) == 3) {
                if (strcmp(cmdString, "Level") == 0) {
                    nivelBruto = val2; 
                }
            } else if (sscanf(bufferRecebido, "%10[^#!]#%d!", cmdString, &val1) == 2) {
                if ((strcmp(cmdString, "Open") == 0 || strcmp(cmdString, "Close") == 0) && val1 == ultima_seq) {
                    comandoPendente = 0; /* ACK Recebido! Libera o fluxo para novos comandos */
                }
            }
        }

        /* 3. ENVIO CONTROLADO POR FLUXO (Flag comandoPendente) */
        if (comando != anguloAlvo && !comandoPendente) {
            int delta = comando - anguloAlvo;

            if (delta != 0){
                if (delta > 0){
                    sprintf(bufferRede, "OpenValve#%d#%d!", sequencia, abs(delta));
                } else if (delta < 0){
                    sprintf(bufferRede, "CloseValve#%d#%d!", sequencia, abs(delta));
                }
                ultimo_delta = delta;
                ultima_seq = sequencia;

                sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
                
                deltaPendente += (double)delta;
                anguloAlvo = comando;
                comandoPendente = 1; /* Trava novos envios até receber o ACK */
                sequencia++;
            }
        }else if (comandoPendente){
            // Reenvia o ultimo comando caso não tenha recebido a flag de comandoPendente
            if (ultimo_delta > 0){                
                sprintf(bufferRede, "OpenValve#%d#%d!", ultima_seq, abs(ultimo_delta));
            } else if (ultimo_delta < 0){
                sprintf(bufferRede, "CloseValve#%d#%d!", ultima_seq, abs(ultimo_delta));
            }
            sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        }

        /* 4. SOLICITAÇÃO PERIÓDICA DE NÍVEL */
        sprintf(bufferRede, "GetLevel#%d!", sequencia);
        sequencia++;
        sendto(sock, bufferRede, strlen(bufferRede), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

        

        /* 6. ESTIMATIVA LOCAL DA VÁLVULA PARA A INTERFACE */
        double passo = TAXA_MOV_VALVULA * PERIODO_MS;
        if (deltaPendente > 0.0) {
            double passoReal = (deltaPendente < passo) ? deltaPendente : passo;
            anguloSimulado += passoReal; deltaPendente -= passoReal;
        } else if (deltaPendente < 0.0) {
            double passoReal = (deltaPendente > -passo) ? deltaPendente : -passo;
            anguloSimulado += passoReal; deltaPendente -= passoReal;
        }
        anguloSimulado = fmax(0.0, fmin(100.0, anguloSimulado));

        /* 7. PUBLICAÇÃO NA MEMÓRIA COMPARTILHADA */
        pthread_mutex_lock(&dados->travaNivel);
        dados->nivel = (double)nivelBruto; 
        pthread_mutex_unlock(&dados->travaNivel);

        pthread_mutex_lock(&dados->travaAngulo);
        dados->anguloEntrada = anguloSimulado;
        pthread_mutex_unlock(&dados->travaAngulo);

        /* Temporização estável do ciclo */
        while (tempoDecorridoMs(&marcadorTempo) < PERIODO_MS);
        obterTempoAtual(&marcadorTempo);
    }
    return NULL;
}