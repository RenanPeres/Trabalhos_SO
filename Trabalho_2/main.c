//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Código main - Cria as threads dos processos produtores e consumidores e espera até que sejam concluidas

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "trabalho2.h"

int main(){
    pthread_t t1; //Thread do processo produtor
    pthread_t t2; //Thread do processo consumidor
    int p, c, r[2]; //Variável que carrega o status de sucesso ou erro de criação
    printf("entrei\n");
    cria_lista(); //Cria e inicializa as listas de buffer livre e cheio
    printf("Foram lidos, no buffer:\n\n");
    if(pthread_create(&t1, NULL, produtor, (void *) NULL)){   //Inicia e testa o processo produtor
        fprintf(stderr,"ERRO - pthread_create() retornou: %d\n",p);
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&t2, NULL, consumidor, (void *) NULL)){  //Inicia e testa o processo consumidor
        fprintf(stderr,"ERRO - pthread_create() retornou: %d\n",c);
        exit(EXIT_FAILURE);
    }

    //Espera a conclusão das threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
