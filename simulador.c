#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mem
{
    unsigned int endereco;
    char processo[2];
    int alteracao;
    int alocado;
} memoria;

typedef struct tab
{
    char processo[2];
    unsigned int *enderecoL, *enderecoF;
    int *alocado;
    int qtdPag;
    struct tab *prox;
} tabelaPaginas;

void inicializa_lista(tabelaPaginas **N);
tabelaPaginas *Cria_Nodo();
void imprime_lista_encadeada(tabelaPaginas *N);
int busca_lista(tabelaPaginas *N, char *nome, tabelaPaginas **R);
int retorna_indice_pagina(tabelaPaginas *N, unsigned int endereco);
void atualiza_tabela_pag(tabelaPaginas **N, int indice, char *nomeProcesso, int qtdPag, memoria **ram);
void imprime_memoria(memoria *ram, int qtdPag);
int retorna_pagina_do_endereco_lido(tabelaPaginas *N, unsigned int enderecoLido, unsigned int *pagina);

// ./simulador -p 8(tamanho da página em kb) -m 1024(tamanho da memoria em kb)
void main(int argc, char *argv[])
{
    int tamPag = 0, tamMem = 0, qtdPag, qtdPagProcesso, cont = 1, tamanho, indice;
    char nomeArq[20], op;
    unsigned int e = 0x0, eL, endereco, pagina;
    FILE *ftp;
    char unidade[3], nomeProcesso[3];
    tabelaPaginas *MyList, *retorno;
    inicializa_lista(&MyList);

    memoria *ram;

    if (argc != 6)
    {
        printf("Quantidade de parametros errados!");
        exit(0);
    }
    for (int i = 1; i < argc; i += 2)
    {
        if (!strcmp(argv[i], "-p"))
        {
            tamPag = atoi(argv[i + 1]);
            continue;
        }
        else if (!strcmp(argv[i], "-m"))
        {
            tamMem = atoi(argv[i + 1]);
        }
    }
    if (tamPag <= 0 || tamMem <= 0)
    {
        printf("Tamanho de página e memória devem ser maior que 0.");
        exit(0);
    }
    if (tamMem < tamPag)
    {
        printf("Tamanho de pagina deve ser menor que tamanho de memória.");
        exit(0);
    }
    if (((float)(tamMem % tamPag)) != 0)
    {
        printf("Tamanho de memória deve ser multiplo do tamanho de página.");
        exit(0);
    }
    strcpy(argv[5], nomeArq);

    qtdPag = tamMem / tamPag;

    ram = (memoria *)malloc(qtdPag * sizeof(memoria));
    printf("------------------------------Criando páginas da memória--------------------------------\n");
    for (int i = 0; i < qtdPag; i++)
    {
        ram[i].endereco = e;
        strcpy(ram[i].processo, "00");
        ram[i].alocado=0;
        ram[i].alteracao=0;
        printf("%X ", ram[i].endereco);
        e += tamPag;
        cont++;
        if (cont % 16 == 0)
            printf("\n");
    }
    printf("\n--------------------------------------------------------------------\n");

    if ((ftp = fopen("arquivo.txt", "r")) == NULL)
    {
        exit(0);
    }

    printf("\nLendo arquivo...\n");
    while (!feof(ftp))
    {
        fscanf(ftp,"%s %c",nomeProcesso,&op);
        if (op == 'C')
        {
            fscanf(ftp,"%d %s",&tamanho,unidade);
            if (!strcmp(unidade, "MB"))
            {
                tamanho *= 1024;
            }
            
            qtdPagProcesso = tamanho / tamPag;
            if (qtdPagProcesso < 1)
            {
                qtdPagProcesso = 1;
            }
            else if ((float)(tamanho % tamPag) != 0)
            {
                qtdPagProcesso++;
            }

            printf("\n\nProcesso %s criado.\nTamanho: %d\nQuantidade de Páginas: %d\n",nomeProcesso,tamanho,qtdPagProcesso); 
            
            tabelaPaginas *novo, *aux;
            novo = Cria_Nodo();
            novo->qtdPag=qtdPagProcesso;
            novo->enderecoL = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
            novo->enderecoF = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
            novo->alocado = (int*) malloc(qtdPagProcesso*sizeof(int));
            strcpy(novo->processo,nomeProcesso);
            novo->prox = NULL;
            if (MyList == NULL)
                MyList = novo;
            else
            {
                aux = MyList;
                while (aux->prox != NULL)
                    aux = aux->prox;
                aux->prox = novo;
            }

            eL=0x0;
            for (int i = 0; i < qtdPagProcesso; i++)
            {
                novo->enderecoL[i]=eL;
                novo->enderecoF[i]=NULL;
                novo->alocado[i]=0;
                eL+=tamPag;
            }
            printf("\n\n");
            continue;
        }else{
            indice=-1;
            fscanf(ftp,"%x",&endereco);
            if(!busca_lista(MyList,nomeProcesso,&retorno)){
                printf("ERRO NAO ACHOU PROCESSO!");
                exit(0);
            }
            if(op=='R')
                printf("\nOperaçao de Leitura para o processo %s no endereço %x\n",nomeProcesso,endereco);
            else
                printf("\nOperaçao de Leitura para o processo %s no endereço %x",nomeProcesso,endereco);
            
            if(retorna_pagina_do_endereco_lido(retorno, endereco, &pagina)){
                printf("\nEndereço pertence à página %x\n",pagina);
                indice = retorna_indice_pagina(retorno,pagina);
                if(indice==-1){
                    printf("ERRO NAO ACHOU ENDERECO LOGICO NO VETOR!");
                    exit(0);
                }
                atualiza_tabela_pag(&retorno, indice, nomeProcesso, qtdPag, &ram);
                printf("\nOperação finalizada\n-----------------------------------------------\n");
            }
        }
    }

    fclose(ftp);
    imprime_memoria(ram, qtdPag);
    imprime_lista_encadeada(MyList);
}

void inicializa_lista(tabelaPaginas **N) //inicializa a lista
{
    *N = NULL;
}

tabelaPaginas *Cria_Nodo() //aloca memória para o nodo
{
    tabelaPaginas *p;
    p = (tabelaPaginas *)malloc(sizeof(tabelaPaginas));
    if (!p)
    {
        printf("Problema de alocação");
        exit(0);
    }
    return p;
}

void imprime_lista_encadeada(tabelaPaginas *N)
{
    tabelaPaginas *aux;
    if (N == NULL)
        printf("\n A lista está vazia!!");
    else
    {
        for (aux = N; aux != NULL; aux = aux->prox){
            printf("\n%s\n----Tabela de Páginas----\nE Lógico x E Físico\n",aux->processo);
            for(int i=0;i<aux->qtdPag;i++){
                if(!aux->alocado[i]){
                    printf("%x - N/A\n",aux->enderecoL[i]);
                }else{
                    printf("%x - %x\n",aux->enderecoL[i],aux->enderecoF[i]);
                }
            }
        }
    }
}

int busca_lista(tabelaPaginas *N, char *nome, tabelaPaginas **R)
{
    int achou = 0;
    tabelaPaginas *aux;

    for (aux = N; aux != NULL; aux = aux->prox)
    {
        if (!strcmp(aux->processo, nome))
        {
            achou = 1;
            (*R)=aux;
            break;
        }
    }
    if (achou)
        return 1;
    return 0;
}

int retorna_indice_pagina(tabelaPaginas *N, unsigned int endereco){
    for(int i=0;i<N->qtdPag;i++){
        if(N->enderecoL[i]==endereco){
            return i;
        }
    }
    return -1;
}

void atualiza_tabela_pag(tabelaPaginas **N, int indice, char *nomeProcesso, int qtdPag, memoria **ram){
    if(!(*N)->alocado[indice]){
        printf("\nMemória não alocada para endereço lógico %x, buscando espaço em memória...\n",(*N)->enderecoL[indice]);
        for(int i=0;i<qtdPag;i++){
            if(!(*ram)[i].alocado){
                printf("\nEspaço encontrado no endereço %x, alocando para o processo...\n",(*ram)[i].endereco);
                strcpy((*ram)[i].processo,nomeProcesso);
                (*ram)[i].alocado=1;
                printf("\nAtualizando tabela de páginas, %x corresponde à %x\n",(*N)->enderecoL[indice],(*N)->enderecoF[indice]);
                (*N)->enderecoF[indice]=(*ram)[i].endereco;
                (*N)->alocado[indice]=1;
                break;
            }
        }
    }
}

void imprime_memoria(memoria *ram, int qtdPag){
    printf("------------------------------Memória--------------------------------\n");
    for (int i = 0; i < qtdPag; i++)
    {
        printf("%X - %s - %d\n", ram[i].endereco, ram[i].processo, ram[i].alteracao);
    }
    printf("\n--------------------------------------------------------------------\n");   
}

int retorna_pagina_do_endereco_lido(tabelaPaginas *N, unsigned int enderecoLido, unsigned int *pagina){
    int i;
    if(N->qtdPag==1){
        *pagina=N->enderecoL[0];
        return 1;
    }
    for(i=0;i<(N->qtdPag-1);i++){
        if(enderecoLido > N->enderecoL[i] && enderecoLido < N->enderecoL[i+1]){
            *pagina=N->enderecoL[i];
            break;
        }
        if(enderecoLido == N->enderecoL[i]){
            *pagina=N->enderecoL[i];
            break;
        }
    }
    if(N->enderecoF[i]!=NULL){
        return 0;
    }
    return 1;
}