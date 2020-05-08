//Login:gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Descrição das funções utilizadas - cria as funções chamadas pelos processos

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>


//Cria e inicializa as listas de buffer livre e cheio
void cria_lista(){

    //Elemento *fila auxiliar
    fila *aux;
    aux = (fila*)malloc(sizeof(fila));
    if(aux == NULL){
        printf("Não foi possivel alocar memoria para a lista de buffer\n");
        return(-1);
    }
    aux->mapa = BUFFER;
    aux->prox = NULL;

    //Cria um elemento fila para cada casa do buffer disponibilizado na lista de entrada de dados
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
        while(aux->prox != NULL){
            aux = aux->prox;
        }aux->prox = *f1;
    }else{
        aux = (fila*)malloc(sizeof(fila));
        if(aux == NULL){
            printf("Não foi possivel alocar memoria para a lista de buffer\n");
            return(-1);
        }*f2 = *f1;
    }

    //Elimina o primeiro elemento da fila **f1
    *f1 = (*f1)->prox;

    //Boas práticas de programação para liberar memória
    aux = NULL;
    free(aux);
    return;
}


//Escreve no buffer
//int dado - dado a ser escrito no buffer
void produz_elemento(int dado){

    //Escreve na primeira posição da lista de entrada
    *((inicio.entrada)->mapa) = dado;

    //Elimina esse elemento da lista de entrada e passa-o para a lista de saida
    troca_elementos(&inicio.entrada, &inicio.saida);

    //Diminui o contador de espaçoes livres do buffer
    espaco_livre--;

    return;
}


//Lê o buffer
//return - retorna o dado lido no buffer
int consome_elemento(){

    //Le na primeira posição da lista de saida
    int dado = *((inicio.saida)->mapa);

    //Elimina esse elemento da lista de saida e passa-o para a lista de entrada
    troca_elementos(&inicio.saida, &inicio.entrada);

    //Aumenta o contador de espaçoes livres do buffer
    espaco_livre++;

    return dado;
}
