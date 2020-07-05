//Login: gso14
//Trabalho de Sistemas Operacionais - Gerenciamento de memória
//Programa com simulador  de gerenciamento de memória virtual com paginação (algorítmo do tipo relógio)

//Bliblioteca
#include <stdio.h>
#include <stdlib.h>

//Definição dos parâmetros globais do gerenciador
#define TAM_PAGINA 8
#define MEM_RAM 700
#define MEM_VIRTUAL 1000
int andarilho = 0;

//Elemento da fila de processos
//int uso - Indica se foi utilizado recentemente
//int indice - Guarda o índice de seu quadro de alocação em memória
//char texto - arquivo de texto atribuido a essa página de memoria
struct pagina{
    int uso;
    int indice;
    char texto[TAM_PAGINA];
}

//Indicador da fila de processos
//int pid - identidade do processo
//int tamanho - tamanho em bytes do processo
//struct pagina *inicio - ponteiro para a lista de páginas de memória do processo
//struct processo *prox - ponteiro para o proximo processo da lista
struct processo{
    int pid;
    int tamanho;
    struct pagina *inicio;
    struct processo *prox;
}

//Fila de uso da memória
//int livre - indice do primeiro quadro de memoria livre, se houver
//struct pagina **ponteiro - ponteiro para um vetor de ponteiros de páginas de memória, onde estará armazenado
//as páginas alocadas em meoria principal ou secundária
struct memoria{
    int livre;
    struct pagina **ponteiro;
}


//Declara e inicia as struct de memória (ram - principal, virtual - secundaria)
struct memoria ram, virtual;
ram.livre = 0;
virtual.livre = 0;
ram.ponteiro = (struct pagina **)calloc(MEM_RAM, sizeof(struct pagina*));
if(ram.ponteiro == NULL){
    printf("Erro na alocacao de memoria das estruturas do gerenciador\n");
    return;
}
virtual.ponteiro = (struct pagina **)calloc(MEM_VIRTUAL, sizeof(struct pagina*));
if(virtual.ponteiro == NULL){
    printf("Erro na alocacao de memoria das estruturas do gerenciador\n");
    return;
}

//Funções do programa
void criar(struct processo **lista, int id, int tam); //Cria um processo e suas páginas no tamanho indicado (Função "C")
void trocar(struct pagina *aux); //troca uma página na memoria secundária para a memória principal pelo método do relógio 
void escrever(struct processo **lista, int id, int b_set); //encontra, coloca em memória principal, caso ñ estivesse, e escreve no endereço lógico (Função "W")
void ler(struct processo **lista, int id, int b_set); //encontra, coloca em memória principal, caso ñ estivesse, e lê no endereço lógico (Função "R")
void imprimir(struct processo **lista, int id); //Exibe a tabela de alocação de um processo ou o uso da memória (Função "A")

//Função main()
int main(){

    struct processo *fila;
    fila = NULL;
    char funcao;
    int dado1;
    int dado2;

    printf("Bem-vindo ao simulador de memoria do grupo 14\n\nTamanho da pagina de memoria: %d bytes\n", TAM_PAGINA);
    printf("Tamanho da RAM: %d paginas (%d bytes)\n", MEM_RAM, (MEM_RAM*TAM_PAGINA));
    printf("Tamanho da Virtual: %d paginas (%d bytes)\n\n", MEM_VIRTUAL, (MEM_VIRTUAL*TAM_PAGINA));

    while(1){
        scanf("%c", &funcao);
        switch (funcao){
            case 'C': //Criar
                scanf("%d", &dado1);
                scanf("%d", &dado2);
                criar(&fila, dado1, dado2);
                break;
            case 'W': //Escrever
                scanf("%d", &dado1);
                scanf("%d", &dado2);
                escrever(&fila, dado1, dado2);
                break;
            case 'R': //Ler
                scanf("%d", &dado1);
                scanf("%d", &dado2);
                ler(&fila, dado1, dado2);
                break;
            case 'P':

                break;
            case 'I':

                break;
            case 'A': //Imprimir
                scanf("%d", &dado1);
                criar(&fila, dado1);
                break;
            case 'X': //Sair
                return;
            default:
                printf("Opcao invalida\n");
        }
    }
}

//Criar um processo (Função "C")
//struct processo **lista - lista de processos
//int id - identidade do processo a ser criado
//int tam - tamanho, em bytes, do processo a ser criado
void criar(struct processo **lista, int id, int tam){

    struct processo *aux;
    aux = *lista;
    int num_quadros = (tam - (tam % TAM_PAGINA))/TAM_PAGINA;
    if(tam % TAM_PAGINA != 0) num_quadros ++;

//Cria todas as paginas de memoria utilizado em um processo e adiciona esse processo a lista de processos
    if(aux == NULL){
        aux = (struct processo *)malloc(sizeof(struct processo));
        if(aux == NULL){
            printf("Erro na alocacao de memoria das estruturas do gerenciador\n");
            return;
        }
        *lista = aux;
    }else{
        while(aux->prox != NULL){
            aux = aux->prox;
        }aux->prox = (struct processo *)malloc(sizeof(struct processo));
        if(aux->prox == NULL){
            printf("Erro na alocacao de memoria para o processo\n");
            return;
        }
        aux = aux->prox;
    }
    aux->pid = id;
    aux->tamanho = tam;
    aux->prox = NULL;

//Criando as páginas de memoria do processo e alocando em RAM
    aux->inicio = (struct pagina *)calloc(num_quadros, sizeof(struct pagina));
    if(aux->prox == NULL){
            printf("Erro na alocacao de memoria para o processo\n");
            return;
    }for(int i = 0; i < num_quadros; i++){
        if(ram.livre < MEM_RAM){
            ram.ponteiro[ram.livre] -> (aux->inicio[i]);
            ram.livre++;
        }else{
            while(ram.ponteiro[andarilho]->uso){
                ram.ponteiro[andarilho]->uso = 0;
                andarilho = (andarilho + 1) % MEM_RAM;
            }
            virtual.ponteiro[virtual.livre] = ram.ponteiro[andarilho];
            virtual.ponteiro[virtual.livre]->uso = -1;
            virtual.ponteiro[virtual.livre]->indice = virtual.livre;
            virtual.livre++;
            ram.ponteiro[andarilho] -> (aux->inicio[i]);
            andarilho = (andarilho + 1) % MEM_RAM;
        }
        aux->inicio[i].uso = 0;
        aux->inicio[i].indice = i;
    }
    return;
}

//Realiza a paginação por meio da estrategia do relógio
//struct pagina *aux - ponteiro para a página de um processo, que será alocada
void trocar(struct pagina *aux){

    while(ram.ponteiro[andarilho]->uso){
        ram.ponteiro[andarilho]->uso = 0;
        andarilho = (andarilho + 1) % MEM_RAM;
    }virtual.ponteiro[aux->indice] = ram.ponteiro[andarilho];
    virtual.ponteiro[aux->indice]->uso = -1;
    virtual.ponteiro[aux->indice]->indice = aux->indice;
    ram.ponteiro[andarilho] = aux;
    ram.ponteiro[andarilho]->uso = 0;
    ram.ponteiro[andarilho]->indice = andarilho;
    andarilho = (andarilho + 1) % MEM_RAM;  
    return;  
}

//Escrever em um endereço lógico (Função "W")
//struct processo **lista - lista de processos
//int id - identidade do processo a ser escrito
//int b_set - byte a ser escrito
void escrever(struct processo **lista, int id, int b_set){
    struct processo *aux;
    aux = *lista;
    int num_quadro = (b_set - (b_set % TAM_PAGINA))/TAM_PAGINA;
    if(b_set % TAM_PAGINA != 0) num_quadro ++;
    while(aux != NULL){
        if(aux->pid == id){
            if(aux->tamanho > b_set){
                if(aux->inicio[num_quadro].uso == -1) trocar(&aux->inicio[num_quadro]); //Caso a página não esteja em memória principal realiza a paginação
                (aux->inicio[num_quadro]).texto[b_set%TAM_PAGINA] = 1;
                return;
            }else{
                printf("o processo eh menor do que o informado\n");
                return;
            }
        }
        aux = aux->prox;
    }
    printf("Esse processo nao existe em memoria\n");
    return;
}

//Ler um endereço lógico (Função "R")
//struct processo **lista - lista de processos
//int id - identidade do processo a ser escrito
//int b_set - byte a ser escrito
void ler(struct processo **lista, int id, int b_set){
    struct processo *aux;
    aux = *lista;
    int num_quadro = (b_set - (b_set % TAM_PAGINA))/TAM_PAGINA;
    if(b_set % TAM_PAGINA != 0) num_quadro ++;
    while(aux != NULL){
        if(aux->pid == id){
            if(aux->tamanho > b_set){
                if(aux->inicio[num_quadro].uso == -1) trocar(&aux->inicio[num_quadro]); //Caso a página não esteja em memória principal realiza a paginação
                printf("Caracter lido: %c", (aux->inicio[num_quadro]).texto[b_set%TAM_PAGINA]);
                return;
            }else{
                printf("o processo eh menor do que o informado\n");
                return;
            }
        }
        aux = aux->prox;
    }
    printf("Esse processo nao existe em memoria\n");
    return;

}

//Imprimir a tabela de atribuição de memoria de um processo ou o uso da memória (Função "A")
//struct processo **lista - lista de processos
//int id - define qual processo terá sua tabela exibida (caso id = 0, exibe a disposição de dados na memória)
void imprimir(struct processo **lista, int id){
    struct processo *aux;
    aux = *lista;
    int num_quadros;

    if(flag == 0){ //Exibe o uso e os dados na memória
        printf("Uso da Memoria Principal:\n\n\n");
        for(int i = 0; i < ram.livre; i++){
            printf("Quadro: %d     Dado: %s     ", i, ram.ponteiro[i]->texto);
            if(ram.ponteiro[i]->uso)  printf("Segunda chance: sim\n");
            else printf("Segunda chance: nao\n");
        }printf("\n\n\n");
        printf("Uso da Memoria Secundaria:\n\n\n");
        for(int i = 0; i < virtual.livre; i++){
            printf("Quadro: %d     Dado: %s\n", i, ram.ponteiro[i]->texto);
        }printf("\n\n\n");
        return;
    }else{ //Exibe as atribuições de páginas de um processo id
        while(aux != NULL){
            if(aux->pid == id){
                printf("Mapa de alocacao do Processo %d\n", id);
                printf("Tamanho em bytes: %d\n\n", aux->tamanho);
                num_quadros = (aux->tamanho - (aux->tamanho%TAM_PAGINA))/TAM_PAGINA;
                if(aux->tamanho%TAM_PAGINA != 0)num_quadros++;
                for(int i = 0; i < num_quadros; i++){
                    printf("Pagina do Processo: %d     ", i);
                    if((aux->inicio[i]).uso == -1) printf("em Memoria Secundaria quadro %d\n", (aux->inicio[i]).indice);
                    else printf("em Memoria Principal quadro %d\n", (aux->inicio[i]).indice);
                    return;
                }
            }
            aux = aux->prox;
        }
        printf("Esse processo nao existe em memoria\n");
        return;
    }
}