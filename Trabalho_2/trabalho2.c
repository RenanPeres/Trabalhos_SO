//Login:gso14 att
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Descrição das funções utilizadas - cria as funções chamadas pelos processos

//Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include "trabalho2.h"

//Cria e inicializa as listas de buffer livre e cheio
void cria_lista(){

    //Elemento *fila auxiliar
    struct fila *aux;
    aux = (struct fila*)malloc(sizeof(struct fila));
    if(aux == NULL){
        printf("Não foi possivel alocar memoria para a lista de buffer\n");
        return;
    }
    aux->mapa = &BUFFER[0];
    aux->prox = NULL;

    //Cria um elemento fila para cada casa do buffer disponibilizado na lista de entrada de dados
    inicio.entrada = aux;
    for(int i = 1; i < 20; i++){
        aux->prox = (struct fila*)malloc(sizeof(struct fila));
        if(aux->prox == NULL){
            printf("Não foi possivel alocar memoria para a lista de buffer\n");
            return;
        }aux = aux->prox;
        aux->mapa = &BUFFER[i];
        aux->prox = NULL;
    }
    
    //Boas práticas de programação para liberar memória
    aux = NULL;
    free(aux);
    return;
}


//Transfere o primeiro elemento da fila 1 para o final da fila 2
//fila **f1 - ponteiro da fila que tera seu elemento excluido da lista
//fila **f2 - ponteiro da fila que tera um elemento adicionado à lista
void troca_elementos(struct fila **f1, struct fila **f2){
    
    //Elemento *fila auxiliar
    struct fila *aux;

    //Proucura o último elemento em **f2, caso a lista esteja vazia, cria o primeiro elemento
    aux = *f2;
    if(aux != NULL){
        while(aux->prox != NULL){
            aux = aux->prox;
        }aux->prox = *f1;
    }else{
        aux = (struct fila*)malloc(sizeof(struct fila));
        if(aux == NULL){
            printf("Não foi possivel alocar memoria para a lista de buffer\n");
            return;
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

//Função produtor 
void produtor(){

    int dado = 1;  //Variável dos dados a ser escrita no buffer
    int teste = 1; //Variável de validação da condição de thread
   
    while(dado < 200){

        //Verifica se tem espaço livre e, caso não tenha, coloca o processo em pausa
        pthread_mutex_lock(&thread_control);
        if(espaco_livre == 0){
            espera_produtor ++;
            while(teste) teste = pthread_cond_wait(&libera, &thread_control);
            espera_produtor --;
        }pthread_mutex_unlock(&buffer_control);

        //Escreve no buffer
        pthread_mutex_lock(&buffer_control);
        produz_elemento(dado);
        pthread_mutex_unlock(&buffer_control);

        //Verifica se o processo consumidor está em pausa e libera-o
        pthread_mutex_lock(&thread_control);
        if(espera_consumidor == 1) pthread_cond_signal(&libera);
        pthread_mutex_unlock(&thread_control);
        
        //Altera aleatoriamente o valor do dado
        dado  += (rand() % 15);
    }

    //Uma vez que todos os dados do produtor sejam escritos no buffer, um sinal fim é determinado para controle do consumidor
    fim = 1;
    return;
}

//Função consumidor
void consumidor(){

    int dado;       //Variável dos dados a ser lida no buffer
    int teste = 1;  //Variável de validação da condição de thread

    while(1){

        //Verifica se tem conteudo a ser consumido no buffer e, caso não tenha, coloca o processo em pausa
        pthread_mutex_lock(&thread_control);
        if(espaco_livre == 20){
            if(fim) return; //Verifica se o sinal de termino do produtor foi acionado, caso afirmativo, encerra sua execução
            espera_consumidor = 1;
            while(teste) teste = pthread_cond_wait(&libera, &thread_control);
            espera_consumidor = 0;
        }pthread_mutex_unlock(&buffer_control);

        //Lê o buffer
        pthread_mutex_lock(&buffer_control);
        dado = consome_elemento();
        pthread_mutex_unlock(&buffer_control);

        //Verifica se o processo produtor está em pausa e libera-o
        pthread_mutex_lock(&thread_control);
        if(espera_produtor == 1) pthread_cond_signal(&libera);
        pthread_mutex_unlock(&thread_control);

        //Exibe na tela o dado lido no buffer
        printf("%d\n", dado);

    }    
}
