//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Código main - Cria as threads dos processos produtores e consumidores e espera até que sejam concluidas

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <syscall.h>

//Buffer limitado global (disponível a todos os processos)
int BUFFER[20];
int espaco_livre = 20;

//Variáveis de sinal para saber se um processo esta em execução
int espera_produtor = 0;
int espera_consumidor = 0;
int fim = 0;

//Semáforos de controle das threads
pthread_mutex_t buffer_control = PTHREAD_MUTEX_INITIALIZER; //Controla o uso do buffer
pthread_mutex_t thread_control = PTHREAD_MUTEX_INITIALIZER; //Controla a execução de uma thread 
                                                            //(impede que duas threads durmam simultaneamente)
pthread_cond_t libera = PTHREAD_COND_INITIALIZER;           //Controla quando uma thread deve acordar

//declara e inicia a struct ref_fila para uso nas funções
struct ref_fila inicio;

//Elemento da fila de memoria disponível
//int *mapa - armazena o endereço no buffer
//struct fila *prox - endereço do próximo elemento da fila
struct fila{
    int *mapa;
    struct fila *prox;
};

//Struct de ponteiro que armazena os endereços inicial de cada fila
//struct fila *entrada - armazena o endereço do primeiro elemento da fila de buffer preenchidos
//struct fila *fim - armazena o endereço do primeiro elemento da fila de buffer vazio
struct ref_fila{
    struct fila *entrada;
    struct fila *saida;
};

//Funções do programa
void cria_lista();                         //Cria e inicializa as listas de buffer livre e cheio
void troca_elementos(struct fila **f1, struct fila **f2); //Transfere o primeiro elemento da fila 1 para o final da fila 2
void *produz_elemento();             //Escreve no buffer
void *consome_elemento();                     //Lê o buffer
//void *produtor();                            //Função produtor 
//void *consumidor();                          //Função consumidor
void *escreve();

int main(){

    pthread_t t1; //Thread do processo produtor
    pthread_t t2; //Thread do processo consumidor

    cria_lista(); //Cria e inicializa as listas de buffer livre e cheio
    printf("Foram lidos, no buffer:\n\n");
    if(pthread_create(&t1, NULL, produz_elemento, NULL)){   //Inicia e testa o processo produtor
        fprintf(stderr,"ERRO - pthread_create()");
        exit(EXIT_FAILURE);
    }sleep(3);
    if(pthread_create(&t2, NULL, consome_elemento, NULL)){  //Inicia e testa o processo consumidor
        fprintf(stderr,"ERRO - pthread_create()");
        exit(EXIT_FAILURE);
    }

    //Espera a conclusão das threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}

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
    //Inicia o primeiro elemento da lista de saida
    inicio.saida = NULL;
    
    //Boas práticas de programação para liberar memória
    aux = NULL;
    free(aux);
    return;
}

void *escreve(){
    printf("foi\n");
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
void *produz_elemento(){//int dado

    //Escreve na primeira posição da lista de entrada
    *((inicio.entrada)->mapa) = 2;

    //Elimina esse elemento da lista de entrada e passa-o para a lista de saida
    troca_elementos(&inicio.entrada, &inicio.saida);

    //Diminui o contador de espaçoes livres do buffer
    espaco_livre--;

    //return;
}

//Lê o buffer
//return - retorna o dado lido no buffer
void *consome_elemento(){

    //Le na primeira posição da lista de saida
    int dado = *((inicio.entrada)->mapa);

    //Elimina esse elemento da lista de saida e passa-o para a lista de entrada
    troca_elementos(&inicio.entrada, &inicio.saida);

    //Aumenta o contador de espaçoes livres do buffer
    espaco_livre++;
    printf("%d\n", dado);
    //return dado;
}
/*
//Função produtor 
void *produtor(){

    int dado = 1;  //Variável dos dados a ser escrita no buffer
    int teste = 1; //Variável de validação da condição de thread
   printf("3");
    while(dado < 50){

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
    exit(EXIT_SUCCESS);
}

//Função consumidor
void *consumidor(){

    int dado;       //Variável dos dados a ser lida no buffer
    int teste = 1;  //Variável de validação da condição de thread
	printf("4");
    while(1){

        //Verifica se tem conteudo a ser consumido no buffer e, caso não tenha, coloca o processo em pausa
        pthread_mutex_lock(&thread_control);
        if(espaco_livre == 20){
            if(fim) exit(EXIT_SUCCESS); //Verifica se o sinal de termino do produtor foi acionado, caso afirmativo, encerra sua execução
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
*/
