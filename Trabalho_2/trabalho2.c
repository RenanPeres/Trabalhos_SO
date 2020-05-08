//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Descrição das funções utilizadas - cria as funções chamadas pelos processos

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syscall.h>

void cria_lista(){

    fila *aux;
    aux = (fila*)malloc(sizeof(fila));
    if(aux == NULL){
        printf("Não foi possivel alocar memoria para a lista de buffer\n");
        return(-1);
    }
    aux->mapa = BUFFER;
    aux->prox = NULL;

   /Cria um elemento fila para cada casa do buffer disponibilizado na lista de entrada de dados
    inicio.entrada = aux;
    for(int i = 1; i < TAM_BUFFER; i++){
        aux->prox = (fila*)malloc(sizeof(fila));
        if(aux->prox == NULL){
            printf("Não foi possivel alocar memoria para a lista de buffer\n");
            return(-1);
        }aux = aux->prox;
        aux->mapa = &BUFFER[i];
        aux->prox = NULL;
    }
    
    inicio.saida = NULL;
    aux = NULL;
    free(aux);
    return;
}
