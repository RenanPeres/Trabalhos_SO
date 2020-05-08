//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Declaração da biblioteca de funções utilizadas - declara as structs e as funções

#ifndef BIBLIOTECA_T2
#define BIBLIOTECA_T2

//Constantes globais
#define TAM_BUFFER 20;
#define TRUE 1;
#define FALSE 0;

//Buffer limitado global (disponível a todos os processos)
int BUFFER[TAM_BUFFER];
int espaco_livre = TAM_BUFFER;

//Variáveis de sinal para saber se um processo esta em execução
int espera_produtor = FALSE;
int espera_consumidor = FALSE;
int fim = FALSE;

//Semáforos de controle das threads
pthread_mutex_t buffer_control = PTHREAD_MUTEX_INITIALIZER; //Controla o uso do buffer
pthread_mutex_t thread_control = PTHREAD_MUTEX_INITIALIZER; //Controla a execução de uma thread 
                                                            //(impede que duas threads durmam simultaneamente)
pthread_cond_t libera = PTHREAD_COND_INITIALIZER;           //Controla quando uma thread deve acordar

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

//declara e inicia a struct ref_fila para uso nas funções
struct ref_fila inicio;
inicio.entrada = NULL;
inicio.saida = NULL;

//Funções do programa
void cria_lista();                          //Cria e inicializa as listas de buffer livre e cheio
void troca_elementos(struct fila **f1, struct fila **f2); //Transfere o primeiro elemento da fila 1 para o final da fila 2
void produz_elemento(int dado);             //Escreve no buffer
int consome_elemento();                     //Lê o buffer
void produtor();                            //Função produtor 
void consumidor();                          //Função consumidor


#endif