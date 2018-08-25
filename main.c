#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodom{
    char latitude[1000];
    char longitude[1000];
    char tipo[1000];
    char km[1000];
    struct nodom *prox;
}NodoMarco;

typedef struct nodos{
    char nomeSegmento[1000];
    NodoMarco *listaMarco;
    struct nodos *prox;
}NodoSegmento;

typedef struct nodor{
    char nomeRodovia[1000];
    NodoSegmento *listaSegmento;
    struct nodor *prox;
}NodoRodovia;

void insereMarco(char *latitude, char *longitude, char *tipo, char *km, NodoMarco **listaMarco){
    NodoMarco *novo = (NodoMarco*)malloc(sizeof(NodoMarco));
    if(novo == NULL){
        printf("Sem memoria para Marcos.\n");
        exit(1);
    }

    strcpy(novo->latitude, latitude);
    strcpy(novo->longitude, longitude);
    strcpy(novo->tipo, tipo);
    strcpy(novo->km, km);
    novo->prox = NULL;

    if(*listaMarco == NULL){
        *listaMarco = novo;
        return;
    }

    NodoMarco *aux = *listaMarco;
    while((aux) && (aux->prox)){
        aux = aux->prox;
    }

    aux->prox = novo;
}

void insereSegmento(char *nomeSegmento, char *latitude, char *longitude, char *tipo, char *km, NodoSegmento **listaSegmento){
    NodoSegmento *novo = (NodoSegmento*)malloc(sizeof(NodoSegmento));
    if(novo == NULL){
        printf("Sem memoria para Segmento.\n");
        exit(1);
    }

    strcpy(novo->nomeSegmento, nomeSegmento);
    novo->listaMarco = NULL;
    novo->prox = NULL;

    if(*listaSegmento == NULL){
        insereMarco(latitude, longitude, tipo, km, &novo->listaMarco);
        *listaSegmento = novo;
        return;
    }

    NodoSegmento *aux = *listaSegmento;
    while((aux) && (aux->prox) && (strcmp(nomeSegmento, aux->nomeSegmento) != 0)){
        aux = aux->prox;
    }

    if(strcmp(nomeSegmento, aux->nomeSegmento) == 0){
        insereMarco(latitude, longitude, tipo, km, &aux->listaMarco);
        return;
    }else{
        insereMarco(latitude, longitude, tipo, km, &novo->listaMarco);
        aux->prox = novo;
        return;
    }
}

void insereRodovia(char *nomeRodovia, char *nomeSegmento, char *latitude, char *longitude, char *tipo, char *km, NodoRodovia **listaRodovia){
    NodoRodovia *novo= (NodoRodovia*)malloc(sizeof(NodoRodovia));
    if(novo == NULL) {
        printf("Sem memoria para Rodovias.\n");
        exit(1);
    }

    strcpy(novo->nomeRodovia, nomeRodovia);
    novo->listaSegmento = NULL;
    novo->prox = NULL;


    if(*listaRodovia == NULL){
        insereSegmento(nomeSegmento, latitude, longitude, tipo, km, &novo->listaSegmento);
        *listaRodovia = novo;
        return;
    }

    NodoRodovia *aux = *listaRodovia;
    while((aux) && (aux->prox) && (strcmp(nomeRodovia, aux->nomeRodovia)) !=0){
        aux=aux->prox;
    }

    if((strcmp(nomeRodovia, aux->nomeRodovia)) == 0){
        insereSegmento(nomeSegmento, latitude, longitude, tipo, km, &aux->listaSegmento);
        return;
    }else{
        insereSegmento(nomeSegmento, latitude, longitude, tipo, km, &novo->listaSegmento);
        aux->prox = novo;
        return;
    }
}

void lerArquivoSegmento(FILE *arquivo, NodoRodovia **listaRodovia){
    if(!(arquivo)){
        printf("Sem arquivo no Marco.\n");
        exit(1);
    }
    char linha[10000];
    char *rodovia;
    char *segmento;
    char *latitude;
    char *longitude;
    char *tipo;
    char *km;

    fgets(linha, 10000, arquivo);
    while(fgets(linha, 10000, arquivo)){
        rodovia = strtok(linha, ";");
        segmento = strtok(NULL, ";");
        latitude = strtok(NULL, ";");
        longitude = strtok(NULL, ";");
        tipo = strtok(NULL, ";");
        km = strtok(NULL, "");

        insereRodovia(rodovia, segmento, latitude, longitude, tipo, km, listaRodovia);
    }
}

void criaArquivoKML(FILE *arquivoKML, NodoRodovia *listaRodovia){
    if(listaRodovia == NULL){
        printf("Sem lista de rodovias na funcao criaArquivoKML.\n");
        exit(1);
    }

    if(arquivoKML == NULL){
        printf("Sem arquivo na funcao criaArquivoKML.\n");
        exit(1);
    }

    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", arquivoKML);
    fputs("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n", arquivoKML);
    fputs("<Folder>\n", arquivoKML);
    fputs("<name>Marcos Rodoviarios</name>\n", arquivoKML);
    fputs("<description>Pasta raiz do meu projeto</description>\n", arquivoKML);
    fputs("<open>1</open>\n", arquivoKML);
    fputs("<visibility>1</visibility>\n", arquivoKML);

    while(listaRodovia){
        fputs(" <Folder>\n", arquivoKML);
        fprintf(arquivoKML, " <name>%s</name>\n", listaRodovia->nomeRodovia);
        fputs("<description>Pasta de Rodovia</description>\n", arquivoKML);
        fputs(" <open>0</open>\n", arquivoKML);
        fputs(" <visibility>1</visibility>\n", arquivoKML);

        while(listaRodovia->listaSegmento){
            fputs("  <Folder>\n", arquivoKML);
            fprintf(arquivoKML, "  <name>%s</name>\n", listaRodovia->listaSegmento->nomeSegmento);
            fputs("<description>Pasta de Segmentos Rodoviarios</description>\n", arquivoKML);
            fputs("  <open>0</open>\n", arquivoKML);
            fputs("  <visibility>1</visibility>\n", arquivoKML);

            while(listaRodovia->listaSegmento->listaMarco){
                fputs("   <Placemark>\n", arquivoKML);
                fprintf(arquivoKML, "   <name>%s</name>\n", listaRodovia->listaSegmento->listaMarco->tipo);
                fprintf(arquivoKML, "   <description>%s</description>\n", listaRodovia->listaSegmento->listaMarco->km);
                fputs("   <Point>\n", arquivoKML);
                fputs("   <coordinates>\n", arquivoKML);
                fprintf(arquivoKML, "%s,%s,\n", listaRodovia->listaSegmento->listaMarco->longitude, listaRodovia->listaSegmento->listaMarco->latitude);

                fputs("    </coordinates>\n", arquivoKML);
                fputs("    </Point>\n", arquivoKML);
                fputs("   </Placemark>\n", arquivoKML);
                listaRodovia->listaSegmento->listaMarco = listaRodovia->listaSegmento->listaMarco->prox;
            }

            fputs("  </Folder>\n", arquivoKML);
            listaRodovia->listaSegmento = listaRodovia->listaSegmento->prox;
        }

        fputs(" </Folder>\n", arquivoKML);
        listaRodovia = listaRodovia->prox;
    }

    fputs("</Folder>\n", arquivoKML);
    fputs("</kml>\n", arquivoKML);
}

void print(NodoRodovia *listaRodovia){
    if(listaRodovia == NULL){
        printf("Lista de Rodovias Vazia.\n");
        return;
    }

    while(listaRodovia != NULL){
        while(listaRodovia->listaSegmento != NULL){
            while(listaRodovia->listaSegmento->listaMarco != NULL){
                printf("%s\n", listaRodovia->nomeRodovia);
                printf("%s\n", listaRodovia->listaSegmento->nomeSegmento);
                printf("%s\n", listaRodovia->listaSegmento->listaMarco->latitude);
                printf("%s\n", listaRodovia->listaSegmento->listaMarco->longitude);
                printf("%s\n", listaRodovia->listaSegmento->listaMarco->tipo);
                printf("%s\n", listaRodovia->listaSegmento->listaMarco->km);

                listaRodovia->listaSegmento->listaMarco = listaRodovia->listaSegmento->listaMarco->prox;
            }
            listaRodovia->listaSegmento = listaRodovia->listaSegmento->prox;
        }
        listaRodovia = listaRodovia->prox;
    }
}

int main() {
    FILE *arquivo;
    NodoRodovia *listaRodovia = NULL;
    /*
    char nomeDoArquivo[100];


    printf("Informe o nome do arquivo.\n");
    scanf("%s", &nomeDoArquivo);


    if((arquivo = fopen(nomeDoArquivo, "r")) == NULL){
        printf("Erro no nome do arquivo.\n");
        exit(1);
    }
     */

    if((arquivo = fopen("C:\\Users\\Lucas\\CLionProjects\\Trabalho de LP1 do surf\\Dados_Rodovias_Bahia.csv", "r")) == NULL){
        printf("Sem arquivo.\n");
        exit(1);
    }

    lerArquivoSegmento(arquivo, &listaRodovia);

    FILE *arquivoKML;
    if((arquivoKML = fopen("C:\\Users\\Lucas\\CLionProjects\\Trabalho de LP1 do surf\\Arquivo_Marcos_Rodoviarios.kml", "w")) == NULL){
        printf("Erro ao criar o arquivo.\n");
        exit(1);
    }
    criaArquivoKML(arquivoKML, listaRodovia);

    printf("Done!\n");
    fclose(arquivo);
    fclose(arquivoKML);

    return 0;
}
