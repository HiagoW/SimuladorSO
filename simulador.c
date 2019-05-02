#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ./simulador -p 8(tamanho da página em kb) -m 1024(tamanho da memoria em kb)
void main(int argc, char *argv[]){
    int tamPag=0, tamMem=0;
    if(argc!=5){
        printf("Quantidade de parametros errados!");
        exit(0);
    }
    for(int i=1;i<argc;i+=2){
        if(!strcmp(argv[i],"-p")){
            tamPag=atoi(argv[i+1]);
            continue;
        }else if(!strcmp(argv[i],"-m")){
            tamMem=atoi(argv[i+1]);
        }
    }
    if(tamPag<=0 || tamMem<=0){
        printf("Tamanho de página e memória devem ser maior que 0.");
        exit(0);
    }
    if(tamMem<tamPag){
        printf("Tamanho de pagina deve ser menor que tamanho de memória.");
        exit(0);
    }
    if(((float)(tamMem % tamPag))!=0){
        printf("Tamanho de memória deve ser multiplo do tamanho de página.");
        exit(0);
    }
}