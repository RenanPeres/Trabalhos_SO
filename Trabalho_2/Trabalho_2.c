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
pthread_cond_t libera = PTHREAD_COND_INITIALIZER;           //Controla quando uma thread deve acordar

//Elemento da fila de memoria disponível
//int indice - armazena o indice do buffer
//struct fila *prox - endereço do próximo elemento da fila
struct fila{
    int indice;
    struct fila *prox;
};

//Struct de ponteiro que armazena os endereços inicial de cada fila
//struct fila *entrada - armazena o endereço do primeiro elemento da fila de buffer preenchidos
//struct fila *fim - armazena o endereço do primeiro elemento da fila de buffer vazio
struct ref_fila{
    struct fila *entrada;
    struct fila *saida;
};

//declara e inicia a struct ref_fila para uso nas funções
struct ref_fila inicio, final;

//Funções do programa
void cria_lista();                         //Cria e inicializa as listas de buffer livre e cheio
void troca_elementos(int n); //Transfere o primeiro elemento da fila 1 para o final da fila 2
void produz_elemento(int dado);             //Escreve no buffer
int consome_elemento();                     //Lê o buffer
void *produtor();                            //Função produtor 
void *consumidor();                          //Função consumidor


int main(){

    pthread_t t1; //Thread do processo produtor
    pthread_t t2; //Thread do processo consumidor

    cria_lista(); //Cria e inicializa as listas de buffer livre e cheio
    printf("Foram lidos, no buffer:\n\n");
    if(pthread_create(&t1, NULL, produtor, NULL)){   //Inicia e testa o processo produtor
        fprintf(stderr,"ERRO - pthread_create()");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&t2, NULL, consumidor, NULL)){  //Inicia e testa o processo consumidor
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

    //Crindo a fila de entrada (buffer livre)
    inicio.entrada = (struct fila*)malloc(sizeof(struct fila));
    if(inicio.entrada == NULL){
        printf("Não foi possivel alocar memoria para a lista de buffer\n");
        return;
    }
    inicio.entrada->indice = 0;
    inicio.entrada->prox = NULL;
    final.entrada = inicio.entrada;
    for(int i = 1; i < 20; i++){
        final.entrada->prox = (struct fila*)malloc(sizeof(struct fila));
        if(final.entrada->prox == NULL){
            printf("Não foi possivel alocar memoria para a lista de buffer\n");
            return;
        }final.entrada = final.entrada->prox;
        final.entrada->indice = i;
        final.entrada->prox = NULL;
    }
    //Inicia o primeiro elemento da lista de saida
    inicio.saida = NULL;
    final.saida = NULL;

    return;
}


//Transfere o primeiro elemento da fila 1 para o final da fila 2
//int n - determina qual o metodo a ser realizado
void troca_elementos(int n){
 
    if(n == 0){
        (final.saida == NULL) ? inicio.saida = inicio.entrada : final.saida->prox = inicio.entrada;
        final.saida = inicio.entrada;
        final.saida->prox = NULL;
        inicio.entrada = inicio.entrada->prox;
    }else{
        if(final.entrada != NULL) final.entrada->prox = inicio.saida;
        final.entrada = inicio.saida;
        inicio.saida = inicio.saida->prox;
    }

    return;
}


//Escreve no buffer
//int dado - dado a ser escrito no buffer
void produz_elemento(int dado){
printf("entrou\n");
    //Escreve na primeira posição da lista de entrada
    BUFFER[(inicio.entrada)->indice] = dado;
    //Elimina esse elemento da lista de entrada e passa-o para a lista de saida
    troca_elementos(0);
    //Diminui o contador de espaçoes livres do buffer
    espaco_livre--;
    return;
}

//Lê o buffer
//return - retorna o dado lido no buffer
int consome_elemento(){
printf("entrou\n");
    //Le na primeira posição da lista de saida
    int dado = BUFFER[(inicio.saida)->indice];
    //Elimina esse elemento da lista de saida e passa-o para a lista de entrada
    troca_elementos(1);
    printf("saiu\n");
    //Aumenta o contador de espaçoes livres do buffer
    espaco_livre++;
    return dado;
}

//Função produtor 
void *produtor(){

    int dado = 1;  //Variável dos dados a ser escrita no buffer
    int teste = 1; //Variável de validação da condição de thread
  
   while(dado < 50){
        //Verifica se tem espaço livre e, caso não tenha, coloca o processo em pausa
        printf("10\n");
        pthread_mutex_lock(&thread_control);
        printf("11\n");
        if(espaco_livre == 0){
            espera_produtor ++;
            while(teste) teste = pthread_cond_wait(&libera, &thread_control);
            espera_produtor --;
        }pthread_mutex_unlock(&thread_control);
        printf("12\n");

        //Escreve no buffer
        pthread_mutex_lock(&buffer_control);
        printf("13\n");
        produz_elemento(dado);
        pthread_mutex_unlock(&buffer_control);
        printf("14\n");

        //Verifica se o processo consumidor está em pausa e libera-o
        pthread_mutex_lock(&thread_control);
        printf("15\n");
        if(espera_consumidor == 1) pthread_cond_signal(&libera);
        pthread_mutex_unlock(&thread_control);
        printf("16\n");
       
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

    while(1){

        //Verifica se tem conteudo a ser consumido no buffer e, caso não tenha, coloca o processo em pausa
        pthread_mutex_lock(&thread_control);
        printf("21\n");
        if(espaco_livre == 20){
            if(fim) exit(EXIT_SUCCESS); //Verifica se o sinal de termino do produtor foi acionado, caso afirmativo, encerra sua execução
            espera_consumidor = 1;
            printf("21\n");
            while(teste) teste = pthread_cond_wait(&libera, &thread_control);
            printf("21\n");
            espera_consumidor = 0;
        }pthread_mutex_unlock(&thread_control);
        printf("22\n");

        //Lê o buffer
        pthread_mutex_lock(&buffer_control);
        printf("23\n");
        dado = consome_elemento();
        printf("saiu\n");
        pthread_mutex_unlock(&buffer_control);
        printf("24\n");

        //Verifica se o processo produtor está em pausa e libera-o
        pthread_mutex_lock(&thread_control);
        printf("25\n");
        if(espera_produtor == 1) pthread_cond_signal(&libera);
        pthread_mutex_unlock(&thread_control);
        printf("26\n");

        //Exibe na tela o dado lido no buffer
        printf("%d\n", dado);

    }    
}
