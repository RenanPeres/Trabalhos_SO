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
    
    //Lista de saida de dados não possui elemento, então é sinalizado
    inicio.saida = NULL;
    
    //Boas práticas de programação para liberar memória
    aux = NULL;
    free(aux);
    return;
}
//Transfere o primeiro elemento da fila 1 para o final da fila 2
//fila **f1 - ponteiro da fila que tera seu elemento excluido da lista
//fila **f2 - ponteiro da fila que tera um elemento adicionado à lista
void troca_elementos(fila **f1, fila **f2){
    
    //Elemento *fila auxiliar
    fila *aux;

    //Proucura o último elemento em **f2, caso a lista esteja vazia, cria o primeiro elemento
    aux = *f2;
    if(aux != NULL){
