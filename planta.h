#ifndef PLANTA_H
#define PLANTA_H


#define ANGULO_ENTRADA_INICIO  50.0   // angulo inicial da valvula de entrada
#define NIVEL_INICIO           0.4    //nivel inicial é a porcetagem de 40%
#define MAXIMO_INICIO          50.0    // saida inicial do tanque
#define TAXA_VALVULA           0.015  // taxa da valvula de entrada por millisegundo


typedef double Nivel;   
typedef double Angulo;  

void* planta(void* ponteiroDados); // declaração da planta , é a thread da planta 


#endif 
