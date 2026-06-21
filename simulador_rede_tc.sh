#!/bin/bash

# ==============================================================================
# Script de Simulação de Redes com Traffic Control (tc)
# Configuração baseada em perfis dinâmicos de netem mapeados por portas.
# ==============================================================================

# Configurações de Interface e Portas
INTERFACE="lo"
PORTA_1=8000
PORTA_2=9000
PORTA_3=10000
PORTA_4=11000

# Cores para formatação de output
VERDE='\033[0;32m'
VERMELHO='\033[0;31m'
AZUL='\033[0;34m'
AMARELO='\033[0;33m'
NC='\033[0m' # Sem cor

# Validação de privilégios de superusuário (root)
if [ "$EUID" -ne 0 ]; then
    echo -e "${VERMELHO}[ERRO] Este script precisa ser executado como root. Use 'sudo'.${NC}"
    exit 1
fi

funcao_limpar() {
    echo -e "${AZUL}[*] Removendo configurações existentes de tc na interface ${INTERFACE}...${NC}"
    tc qdisc del dev "$INTERFACE" root 2>/dev/null
    echo -e "${VERDE}[OK] Interface limpa e redefinida para o comportamento normal.${NC}"
}

funcao_status() {
    echo -e "\n${AMARELO}=== QDISCS ATIVOS ===${NC}"
    tc qdisc show dev "$INTERFACE"
    echo -e "\n${AMARELO}=== FILTROS ATIVOS ===${NC}"
    tc filter show dev "$INTERFACE" parent 1:
}

funcao_iniciar() {
    # 1. Garante que começamos com um ambiente limpo
    funcao_limpar
    
    echo -e "\n${AZUL}[*] Criando qdisc raiz 'prio' com suporte a 4 classes principais...${NC}"
    # Criamos com 5 bandas para que o tráfego padrão caia na banda 5 (sem alterações), protegendo o resto da rede
    tc qdisc add dev "$INTERFACE" root handle 1: prio bands 5 priomap 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
    
    echo -e "${AZUL}[*] Aplicando perfis de degradação de rede (netem)...${NC}"
    
    # Classe 1:1 - Mapeada para a Porta 8000 (Leve)
    echo -e "    -> Configurando Classe 1:1 (Porta ${PORTA_1})"
    tc qdisc add dev "$INTERFACE" parent 1:1 handle 10: netem delay 100ms 75ms reorder 30% 60% corrupt 3% loss 5% 30%
    
    # Classe 1:2 - Mapeada para a Porta 9000 (Moderada)
    echo -e "    -> Configurando Classe 1:2 (Porta ${PORTA_2})"
    tc qdisc add dev "$INTERFACE" parent 1:2 handle 20: netem delay 100ms 75ms reorder 30% 60% corrupt 10% loss 15% 30%
    
    # Classe 1:3 - Mapeada para a Porta 10000 (Grave)
    echo -e "    -> Configurando Classe 1:3 (Porta ${PORTA_3})"
    tc qdisc add dev "$INTERFACE" parent 1:3 handle 30: netem delay 125ms 75ms reorder 40% 80% corrupt 20% loss 20% 30%
    
    # Classe 1:4 - Mapeada para a Porta 11000 (Crítica)
    echo -e "    -> Configurando Classe 1:4 (Porta ${PORTA_4})"
    tc qdisc add dev "$INTERFACE" parent 1:4 handle 40: netem delay 125ms 75ms reorder 80% 80% corrupt 30% loss 30% 50%

    echo -e "\n${AZUL}[*] Aplicando filtros de roteamento de pacotes por porta (u32)...${NC}"
    
    # Filtros Porta 8000 -> Classe 1:1
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip dport "$PORTA_1" 0xffff flowid 1:1
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip sport "$PORTA_1" 0xffff flowid 1:1
    
    # Filtros Porta 9000 -> Classe 1:2
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip dport "$PORTA_2" 0xffff flowid 1:2
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip sport "$PORTA_2" 0xffff flowid 1:2
    
    # Filtros Porta 10000 -> Classe 1:3
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip dport "$PORTA_3" 0xffff flowid 1:3
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip sport "$PORTA_3" 0xffff flowid 1:3
    
    # Filtros Porta 11000 -> Classe 1:4
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip dport "$PORTA_4" 0xffff flowid 1:4
    tc filter add dev "$INTERFACE" protocol ip parent 1:0 prio 1 u32 match ip sport "$PORTA_4" 0xffff flowid 1:4

    echo -e "\n${VERDE}[OK] Simulação de rede ativada com sucesso!${NC}"
}

# Controle de Argumentos
case "$1" in
    start)
        funcao_iniciar
        ;;
    stop)
        funcao_limpar
        ;;
    status)
        funcao_status
        ;;
    *)
        echo -e "${AMARELO}Uso correto: $0 {start|stop|status}${NC}"
        exit 1
        ;;
esac
