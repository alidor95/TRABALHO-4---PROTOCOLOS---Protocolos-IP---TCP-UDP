 # Trabalho 4 grupo D

 Bibliotecas .h : rede.h , planta.h , grafico.h , struct_geral.h , controle.h
 Biblioteca .c : cliente.c , servidor.c , controle.c, grafico.c, planta.c, rede.c
 compilador.sh

oque deve fazer cada biblioteca e suas responsabilidades

planta.c / planta.h     → A FÍSICA do tanque

rede.c   / rede.h       → O PROTOCOLO de comunicação UDP

grafico.c / grafico.h   → O DISPLAY em tempo real (SDL)

struck_geral.h          → A MEMÓRIA COMPARTILHADA entre threads

controle.c / controle.h → O CÉREBRO do controlador PI

servidor.c              → Orquestrador do servidor (cria threads)

cliente.c               → Orquestrador do cliente (cria threads)

compilador.sh           → Script de compilação
