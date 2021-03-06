#!/bin/bash

COUNTER=2
COUNTER_EXECUCAO=0
TEMPOEXECUCAO=0
TEMPOEXECUCAOTOTAL=0

if [ -f ./tempos_threads.csv ]; then
    rm ./tempos_threads.csv
fi
echo 'nthreads;tempo' >> tempos_threads.csv

while [  $COUNTER -lt 17 ]; do
    echo "--INICIANDO GAUSS COM " $COUNTER " THREADS--"
    TEMPOEXECUCAOTOTAL=0
    while [ $COUNTER_EXECUCAO -lt 7 ]; do
        TEMPOEXECUCAO="$(./gauss_threads $COUNTER < ../input | tail -1)"
        TEMPOEXECUCAOTOTAL=$(awk "BEGIN {print $TEMPOEXECUCAOTOTAL+$TEMPOEXECUCAO; exit;}")
        echo "Gauss com " $COUNTER " threads: " $TEMPOEXECUCAO " segundos " $COUNTER_EXECUCAO
        let COUNTER_EXECUCAO=COUNTER_EXECUCAO+1
    done
    TEMPOEXECUCAOTOTAL=$(awk "BEGIN {print $TEMPOEXECUCAOTOTAL/7.0; exit;}")
    echo "Media: " $TEMPOEXECUCAOTOTAL
    echo ""
    echo $COUNTER';'$TEMPOEXECUCAOTOTAL >> tempos_threads.csv 
    let COUNTER=COUNTER+1 
    let COUNTER_EXECUCAO=0
done