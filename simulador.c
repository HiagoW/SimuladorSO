//Fernando Luiz Hoflinger
//Hiago William Petris
//Leonardo Barobsa Marques
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct mem
{
    unsigned int endereco;
    char processo[3];
    int alteracao;
    int alocado;
    int referenciado;
} memoria;

typedef struct tab
{
    char processo[3];
    unsigned int *enderecoL, *enderecoF;
    int *alocado;
    int qtdPag, contSwap, tamanho;
    struct tab *prox;
} tabelaPaginas;

typedef struct filo{
    char nome[3];
    struct filo *prox;
}fila;

typedef struct sw{
    char nome[3];
    unsigned int endereco;
    struct sw *prox;
}swap;

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

void inicializa_lista(tabelaPaginas **N);
tabelaPaginas *Cria_Nodo();
void imprime_lista_encadeada(tabelaPaginas *N, int tamPag);
int busca_lista(tabelaPaginas *N, char *nome, tabelaPaginas **R);
int retorna_indice_pagina(tabelaPaginas *N, unsigned int endereco);
void atualiza_tabela_pag(tabelaPaginas **N, tabelaPaginas **tabela, fila **fila, int indice, char *nomeProcesso, int qtdPag, memoria **ram, char op, swap **S);
void imprime_memoria(memoria *ram, int qtdPag);
int retorna_pagina_do_endereco_lido(tabelaPaginas *N, unsigned int enderecoLido, unsigned int *pagina);
void insere_fila(fila **N, char *nome);
int remove_fila(fila **N);
int busca_fila(fila *N, char *nome);
int exclui_elem_fila(fila **N, char *nome);
fila *Cria_Nodo_Fila();
void inicializa_fila(fila **N);
void imprime_fila(fila *N);
void desaloca_processo(memoria **ram, fila **N, tabelaPaginas **pags, int qtdPag, swap **S);
void inicializa_swap(swap **N);
swap *Cria_Swap();
void insere_swap(swap **N, char *nome, unsigned int endereco);
int remove_swap(swap **N, char *nome, unsigned int endereco);
void imprime_swap(swap *N);

int algoritmo;
int indiceMem=0;

// ./simulador -p 8(tamanho da página em kb) -m 1024(tamanho da memoria em kb) arquivo.txt
void main(int argc, char *argv[])
{
    printf("\nEscolha o Algoritmo de Gerenciamento de Memória\n1.LRU\n2.LRU c/ Digito\n3.Segunda chance:  ");
    scanf("%d",&algoritmo);
    int tamPag = 0, tamMem = 0, qtdPag, qtdPagProcesso, cont = 1, tamanho, indice;
    char nomeArq[50], op;
    unsigned int e = 0x0, eL, endereco, pagina;
    FILE *ftp;
    char unidade[3], nomeProcesso[3];
    tabelaPaginas *MyList, *retorno;
    fila *processos;
    swap *Swap;
    inicializa_lista(&MyList);
    inicializa_swap(&Swap);

    memoria *ram;

    if (argc != 6)
    {
        delay(750); printf("Quantidade de parametros errados!");
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
        delay(750); printf("Tamanho de página e memória devem ser maior que 0.");
        exit(0);
    }
    if (tamMem < tamPag)
    {
        delay(750); printf("Tamanho de pagina deve ser menor que tamanho de memória.");
        exit(0);
    }
    if (((float)(tamMem % tamPag)) != 0)
    {
        delay(750); printf("Tamanho de memória deve ser multiplo do tamanho de página.");
        exit(0);
    }
    strcpy(nomeArq, argv[5]);

    qtdPag = tamMem / tamPag;

    ram = (memoria *)malloc(qtdPag * sizeof(memoria));
    delay(750); printf("------------------------------Criando páginas da memória--------------------------------\n");
    for (int i = 0; i < qtdPag; i++)
    {
        ram[i].endereco = e;
        strcpy(ram[i].processo, "00");
        ram[i].alocado=0;
        ram[i].alteracao=0;
        delay(750); printf("%X ", ram[i].endereco);
        e += tamPag;
        cont++;
        if (cont % 16 == 0)
            delay(750); printf("\n");
    }
    delay(750); printf("\n--------------------------------------------------------------------\n");

    if ((ftp = fopen(nomeArq, "r")) == NULL)
    {
        exit(0);
    }

    delay(750); printf("\nLendo arquivo...\n");
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

            delay(750); printf("\n\nProcesso %s criado.\nTamanho: %d\nQuantidade de Páginas: %d\n",nomeProcesso,tamanho,qtdPagProcesso); 
            delay(750); printf("\nOperação finalizada\n-----------------------------------------------\n");
            tabelaPaginas *novo, *aux;
            novo = Cria_Nodo();
            novo->qtdPag=qtdPagProcesso;
            novo->enderecoL = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
            novo->enderecoF = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
            novo->alocado = (int*) malloc(qtdPagProcesso*sizeof(int));
            novo->tamanho=tamanho;
            novo->contSwap=0;
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
            delay(750); printf("\n\n");
            continue;
        }else{
            indice=-1;
            fscanf(ftp,"%x",&endereco);
            if(!busca_lista(MyList,nomeProcesso,&retorno)){
                delay(750); printf("ERRO NAO ACHOU PROCESSO!");
                exit(0);
            }
            if(op=='R'){
                delay(750); printf("\nOperaçao de Leitura para o processo %s no endereço %x\n",nomeProcesso,endereco);
            }else{
                delay(750); printf("\nOperaçao de Gravação para o processo %s no endereço %x\n",nomeProcesso,endereco);
            }
            if(retorna_pagina_do_endereco_lido(retorno, endereco, &pagina)){
                delay(750); printf("\nEndereço pertence à página %x\n",pagina);
                indice = retorna_indice_pagina(retorno,pagina);
                if(indice==-1){
                    delay(750); printf("ERRO NAO ACHOU ENDERECO LOGICO NO VETOR!");
                    exit(0);
                }
                if(busca_fila(processos, nomeProcesso)==1){
                    exclui_elem_fila(&processos, nomeProcesso);
                    insere_fila(&processos, nomeProcesso);
                }else if(!busca_fila(processos, nomeProcesso)){
                    insere_fila(&processos, nomeProcesso);
                }
                atualiza_tabela_pag(&retorno, &MyList, &processos, indice, nomeProcesso, qtdPag, &ram, op, &Swap);
                //imprime_memoria(ram,qtdPag);
                //imprime_fila(processos);
                //imprime_lista_encadeada(MyList);
                delay(750); printf("\nOperação finalizada\n-----------------------------------------------\n");
            }else{
                delay(750); printf("\nProcesso tentando acessar endereço maior que o disponível!\n-----------------------------------------------\n");
            }
        }
    }
    delay(750); printf("\nGravando dados da SWAP no HD...\n");
    fclose(ftp);
    imprime_memoria(ram, qtdPag);
    imprime_lista_encadeada(MyList,tamPag);
    imprime_swap(Swap);
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
        delay(750); printf("Problema de alocação");
        exit(0);
    }
    return p;
}

fila *Cria_Nodo_Fila() //aloca memória para o nodo
{
    fila *p;
    p = (fila *)malloc(sizeof(fila));
    if (!p)
    {
        delay(750); printf("Problema de alocação");
        exit(0);
    }
    return p;
}

void inicializa_fila(fila **N) //inicializa a lista
{
    *N = NULL;
}

void imprime_lista_encadeada(tabelaPaginas *N, int tamPag)
{
    tabelaPaginas *aux;
    float porcent;
    if (N == NULL){
        delay(750); printf("\n A lista está vazia!!");
    }else
    {
        for (aux = N; aux != NULL; aux = aux->prox){
            //delay(750); printf("%d %d %d",aux->contSwap,aux->tamanho,tamPag);
            porcent=(float)(((float)aux->contSwap*tamPag)/(float)aux->tamanho)*100; 
            if(porcent>100)
                porcent=100;   
            delay(750); printf("\n%s - %.2f por cento do processo na swap \n----Tabela de Páginas----\nE Lógico x E Físico\n",porcent,aux->processo);
            for(int i=0;i<aux->qtdPag;i++){
                if(aux->alocado[i]==0){
                    delay(750); printf("%x - N/A\n",aux->enderecoL[i]);
                }else{
                    delay(750); printf("%x - %x - %d\n",aux->enderecoL[i],aux->enderecoF[i],aux->alocado[i]);
                }
            }
        }
    }
}

void imprime_fila(fila *N)
{
    fila *aux;
    if (N == NULL){
        delay(750); printf("\n A lista está vazia!!");
    }else
    {
        delay(750); printf("\n\n----------------------FILA DE PROCESSOS-----------------------\n");
        for (aux = N; aux != NULL; aux = aux->prox){
            delay(750); printf("[%s]",aux->nome);
        }
        delay(750); printf("\n\n");
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

void atualiza_tabela_pag(tabelaPaginas **N, tabelaPaginas **tabela, fila **fila, int indice, char *nomeProcesso, int qtdPag, memoria **ram, char op, swap **S){
    //delay(750); printf("\n%X - %d\n",(*N)->enderecoL[indice],indice);
    if((*N)->alocado[indice]==1 && (algoritmo == 3 || algoritmo == 2)){
        for(int i=0;i<qtdPag;i++){
            if((*N)->enderecoF[indice]==(*ram)[i].endereco){
                delay(750); printf("\nSetando bit de referencia para 1...\n");
                (*ram)[i].referenciado=1;
            }
        }
    }
    if(!(*N)->alocado[indice]){
        delay(750); printf("\nMemória não alocada para endereço lógico %x, buscando espaço em memória...\n",(*N)->enderecoL[indice]);
        for(int i=0;i<qtdPag;i++){
            if(!(*ram)[i].alocado){
                delay(750); printf("\nEspaço encontrado no endereço %x, alocando para o processo...\n",(*ram)[i].endereco);
                if(remove_swap(S,nomeProcesso,(*N)->enderecoL[indice])==1){
                    (*N)->contSwap--;
                    delay(750); printf("\nBuscando dados gravados na SWAP...\n");
                }
                strcpy((*ram)[i].processo,nomeProcesso);
                (*ram)[i].alocado=1;
                if(algoritmo!=1)
                    (*ram)[i].referenciado=1;
                if(op=='W'){
                    (*ram)[i].alteracao=1;
                }
                (*N)->enderecoF[indice]=(*ram)[i].endereco;
                delay(750); printf("\nAtualizando tabela de páginas, %x corresponde à %x\n",(*N)->enderecoL[indice],(*N)->enderecoF[indice]);
                (*N)->alocado[indice]=1;
                return;
            }
            if(i==(qtdPag-1)){
                delay(750); printf("\nMemória cheia, procurando página para desalocar...\n");
                if(algoritmo==1){
                    desaloca_processo(ram,fila,tabela,qtdPag,S);
                }else if(algoritmo==2){
                    unsigned int enderecoFD;
                    char nomeProcessoD[3];
                    tabelaPaginas *aux;
                    for(int j=0;j<qtdPag;j++){
                        //delay(750); printf("\n%d - %d\n",j,(*ram)[j].referenciado==0);
                        if((*ram)[j].referenciado==0){
                            delay(750); printf("\nAchou página com bit de referência 0...\n");
                            strcpy(nomeProcessoD,(*ram)[j].processo);
                            enderecoFD = (*ram)[j].endereco;
                            //Desaloca Endereço Fisico no Processo a ser desalocado
                            for(aux=(*tabela);aux!=NULL;aux=aux->prox){
                                if(!strcmp(aux->processo,nomeProcessoD)){
                                    for(int y=0;y<aux->qtdPag;y++){
                                        if(aux->enderecoF[y]==enderecoFD){
                                            aux->enderecoF[y]=NULL;
                                            aux->alocado[y]=0;
                                            for(int z=0;z<qtdPag;z++){
                                                if((*ram)[z].endereco==enderecoFD){
                                                    if((*ram)[z].alteracao==1){
                                                        aux->contSwap++;
                                                        insere_swap(S,nomeProcessoD,aux->enderecoL[y]);
                                                    }
                                                    strcpy((*ram)[z].processo,"00");
                                                    (*ram)[z].alocado=0;
                                                    (*ram)[z].alteracao=0;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            strcpy((*ram)[j].processo,nomeProcesso);
                            (*ram)[j].referenciado=1;
                            if(op=='W'){
                                (*ram)[j].alteracao=1;
                            }
                            (*N)->enderecoF[indice]=(*ram)[j].endereco;
                            (*N)->alocado[indice]=1;
                            delay(750); printf("\n%X - %X - %d\n",(*N)->enderecoF[indice],(*ram)[j].endereco,indice);
                            return;
                        }
                        if(j==(qtdPag-1)){
                            for(int j=0;j<qtdPag;j++){
                                (*ram)[j].referenciado=0;
                            }
                            j=-1;
                        }
                    }
                }else if(algoritmo==3){
                    unsigned int enderecoFD;
                    char nomeProcessoD[3];
                    tabelaPaginas *aux;
                    //Muda bit de referencia
                    for(int j=indiceMem;j<qtdPag;j++){
                        //delay(750); printf("\n%d - %d\n",j,indiceMem);
                        if((*ram)[j].referenciado==1){
                            delay(750); printf("\nMudando bit de referência de 1 para 0...\n");
                            (*ram)[j].referenciado=0;
                        }else{
                            delay(750); printf("\nAchou página com bit de referência 0...\n");
                            strcpy(nomeProcessoD,(*ram)[j].processo);
                            enderecoFD = (*ram)[j].endereco;
                            //Desaloca Endereço Fisico no Processo a ser desalocado
                            for(aux=(*tabela);aux!=NULL;aux=aux->prox){
                                if(!strcmp(aux->processo,nomeProcessoD)){
                                    for(int y=0;y<aux->qtdPag;y++){
                                        if(aux->enderecoF[y]==enderecoFD){
                                            aux->enderecoF[y]=NULL;
                                            aux->alocado[y]=0;
                                            for(int z=0;z<qtdPag;z++){
                                                if((*ram)[z].endereco==enderecoFD){
                                                    if((*ram)[z].alteracao==1){
                                                        aux->contSwap++;
                                                        insere_swap(S,nomeProcessoD,aux->enderecoL[y]);
                                                    }
                                                    strcpy((*ram)[z].processo,"00");
                                                    (*ram)[z].alocado=0;
                                                    (*ram)[z].alteracao=0;
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            strcpy((*ram)[j].processo,nomeProcesso);
                            (*ram)[j].referenciado=1;
                            if(op=='W'){
                                (*ram)[j].alteracao=1;
                            }
                            (*N)->enderecoF[indice]=(*ram)[j].endereco;
                            (*N)->alocado[indice]=1;
                            //delay(750); printf("\n%X - %X - %d\n",(*N)->enderecoF[indice],(*ram)[j].endereco,indice);
                            if(j==(qtdPag-1)){
                                indiceMem=0;
                                return;
                            }
                            indiceMem=j+1;
                            return;
                        }
                        if(j==(qtdPag-1)){
                            j=-1;
                        }
                    }
                }
                i=-1;
            }
        }
    }else{
        if(op=='W'){
            for(int i=0;i<qtdPag;i++){
                if((*ram)[i].endereco==(*N)->enderecoF[indice]){
                    (*ram)[i].alteracao=1;
                    break;
                }
            }

        }
    }
}

void imprime_memoria(memoria *ram, int qtdPag){
    delay(750); printf("------------------------------Memória--------------------------------\n");
    for (int i = 0; i < qtdPag; i++)
    {
        delay(750); printf("%X - %s - %d - %d\n", ram[i].endereco, ram[i].processo, ram[i].alteracao, ram[i].referenciado);
    }
    delay(750); printf("\n--------------------------------------------------------------------\n");   
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
            return 1;
        }
        if(enderecoLido == N->enderecoL[i]){
            *pagina=N->enderecoL[i];
            return 1;
        }
    }
    return 0;
}

int exclui_elem_fila(fila **N, char *nome)
{
    fila *aux, *aux2;
    int *dado2;
    if (*N == NULL)
        return 0;
    else
    {
        if (!(strcmp((*N)->nome, nome)))
        {
            remove_fila(N);
            return 1;
        }
        else
        {
            aux2 = *N;
            for(aux = (*N)->prox; aux->prox != NULL; aux = aux->prox)
            {
                if (!(strcmp(aux->nome, nome)))
                {    
                    aux2->prox = aux->prox;
                    free(aux);
                    return 1;
                }
                aux2=aux;
            }
            if(!strcmp(aux->nome, nome))
            {    
                delay(750); printf("\nNAO DEVERIA REMOVER DO FIM\n");
                return 0;
            }
        }
        //Elemento não encontrado
        return -1;
    }
}

int busca_fila(fila *N, char *nome)
{
    fila *aux;

    for (aux = N; aux != NULL; aux = aux->prox)
    {
        if (!strcmp(aux->nome, nome))
        {
            if(aux->prox==NULL){
                return 2;
            }
            return 1;
        }
    }
    return 0;
}

int remove_fila(fila **N)
{
    fila *aux;
    if (*N == NULL) //Verifica se a lista está vazia
        return 0;
    else
    {
        aux = (*N)->prox;
        free(*N);
        *N = aux;
    }
    return 1;
}

void insere_fila(fila **N, char *nome)
{
    fila *novo, *aux;
    novo = Cria_Nodo();
    strcpy(novo->nome, nome);
    novo->prox = NULL;
    if (*N == NULL){
        *N = novo;
    }
    else
    {
        aux = *N;
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }
}

void desaloca_processo(memoria **ram, fila **N, tabelaPaginas **pags, int qtdPag, swap **S){
    char nomeProcessoDesalocar[3];
    strcpy(nomeProcessoDesalocar,(*N)->nome);
    delay(750); printf("\nPágina do processo %s sendo desalocada...\n",nomeProcessoDesalocar);
    tabelaPaginas *aux;
    int controle=0;
    unsigned int enderecoFDesaloca, enderecoLDesaloca;
    //Percorre a struct dos processos
    for(aux=(*pags);aux!=NULL;aux=aux->prox){
        if(!strcmp(aux->processo, nomeProcessoDesalocar)){
            //Achou o processo que vai desalocar
            for(int i=0;i<aux->qtdPag;i++){
                //Acha a primeira página alocada e desaloca
                if(aux->alocado[i]){
                    enderecoFDesaloca = aux->enderecoF[i];
                    enderecoLDesaloca = aux->enderecoL[i];
                    aux->enderecoF[i]=NULL;
                    aux->alocado[i]=0;
                    break;
                }
            }
            for(int i=0;i<aux->qtdPag;i++){
                if(aux->alocado[i]){
                    controle=1;
                }
            }
            if(!controle){
                remove_fila(N);
            }
            break;
        }
    }
    
    for(int i=0;i<qtdPag;i++){
        if((*ram)[i].endereco==enderecoFDesaloca){
            if((*ram)[i].alteracao){
                aux->contSwap++;
                insere_swap(S,nomeProcessoDesalocar,enderecoLDesaloca);
            }
            strcpy((*ram)[i].processo,"00");
            (*ram)[i].alocado=0;
            (*ram)[i].alteracao=0;
        }
    }
}

void inicializa_swap(swap **N){
    *N = NULL;
}

swap *Cria_Swap()
{
    swap *p;
    p = (swap *)malloc(sizeof(swap));
    if (!p)
    {
        delay(750); printf("Problema de alocação");
        exit(0);
    }
    return p;
}

int remove_swap(swap **N, char *nome, unsigned int endereco)
{
    swap *aux, *aux2;
    if (*N == NULL) //Verifica se a lista está vazia
        return 0;
    else
    {
        if ((!(strcmp((*N)->nome, nome))) && ((*N)->endereco==endereco) )
        {
            aux = (*N)->prox;
            free(*N);
            *N = aux;
            return 1;
        }
        if((*N)->prox==NULL){
            return -1;
        }
        else
        {
            aux2 = *N;
            for(aux = (*N)->prox; aux->prox != NULL; aux = aux->prox)
            {
                if ((!(strcmp((*N)->nome, nome))) && ((*N)->endereco==endereco))
                {    
                    aux2->prox = aux->prox;
                    free(aux);
                    return 1;
                }
                aux2=aux;
            }
            
            if(!strcmp(aux->nome, nome))
            {    
                aux2->prox=NULL;
                free(aux);
                return 1;
            }
        }
        //Elemento não encontrado
        return -1;
    }
    return 1;
}

void insere_swap(swap **N, char *nome, unsigned int endereco)
{
    swap *novo, *aux;
    novo = Cria_Swap();
    strcpy(novo->nome, nome);
    novo->endereco = endereco;
    novo->prox = NULL;
    if (*N == NULL)
        *N = novo;
    else
    {
        aux = *N;
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }
}

void imprime_swap(swap *N)
{
    swap *aux;
    if (N == NULL){
        delay(750); printf("\n A lista está vazia!!");
    }else
    {
        delay(750); printf("\n\n----------------------SWAP-----------------------\n");
        for (aux = N; aux != NULL; aux = aux->prox){
            delay(750); printf("[%s] - [%x]",aux->nome,aux->endereco);
        }
        delay(750); printf("\n\n");
    }
}