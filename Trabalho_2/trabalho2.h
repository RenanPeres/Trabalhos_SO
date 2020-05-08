//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Declaração da biblioteca de funções utilizadas - declara as structs e as funções

#ifndef BIBLIOTECA_T2
#define BIBLIOTECA_T2

//Funções do programa
void cria_lista();                          //Cria e inicializa as listas de buffer livre e cheio
void troca_elementos(struct fila **f1, struct fila **f2); //Transfere o primeiro elemento da fila 1 para o final da fila 2
void produz_elemento(int dado);             //Escreve no buffer
int consome_elemento();                     //Lê o buffer
void *produtor();                            //Função produtor 
void *consumidor();                          //Função consumidor


#endif