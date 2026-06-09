/* seguindo a ideia de aportuguesa as coisas
para criar a rede e o grafico teria que olhar as funções aqui e traduzir
*/

#include "planta.h"

//#include "grafico.h" -> falta criar

#define INTERVALO_CICLO_MS  10        // periodo da simulação 10 milissegundos definido no enunciado       
#define VALOR_PI            3.14159265358979  // numero pi padrão

/*

função "equaçãoanguloentrada" que implementa o movimento da valvula de entrada , 
o paramentro tempo decorrido é o tempo real 10 ms , o ponteiro delta é o ponteiro do angulo que ainda falta 
ponteiro angulo atual é o angulo atual.
*/
void equacaoAnguloEntrada(long tempoDecorridoMs,
                          Angulo* ponteiroDelta,
                          Angulo* ponteiroAnguloAtual)
{
   
    const Angulo passoMaximo = TAXA_VALVULA * tempoDecorridoMs; // calcula o passo maximo 0.15 o angulo.

    if (*ponteiroDelta > 0)  { // delta positivo é pq tem um pedido de abertura na valvula 
        
        if (*ponteiroDelta < passoMaximo) {
            /* delta restante menor que o passo — move tudo e zera */
            *ponteiroAnguloAtual += *ponteiroDelta; // ponteiro angulo atual acumula ponteiro delta 
            *ponteiroDelta = 0;
            // aqui o ciclo é concluido qa valvula faz o  ciclo
        } else {
            // não completa o ciclo 
            *ponteiroAnguloAtual += passoMaximo;
            *ponteiroDelta       -= passoMaximo;
        }
    } else if (*ponteiroDelta < 0) { // delta negativo significa pedido para fechamento 
            // mesmo raciocinio do ponteiro delta positivo
        if (*ponteiroDelta > -passoMaximo) {
            *ponteiroAnguloAtual += *ponteiroDelta;
            *ponteiroDelta = 0;
        } else {
            *ponteiroAnguloAtual -= passoMaximo;
            *ponteiroDelta       += passoMaximo;
        }
    }
   
}

/*
Calcula o ângulo da válvula de saída em função do tempo total de simulação
recebe t desde o start é predinida no enunciado . 
*/
Angulo equacaoAnguloSaida(long t)
{
    if (t <= 0) // T=0 ou negativo: válvula de saída em 50%. Valor inicial.
        return 50.0;
    if (t < 20000)
        return 50.0 + t / 400.0;           // sobe 50→100% em 20s */
    if (t < 30000)
        return 100.0;                        // fica em 100% por 10s */
    if (t < 50000)
        return 100.0 - (t - 30000) / 250.0; // desce 100→20% em 20s 
    if (t < 70000)
        return 20 + (t - 50000) / 1000.0; // sobe 20→40% em 20s 
    if (t < 100000)
        //oscila ao redor de 40% com amplitude de 20% e período de 30 segundos. Varia entre 20% e 60%.
        return 40.0 + 20.0 * sin((t - 70000) * 2.0 * VALOR_PI / 30000.0);
    if (t < 120000)
        //sobe linearmente de 40% → 100%. Em t=120000: 40 + 3×20 = 100%
        return 40.0 + 3.0 * (t - 100000) / 1000.0;
    return 100.0;
}

// principal da simulação
void* planta(void* ponteiroDados)
{
    //
    Comum* dadosServidor = (Comum*)ponteiroDados;

    BufferCircular* filaEntradaPlanta = dadosServidor->filaEntrada;
    pthread_mutex_t* travaFila         = &(dadosServidor->travaFilaEntrada);
    // ponteiro para a flag de inicio quando o start é recebido a flag aqui vai para 1.
    int*             flagIniciado       = &(dadosServidor->iniciado);

    pthread_mutex_t* travaNivel         = &(dadosServidor->travaNivel);
    Nivel*           nivelAtual         = &(dadosServidor->nivel);

    pthread_mutex_t* travaAngulo        = &(dadosServidor->travaAngulo);
    Angulo*          anguloEntradaAtual = &(dadosServidor->anguloEntrada);
    Angulo*          anguloSaidaAtual   = &(dadosServidor->anguloSaida);

    Cronometro cron;
    cron.marcadorAtual.tv_nsec = 0;
    cron.marcadorAtual.tv_sec = 0;
    cron.marcadorInicio.tv_nsec = 0;
    cron.marcadorInicio.tv_sec = 0;

    // struct timespec marcadorInicio, marcadorAtual;
    long            tempoDecorridoCicloMs;
    Nivel           proximoNivel        = 0;
    Angulo          anguloEntradaCalc   = 0;
    Angulo          anguloSaidaCalc     = 0;
    Angulo          deltaPendente       = 0;
    double          configuracaoMaximo  = MAXIMO_INICIO;
    double          fluxoEntrada, fluxoSaida;

    // obterTempoAtual(&marcadorAtual);

    // clock_gettime(CLOCK_MONOTONIC_RAW, &marcadorAtual);
    // obterTempoAtual(&cron);
    iniciar_cronometro(&cron);

    long deltaMS;

    for (;;) {
        
        while ((tempoDecorridoCicloMs = tempoDecorridoMs(&cron)) < INTERVALO_CICLO_MS);
        obterTempoAtual(&cron);

        // processa todas as mensagens da fila 
        while ((novas_mensagens(filaEntradaPlanta)) != 0) {
            printf("nova msg - %ld\n", tempoDecorridoCicloMs);
            Mensagem mensagemRecebida = le_mensagem(filaEntradaPlanta);
            if (mensagemRecebida.comando == INICIAR) {
                // reinicia toda a simulação 
                // obterTempoAtual(&marcadorInicio);
                iniciar_cronometro(&cron);
                // clock_gettime(CLOCK_MONOTONIC_RAW, &marcadorAtual);
                // obterTempoAtual(&cron);
                cron.marcadorAtual       = cron.marcadorInicio;
                anguloEntradaCalc   = ANGULO_ENTRADA_INICIO;
                proximoNivel        = NIVEL_INICIO;
                deltaPendente       = 0.0;
                configuracaoMaximo  = MAXIMO_INICIO;
                *flagIniciado       = 1;
                puts("Servidor - PLANTA iniciada.");
                continue;
            }

            if (*flagIniciado) {
                switch (mensagemRecebida.comando) {
                case ABRIR_VALVULA:
                    //acumula delta positivo — abre válvula 
                    printf("abrir - %d\n", mensagemRecebida.valor);
                    deltaPendente += mensagemRecebida.valor;
                    break;
                case FECHAR_VALVULA:
                    // acumula delta negativo — fecha válvula 
                    printf("fechar - %d\n", mensagemRecebida.valor);
                    deltaPendente -= mensagemRecebida.valor;
                    break;
                case DEFINIR_MAX:
                    configuracaoMaximo = mensagemRecebida.valor;
                    break;
                default:
                    break;
                }
            }
        }

        
        if (!(*flagIniciado)){
            continue;
        }

       
        equacaoAnguloEntrada(tempoDecorridoCicloMs,
                             &deltaPendente,
                             &anguloEntradaCalc);

    
        if (anguloEntradaCalc > 100.0) anguloEntradaCalc = 100.0;
        if (anguloEntradaCalc <   0.0) anguloEntradaCalc =   0.0;

        
        deltaMS = calcularDeltaMs(&cron);
        // printf("deltaMS - %ld\n", deltaMS);
        anguloSaidaCalc = equacaoAnguloSaida(deltaMS);

        // equações físicas do tanque 
        fluxoEntrada = sin(VALOR_PI / 2.0 * anguloEntradaCalc / 100.0);
        fluxoSaida   = (configuracaoMaximo / 100.0)
                     * (proximoNivel / 1.25 + 0.2)
                     * sin(VALOR_PI / 2.0 * anguloSaidaCalc / 100.0);

        // atualiza nível: level(T+dT) = level(T) + 0.00002*dT*(influx-outflux) 
        proximoNivel += 0.00002 * tempoDecorridoCicloMs * (fluxoEntrada - fluxoSaida);

        // limita nível entre 0 e 1 
        if (proximoNivel > 1.0) proximoNivel = 1.0;
        if (proximoNivel < 0.0) proximoNivel = 0.0;

        //publica nível com mutex 
        pthread_mutex_lock(travaNivel);
        *nivelAtual = proximoNivel*100;
        pthread_mutex_unlock(travaNivel);

        // publica ângulos com mutex 
        pthread_mutex_lock(travaAngulo);
        *anguloEntradaAtual = anguloEntradaCalc;
        *anguloSaidaAtual   = anguloSaidaCalc;
        pthread_mutex_unlock(travaAngulo);
    }
    printf("retornando \n");
    return NULL;
}
