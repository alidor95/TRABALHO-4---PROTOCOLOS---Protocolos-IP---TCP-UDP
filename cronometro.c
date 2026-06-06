#include "cronometro.h"

void obterTempoAtual(Cronometro* cron){
    clock_gettime(CLOCK_MONOTONIC_RAW, &cron->marcadorAtual);
}

long tempoDecorridoMs(Cronometro* cron) {
    // 1. Proteção contra ponteiro nulo (Evita Segmentation Fault)
    if (&cron->marcadorAtual == NULL) {
        return 0; 
    }

    struct timespec tempoAtual;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tempoAtual);    

    // 2. Calcula a diferença bruta de segundos e nanossegundos
    long segundos = tempoAtual.tv_sec - cron->marcadorAtual.tv_sec;
    long nanossegundos = tempoAtual.tv_nsec - cron->marcadorAtual.tv_nsec;

    // 3. Ajusta se os nanossegundos atuais forem menores que o do início (virada de segundo)
    if (nanossegundos < 0) {
        segundos -= 1;
        nanossegundos += 1000000000; // Soma 1 bilhão de ns (1 segundo)
    }

    // 4. Converte tudo para Milissegundos (ms)
    // Segundos multiplicam por 1.000
    // Nanossegundos dividem por 1.000.000
    long diff_ms = (segundos * 1000) + (nanossegundos / 1000000);

    return diff_ms;
}

long calcularDeltaMs(Cronometro* cron) {
    // Proteção simples contra ponteiros nulos (evita Segmentation Fault)
    if (&cron->marcadorInicio == NULL || &cron->marcadorAtual == NULL) {
        return 0;
    }

    // 1. Diferença bruta de segundos e nanossegundos
    long delta_sec = cron->marcadorAtual.tv_sec - cron->marcadorInicio.tv_sec;
    long delta_nsec = cron->marcadorAtual.tv_nsec - cron->marcadorInicio.tv_nsec;

    // 2. Correção da virada de segundo:
    // Se os nanossegundos do ponto atual forem menores que os do início,
    // significa que virou o segundo, então "pedimos emprestado" 1 segundo (1 bilhão de ns).
    if (delta_nsec < 0) {
        delta_sec -= 1;
        delta_nsec += 1000000000; // + 1.000.000.000 ns
    }

    // 3. Conversão e junção para milissegundos (ms)
    // Segundos multiplicados por 1.000
    // Nanossegundos divididos por 1.000.000
    return (delta_sec * 1000) + (delta_nsec / 1000000);
}
