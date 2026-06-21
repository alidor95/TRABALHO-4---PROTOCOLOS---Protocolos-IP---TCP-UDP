#!/bin/bash

# Define a porta: usa o primeiro parâmetro ($1) se existir, 
# caso contrário, usa 11000 como padrão.
PORTA=${1:-11000}

sudo ./simulador_rede_tc.sh stop
sleep 2

# Rede
sudo ./simulador_rede_tc.sh start
sleep 2

DIR_ATUAL="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Abrir janelas utilizando a variável $PORTA
gnome-terminal --working-directory="$DIR_ATUAL" -- ./servidor "$PORTA" &
set SERVER_PID $last_pid
gnome-terminal --working-directory="$DIR_ATUAL" -- ./cliente 127.0.0.1 "$PORTA" &
set CLIENT_PID $last_pid

# Aguardar fechamento das janelas para só então parar a rede
wait $SERVER_PID #$CLIENT_PID
sudo ./simulador_rede_tc.sh stop