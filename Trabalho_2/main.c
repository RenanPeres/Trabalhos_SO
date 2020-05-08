//Login: gso14
//Trabalho de Sistemas Operacionais - Produtor X Consumidor
//Código main - Cria as threads dos processos produtores e consumidores e espera até que sejam concluidas

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "trabalho2.h"

int main(){
    pthread_t t1, t2; //Thread do processo produtor e consumidor
    int p, c, r[2]; //Variável que carrega o status de sucesso ou erro de criação

    cria_lista(); //Cria e inicializa as listas de buffer livre e cheio
    printf("Foram lidos, no buffer:\n\n");
    if(pthread_create(&t1, NULL, (void *) produtor, NULL)){   //Inicia e testa o processo produtor
        fprintf(stderr,"ERRO - pthread_create() retornou: %d\n",p);
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&t2, NULL, (void *) consumidor, NULL)){  //Inicia e testa o processo consumidor
        fprintf(stderr,"ERRO - pthread_create() retornou: %d\n",c);
        exit(EXIT_FAILURE);
    }

    //Espera a conclusão das threads
    r[0] = pthread_join(t1, NULL);
    r[1] = pthread_join(t2, NULL);

    //Verifica se as threads encerraram corretamente
    if(r[0] == 0 && r[1] == 0) exit(EXIT_SUCCESS);
    else exit(EXIT_FAILURE);
}
