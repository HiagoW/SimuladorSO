#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mem
{
    unsigned int endereco;
    char processo[2];
} memoria;

typedef struct tab
{
    char processo[2];
    unsigned int *enderecoL, *enderecoF;
    int qtdPag;
    struct tab *prox;
} tabelaPaginas;

void inicializa_lista(tabelaPaginas **N);
tabelaPaginas *Cria_Nodo();
void imprime_lista_encadeada(tabelaPaginas *N);

// ./simulador -p 8(tamanho da página em kb) -m 1024(tamanho da memoria em kb)
void main(int argc, char *argv[])
{
    int tamPag = 0, tamMem = 0, qtdPag, qtdPagProcesso, cont = 1, tamanho;
    char nomeArq[20], op;
    unsigned int e = 0x0, eL;
    FILE *ftp;
    char unidade[3], nomeProcesso[3], endereco[7];
    tabelaPaginas *MyList;
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
        printf("0x%X ", ram[i].endereco);
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

    while (!feof(ftp))
    {
        fscanf(ftp,"%s %c %s",nomeProcesso,&op, endereco);
        if (op == 'C')
        {
            tamanho=atoi(endereco);
            fscanf(ftp,"%s",unidade);
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

            printf("\n\nProcesso %s\nTamanho: %d\nQuantidade de Páginas: %d\n",nomeProcesso,tamanho,qtdPagProcesso); 
            
            tabelaPaginas *novo, *aux;
            novo = Cria_Nodo();
            novo->qtdPag=qtdPagProcesso;
            novo->enderecoL = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
            novo->enderecoF = (unsigned int*) malloc(qtdPagProcesso*sizeof(unsigned int));
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
                eL++;
            }
            printf("\n\n");
            continue;
        }
    }

    fclose(ftp);
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
                if(aux->enderecoF[i]==NULL){
                    printf("0x%x - N/A\n",aux->enderecoL[i]);
                }else{
                    printf("0x%x - 0x%x\n",aux->enderecoL[i],aux->enderecoF[i]);
                }
            }
        }
    }
}