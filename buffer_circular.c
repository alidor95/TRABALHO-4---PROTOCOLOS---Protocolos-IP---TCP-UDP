#include "buffer_circular.h"

/// @brief Adiciona uma mensagem nova no buffer circular
/// @param buff Ponteiro para o buffer circular
/// @param msg Mensagem a ser adicionada
void adiciona_mensagem(buffer_circular* buff, Mensagem msg){
    buff->lista_mensagens[buff->i_escrita] = msg;

    buff->i_escrita++;
    if (buff->i_escrita == BUFFSIZE) {
        buff->i_escrita = 0;
    }

    if (buff->i_escrita != buff->i_leitura){
        return;
    }

    buff->i_leitura++;
    if (buff->i_leitura == BUFFSIZE) {
        buff->i_leitura = 0;
    }
    
    return;
}

/// @brief Pega uma mensagem da fila do buffer circular
/// @param buff Ponteiro para o buffer circular
/// @return  Retorna a mensagem selecionada
Mensagem le_mensagem(buffer_circular* buff){
    // Se não tiver mensagem nova, retorna nulo
    if (buff->i_escrita == buff->i_leitura){
        return;
    }

    Mensagem msg = buff->lista_mensagens[buff->i_leitura];
    
    // Atualiza indice de leitura
    buff->i_leitura++;
    if (buff->i_leitura == BUFFSIZE) {
        buff->i_leitura = 0;
    }

    return msg;
}

/// @brief Diz quantas mensagens novas existem no buffer circular
/// @param buff ponteiro para o buffer circular
/// @return retorna quantas mensagens novas existem
int novas_mensagens(buffer_circular* buff){
    int num_novas_mensagens = buff->i_escrita - buff->i_leitura;
    if (num_novas_mensagens < 0){
        num_novas_mensagens = num_novas_mensagens + BUFFSIZE;
    }
    return num_novas_mensagens;
}

/// @brief Verifica se a mensagem ja foi recebida (verifica todas que estão no buffer)
/// @param buff Ponteiro para o buffer circular
/// @param comando Comando a ser verificado
/// @param seq Sequencia a ser verificada
/// @return 
int verifica_mensagem_repetida(buffer_circular* buff, int comando, int seq){
    int i = 0;
    int num_msgs = novas_mensagens(buff);
    int index = 0;

    if (num_msgs == BUFFSIZE - 1){
        for (i; i < BUFFSIZE - 1; i++){
            if (buff->lista_mensagens[i].comando == comando && buff->lista_mensagens[i].seq == seq){
                return MENSAGEM_REPETIDA;
            }
        }
    }else{
        index = buff->i_leitura; 
        for (i; i < num_msgs; i++){
            if (index >= BUFFSIZE){
                index = index - BUFFSIZE;
            }

            if (buff->lista_mensagens[index].comando == comando && buff->lista_mensagens[index].seq == seq){
                return MENSAGEM_REPETIDA;
            }

            index++;
        }
    }

    return SEM_MENSAGEM_REPETIDA;
}
