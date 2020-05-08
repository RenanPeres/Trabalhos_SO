//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Declaração da biblioteca de funções utilizadas - declara as structs e as funções

#ifndef BIBLIOTECA_T2
#define BIBLIOTECA_T2


#define TAM_BUFFER 20;
#define TRUE 1;
#define FALSE 0;

int BUFFER[TAM_BUFFER];
int espaco_livre = TAM_BUFFER;


typedef struct{
    int *mapa;
    fila *prox;
} fila;

typedef struct{
    fila *entrada;
    fila *saida;
} ref_fila;

ref_fila inicio  = (ref_fila)malloc(sizeof(ref_fila));
if(inicio == NULL){
    printf("Não foi possivel alocar memoria para a lista de buffer\n");
    exit(-1);
}