/*
 ============================================================================
 Name        : maquina_cache.c
 Author      : Lavínia Fonseca
 Version     : v.01
 Copyright   : Your copyright notice
 Description : Segundo TP de organizacaoo de computadores - Criacao da Maquina - Foco na Memoria Cache
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

typedef struct
{
    int *palavras;
    int endBloco;
    bool atualizado;
    int custo;
    int cacheHit;
    time_t segundos;
} BlocoMemoria;

typedef struct
{
    int endBloco;
    int endPalavra;
} Endereco;

typedef struct
{
    Endereco add1;
    Endereco add2;
    Endereco add3;
    int opcode;
} Instrucao;

int tamanhoRAM = 1000;
int qdePalavrasBloco = 4;
int tamanhoCache1 = 32;
int tamanhoCache2 = 64;
int tamanhoCache3 = 128;
int tamanhoPrograma = 10001;

Instrucao *memoriaInstrucoes;
BlocoMemoria *cache1;
BlocoMemoria *cache2;
BlocoMemoria *cache3;
BlocoMemoria *RAM;
FILE *arquivo;

time_t inicioPrograma;

//BlocoMemoria
void setCustoMemoria(BlocoMemoria *aux, int custoAux);
void setEndBlocoMemoria(BlocoMemoria *aux, int endBlocoAux);
void setPalavrasMemoria(BlocoMemoria *aux, int * palavrasAux);
void setCacheHitMemoria(BlocoMemoria *aux, int cacheHitAux);
void setSegundosMemoria(BlocoMemoria *aux, int segundosAux);
void setAtualizadoMemoria1(BlocoMemoria *aux);
void setAtualizadoMemoria(BlocoMemoria *aux);
void setDesatualizadoMemoria(BlocoMemoria *aux);
int getCustoMemoria(BlocoMemoria *aux);
int* getPalavrasMemoria(BlocoMemoria *aux);
int getEndBlocoMemoria(BlocoMemoria *aux);
bool isAtualizadoMemoria(BlocoMemoria *aux);


//Instrucao
void setOpcodeInstrucao(Instrucao *umaInstrucao, int opcodeAux);
void setAdd1Instrucao(Instrucao *umaInstrucao, Endereco *add1Aux);
void setAdd2Instrucao(Instrucao *umaInstrucao, Endereco *add2Aux);
void setAdd3Instrucao(Instrucao *umaInstrucao, Endereco *add3Aux);
int getOpcodeInstrucao(Instrucao *umaInstrucao);
Endereco* getAdd1Instrucao(Instrucao *umaInstrucao);
Endereco* getAdd2Instrucao(Instrucao *umaInstrucao);
Endereco* getAdd3Instrucao(Instrucao *umaInstrucao);

//Endereco
void setEndBlocoEndereco(Endereco *end, int endBlocoAux);
void setEndPalavraEndereco(Endereco *end, int endPalavraAux);
int getEndBlocoEndereco(Endereco *end);
int getEndPalavraEndereco(Endereco *end);

//Caches - MMU
void setEndBlocoCache(int endBlocoAux, int posicao, BlocoMemoria *qqCache);
void setCustoCache(int custoAux, int posicao, BlocoMemoria *qqCache);
void setCacheHitCache(int cacheHitAux, int posicao, BlocoMemoria *qqCache);
void setAtualizadoCache(int posicao, BlocoMemoria *qqCache);
void setDesatualizadoCache(int posicao, BlocoMemoria *qqCache);
void setSegundosCache(int segundoAux, int posicao, BlocoMemoria *qqCache);
int getEndBlocoCache(int posicao, BlocoMemoria *qqCache);
int getCacheHitMemoria(BlocoMemoria *aux);
time_t getSegundosCache(int posicao, BlocoMemoria *qqCache);
bool isAtualizadoCache(int posicao, BlocoMemoria *qqCache);

//Funcoes
void menu();
void menuPrintar();
void alocaMemorias();
void desalocaMemorias();
void montarRAM();
void montarCacheVazio(int tamanho, BlocoMemoria* qqCache);
void montarInstrucoesProgramaAleatorio();
void montarInstrucaoGerador(FILE *arquivo);
void maquina();
BlocoMemoria *buscarNasMemorias(Endereco *end);
int algoritmoSubstituicaoLRU(int tamanhoCache);
void guardarCaches(int posicaoCache, int tamanhoCache, Endereco *endAux);
void apagarCaches(int posicaoCacheX, int posicaoCacheY, int tamanhoCache);
void montarCachePrintar(int tamanho, BlocoMemoria *qqCache);

int main(){
   FILE *arquivo = fopen("instrucoes.txt", "r");
    alocaMemorias();
    montarRAM();
    montarCacheVazio(tamanhoCache1, cache1);
    montarCacheVazio(tamanhoCache2, cache2);
    montarCacheVazio(tamanhoCache3, cache3);

    int opcao;

    do{
        menu();

        scanf("%d", &opcao);
		getchar();
		switch (opcao){
            case 1:
                montarInstrucoesProgramaAleatorio();
                maquina();
                break;
            
            case 2:
                montarInstrucaoGerador(arquivo);
                maquina();
                break;
        }

    }while (opcao != 0);

    desalocaMemorias();
    fclose(arquivo);
    return 0;
}

void menuPrintar(){
    printf("\n========================== MENU =======================\n\n");
    printf("Printar Cache1 - 1\n");
    printf("Printar Cache2 - 2\n");
    printf("Printar Cache3 - 3\n");
    printf("Sair - Digite 0\n");
    printf("\n=======================================================\n\n");
	printf("Digite um numero: ");
}

void menu(){
	printf("\n========================== MENU =======================\n\n");
	printf("Maquina com Mapeamento Direto Aleatorio - Digite 1\n");
	printf("Maquina com Mapeamento Direto Arquivo - Digite 2\n");
	printf("Sair - Digite 0\n");
	printf("\n=======================================================\n\n");
	printf("Digite um numero: ");
}

void alocaMemorias(){
    RAM = malloc(tamanhoRAM * sizeof(BlocoMemoria));
    memoriaInstrucoes = malloc(tamanhoPrograma * sizeof(Instrucao));
    cache1 = malloc(tamanhoCache1 * sizeof(BlocoMemoria));
    cache2 = malloc(tamanhoCache2 * sizeof(BlocoMemoria));
    cache3 = malloc(tamanhoCache3 * sizeof(BlocoMemoria));
}

void desalocaMemorias(){
    free(RAM);
    free(cache1);
    free(cache2);
    free(cache3);
    free(memoriaInstrucoes); 
}

void montarRAM()
{
    srand(time(NULL));
    int r;
    BlocoMemoria *aux = NULL;
    int *palavrasAux = NULL;

    for (int i = 0; i < tamanhoRAM; i++)
    {
        aux = malloc(sizeof(BlocoMemoria));

        palavrasAux = malloc(qdePalavrasBloco * sizeof(int));

        for(int j = 0; j < qdePalavrasBloco; j++){
            r = rand() % 1000000; 
            palavrasAux[j] = r;
        }

        setPalavrasMemoria(aux, palavrasAux);
        setEndBlocoMemoria(aux, i);
    
        RAM[i] = *aux;
    }
}

void montarCacheVazio(int tamanho, BlocoMemoria *qqCache){
    time_t segundoEntradoCache;
    time(&segundoEntradoCache);
    segundoEntradoCache = segundoEntradoCache - 10;

    for(int i = 0; i < tamanho; i++){
        BlocoMemoria *aux = malloc(sizeof(BlocoMemoria));

        //Forcando cache estar vazia
        setEndBlocoMemoria(aux, INT_MIN);
        setSegundosMemoria(aux, segundoEntradoCache);

        qqCache[i] = *aux;
    }
}

void montarCachePrintar(int tamanho, BlocoMemoria *qqCache){
    for(int i = 0; i < tamanho; i++){
        printf("\n==================== Cache =====================\n\n");
        printf("EndBloco: %d\n", qqCache[i].endBloco);
        printf("Atualizado: %d\n", qqCache[i].atualizado);
        printf("Custo: %d\n", qqCache[i].custo);
        printf("CacheHit: %d\n", qqCache[i].cacheHit);
        struct tm *data;
        data = localtime(&qqCache[i].segundos);

        printf("Segundos: %d:%d:%d\n", data->tm_hour, data->tm_min, data->tm_sec);
        printf("\n=================================================\n\n");
    }
}

void montarInstrucoesProgramaAleatorio(){
    Instrucao *umaInstrucao = NULL;

    srand(time(NULL));
    int r;

    for(int i = 0; i < tamanhoPrograma -1; i++){
        umaInstrucao = malloc(sizeof(Instrucao));

        r = rand() % 3;
        setOpcodeInstrucao(umaInstrucao, r);

        Endereco *add1 = malloc(sizeof(Endereco));
        r = rand() % tamanhoRAM;
        add1->endBloco = r;
        umaInstrucao->add1.endBloco = r;
        r = rand() % qdePalavrasBloco;
        add1->endPalavra = r;
        setAdd1Instrucao(umaInstrucao, add1);
        
        Endereco *add2 = malloc(sizeof(Endereco));
        r = rand() % tamanhoRAM;
        add2->endBloco = r;
        r = rand() % qdePalavrasBloco;
        add2->endPalavra = r;
        setAdd2Instrucao(umaInstrucao, add2);

        Endereco *add3 = malloc(sizeof(Endereco));
        r = rand() % tamanhoRAM;
        add3->endBloco = r;
        r = rand() % qdePalavrasBloco;
        add3->endPalavra = r;
        setAdd3Instrucao(umaInstrucao, add3);

        memoriaInstrucoes[i] = *umaInstrucao;
    }

    umaInstrucao = malloc(sizeof(Instrucao));
    setOpcodeInstrucao(umaInstrucao, -1);

    memoriaInstrucoes[tamanhoPrograma - 1] = *umaInstrucao;
}

void montarInstrucaoGerador(FILE *arquivo){
    int opcodeAux, endBlocoAux1, endPalavraAux1, endBlocoAux2, endPalavraAux2, endBlocoAux3, endPalavraAux3;

    Instrucao *umaInstrucao = NULL;

	for(int i = 0; i < tamanhoPrograma -1; i++){
		umaInstrucao = malloc(sizeof(Instrucao));

        fscanf(arquivo, "%d %d %d %d %d %d %d\n", &opcodeAux, &endBlocoAux1, &endPalavraAux1, &endBlocoAux2, &endPalavraAux2, &endBlocoAux3, &endPalavraAux3);
        
        setOpcodeInstrucao(umaInstrucao, opcodeAux);

        Endereco *add1 = malloc(sizeof(Endereco));
        add1->endBloco = endBlocoAux1;
        add1->endPalavra = endPalavraAux1;
        setAdd1Instrucao(umaInstrucao, add1);

        Endereco *add2 = malloc(sizeof(Endereco));
        add2->endBloco = endBlocoAux2;
        add2->endPalavra = endPalavraAux2;
        setAdd2Instrucao(umaInstrucao, add2);

        Endereco *add3 = malloc(sizeof(Endereco));
        add3->endBloco = endBlocoAux3;
        add3->endPalavra = endPalavraAux3;
        setAdd3Instrucao(umaInstrucao, add3);

        memoriaInstrucoes[i] = *umaInstrucao;
	}

    umaInstrucao = malloc(sizeof(Instrucao));
    setOpcodeInstrucao(umaInstrucao, -1);

    memoriaInstrucoes[tamanhoPrograma - 1] = *umaInstrucao;
}

void maquina(){
    //Registradores
    int PC = 0;
    int opcode = INT_MAX;
    int custo = 0;
    
    //caches 1 e 2 Hit e miss
    int missC1 = 0;
    int hitC1 = 0;
    int missC2 = 0;
    int hitC2 = 0;
    int missC3 = 0;
    int hitC3 = 0;
    int conteudo1, conteudo2, sub, soma;
    
    while(opcode != -1){
        Instrucao *umaInstrucao = malloc(sizeof(Instrucao));
        *umaInstrucao = memoriaInstrucoes[PC];
        opcode = getOpcodeInstrucao(umaInstrucao);
        
        //TP2
        if(opcode != -1){        
            BlocoMemoria *dadoMemoriaAdd1 = malloc(sizeof(BlocoMemoria));
            dadoMemoriaAdd1 = buscarNasMemorias(getAdd1Instrucao(umaInstrucao));
            
            BlocoMemoria *dadoMemoriaAdd2 = malloc(sizeof(BlocoMemoria));
            dadoMemoriaAdd2 = buscarNasMemorias(getAdd2Instrucao(umaInstrucao));
            
            BlocoMemoria *dadoMemoriaAdd3 = malloc(sizeof(BlocoMemoria));
            dadoMemoriaAdd3 = buscarNasMemorias(getAdd3Instrucao(umaInstrucao));

            //incremento custos
            custo += getCustoMemoria(dadoMemoriaAdd1);
            custo += getCustoMemoria(dadoMemoriaAdd2);
            custo += getCustoMemoria(dadoMemoriaAdd3);

            //Validando hits e misses
            if(getCacheHitMemoria(dadoMemoriaAdd1) == 1){
                hitC1++;
            }
            else if(getCacheHitMemoria(dadoMemoriaAdd1) == 2){
                missC1++;
				hitC2++;
            }else if(getCacheHitMemoria(dadoMemoriaAdd1) == 3){
                missC1++;
				missC2++;
                hitC3++;	
            }else if(getCacheHitMemoria(dadoMemoriaAdd1) == 4){
                missC1++;
				missC2++;
                missC3++;	
            }

            if(getCacheHitMemoria(dadoMemoriaAdd2) == 1){
                hitC1++;
            }
            else if(getCacheHitMemoria(dadoMemoriaAdd2) == 2){
                missC1++;
			    hitC2++;
            }else if(getCacheHitMemoria(dadoMemoriaAdd2) == 3){
                missC1++;
				missC2++;
                hitC3++;	
            }else if(getCacheHitMemoria(dadoMemoriaAdd2) == 4){
                missC1++;
				missC2++;
                missC3++;	
            }

            if(getCacheHitMemoria(dadoMemoriaAdd3) == 1){
                hitC1++;
            }
            else if(getCacheHitMemoria(dadoMemoriaAdd3) == 2){
                missC1++;
				hitC2++;
            }else if(getCacheHitMemoria(dadoMemoriaAdd3) == 3){
                missC1++;
				missC2++;
                hitC3++;	
            }else if(getCacheHitMemoria(dadoMemoriaAdd3) == 4){
                missC1++;
				missC2++;
                missC3++;	
            }

            printf("Custo ate o momento do programa em execucao: %d\n", custo);
            printf("Hits e Misses ate o momento - C1 hit | C1 miss | C2 hit | C2 miss | C3 hit | C3 miss: %d | %d | %d | %d | %d | %d\n", hitC1 , missC1, hitC2, missC2, hitC3, missC3);
            printf("===================== %d =====================\n", PC);

            switch (opcode)
            {   
                //Levar para cache1 dados externos
                case 0:
                    printf("Nao ha demanda por levar dados externos para as memorias\n");
                    break;
                
                case 1:
                    conteudo1 = getPalavrasMemoria(dadoMemoriaAdd1)[umaInstrucao->add1.endPalavra];
                    conteudo2 = getPalavrasMemoria(dadoMemoriaAdd2)[umaInstrucao->add2.endPalavra];
                    soma = conteudo1 + conteudo2;

                    getPalavrasMemoria(dadoMemoriaAdd3)[umaInstrucao->add2.endPalavra] = soma;

                    setAtualizadoMemoria(dadoMemoriaAdd3);

                    printf("Somando %d\n", soma);                    
                    break;

                case 2:
                    conteudo1 = getPalavrasMemoria(dadoMemoriaAdd1)[umaInstrucao->add1.endPalavra];
                    conteudo2 = getPalavrasMemoria(dadoMemoriaAdd2)[umaInstrucao->add2.endPalavra];
                    sub = conteudo1 - conteudo2;

                    getPalavrasMemoria(dadoMemoriaAdd3)[umaInstrucao->add2.endPalavra] = sub;

                    setAtualizadoMemoria(dadoMemoriaAdd3);

                    printf("Subtraindo %d\n", sub);
                    break;
            }

            PC++;
        }
    }

    float somaHit = hitC1 + hitC2 + hitC3;
    float somaMiss = missC1 + missC2 + missC3;
    float MissHit = somaHit + somaMiss;

    float C1 = hitC1 * 100;
    float total1 = C1/MissHit;

    float C2 = hitC2 * 100;
    float total2 = C2/MissHit;
    
    float C3 = hitC3 * 100;
    float total3 = C3/MissHit;
    
    float RAM4 = somaMiss * 100;
    float total4 = RAM4/MissHit;

    printf("Custo total do programa: %d\n", custo);
    printf("Hits e Misses do programa - C1 hit | C1 miss | C2 hit | C2 miss | C3 hit | C3 miss: %d | %d | %d | %d | %d | %d\n", hitC1 , missC1, hitC2, missC2, hitC3, missC3);
    printf("\n================================================================\n\n");
    printf("CUSTO C1: %.2f\n", total1);
    printf("CUSTO C2: %.2f\n", total2);
    printf("CUSTO C3: %.2f\n", total3);
    printf("RAM: %.2f\n", total4);
}

BlocoMemoria* buscarNasMemorias(Endereco* end){
    int custo = 0;
    int cacheHit = 0;
    time_t momentoAdicionadoCache;
    time(&momentoAdicionadoCache);

    for(int i = 0; i < tamanhoCache1; i++){
        if(getEndBlocoCache(i, cache1) == getEndBlocoEndereco(end)){
            custo = 10;
            cacheHit = 1;

            setCustoCache(custo, i, cache1);
            setCacheHitCache(cacheHit, i, cache1);
            setSegundosCache(momentoAdicionadoCache, i, cache1);

            return &cache1[i];
        }

        
    }

    for(int i = 0; i < tamanhoCache2; i++){
        if(getEndBlocoCache(i, cache2) == getEndBlocoEndereco(end)){
            custo = 110;
            cacheHit = 2;

            setCustoCache(custo, i, cache2);
            setCacheHitCache(cacheHit, i, cache2);
            setSegundosCache(momentoAdicionadoCache, i, cache2);

            return &cache2[i];
        }
    }

    for(int i = 0; i < tamanhoCache3; i++){
        if(getEndBlocoCache(i, cache3) == getEndBlocoEndereco(end)){
            custo = 1110;
            cacheHit = 3;

            setCustoCache(custo, i, cache3);
            setCacheHitCache(cacheHit, i, cache3);
            setSegundosCache(momentoAdicionadoCache, i, cache3);

            return &cache3[i];
        }
    }

    for(int i = 0; i < tamanhoCache3; i++){
        if(!isAtualizadoCache(i, cache3)){
            custo = 11110;
            cacheHit = 4;
            cache3[i] = RAM[getEndBlocoEndereco(end)];

            setCustoCache(custo, i, cache3);
            setCacheHitCache(cacheHit, i, cache3);
            setSegundosCache(momentoAdicionadoCache, i, cache3);

            int posicaoCache2 = algoritmoSubstituicaoLRU(tamanhoCache2);
            guardarCaches(posicaoCache2, tamanhoCache2, end);

            apagarCaches(posicaoCache2, i, tamanhoCache2);
            cache2[posicaoCache2] = cache3[i];

            return &cache3[i];
        }
    }

    for(int i = 0; i < tamanhoCache2; i++){
        if(!isAtualizadoCache(i, cache2)){
            custo = 11110;
            cacheHit = 4;
            cache2[i] = RAM[getEndBlocoEndereco(end)];

            setCustoCache(custo, i, cache2);
            setCacheHitCache(cacheHit, i, cache2);
            setSegundosCache(momentoAdicionadoCache, i, cache2);

            int posicaoCache1 = algoritmoSubstituicaoLRU(tamanhoCache1);
            guardarCaches(posicaoCache1, tamanhoCache1, end);
            
            apagarCaches(posicaoCache1, i, tamanhoCache2);
            cache1[posicaoCache1] = cache2[i];

            return &cache2[i];
        }
    }

    custo = 11100;
    cacheHit = 4;

    int posicaoCache3 = algoritmoSubstituicaoLRU(tamanhoCache3);
    guardarCaches(posicaoCache3, tamanhoCache3, end);

    cache3[posicaoCache3] = RAM[getEndBlocoEndereco(end)];
    setCustoCache(custo, posicaoCache3, cache3);
    setCacheHitCache(cacheHit, posicaoCache3, cache3);
    setSegundosCache(momentoAdicionadoCache, posicaoCache3, cache3);

    int posicaoCache2 = algoritmoSubstituicaoLRU(tamanhoCache2);
    guardarCaches(posicaoCache2, tamanhoCache2, end);

    apagarCaches(posicaoCache2, posicaoCache3, tamanhoCache3);
    cache2[posicaoCache2] = cache3[posicaoCache3];
    
    return &cache2[posicaoCache2];
}

void guardarCaches(int posicaoCache, int tamanhoCache, Endereco *endAux){
    if(tamanhoCache == tamanhoCache1){
        int posicaoCache2 = algoritmoSubstituicaoLRU(tamanhoCache2);
        int posicaoCache3 = algoritmoSubstituicaoLRU(tamanhoCache3);
        
        if(getEndBlocoCache(posicaoCache3, cache3) == getEndBlocoEndereco(endAux)){
            setDesatualizadoCache(posicaoCache3, cache3);
            RAM[getEndBlocoCache(posicaoCache3, cache3)] = cache3[posicaoCache3];
            cache3[posicaoCache3] = cache2[posicaoCache2];
            cache2[posicaoCache2] = cache1[posicaoCache];
        }
    }else if(tamanhoCache == tamanhoCache2){
        int posicaoCache3 = algoritmoSubstituicaoLRU(tamanhoCache3);
        if(getEndBlocoCache(posicaoCache3, cache3) == getEndBlocoEndereco(endAux)){
            setDesatualizadoCache(posicaoCache3, cache3);
            RAM[getEndBlocoCache(posicaoCache3, cache3)] = cache3[posicaoCache3];
            cache3[posicaoCache3] = cache2[posicaoCache];
        }
    }else if(tamanhoCache == tamanhoCache3){
        if(getEndBlocoCache(tamanhoCache, cache3) == getEndBlocoEndereco(endAux)){
            setDesatualizadoCache(tamanhoCache, cache3);
            RAM[getEndBlocoCache(posicaoCache, cache3)] = cache3[posicaoCache];
        }
    }
}

void apagarCaches(int posicaoCacheX, int posicaoCacheY, int tamanhoCache){
    time_t tempoMaisVelha;
    time(&tempoMaisVelha);
    tempoMaisVelha = tempoMaisVelha - 2100;

    if(tamanhoCache == tamanhoCache1){
        if(getEndBlocoCache(posicaoCacheX, cache1) == getEndBlocoCache(posicaoCacheY, cache2)){
            setDesatualizadoCache(posicaoCacheY, cache2);
            setSegundosCache(tempoMaisVelha, posicaoCacheY, cache2);
        }    
    }else if(tamanhoCache == tamanhoCache2){
        if(getEndBlocoCache(posicaoCacheX, cache2) == getEndBlocoCache(posicaoCacheY, cache3)){
            setDesatualizadoCache(posicaoCacheY, cache3);
            setSegundosCache(tempoMaisVelha, posicaoCacheY, cache2);
        }    
    }
}

int algoritmoSubstituicaoLRU(int tamanhoCache){
    time_t posicaoMaisVelha;
    time(&posicaoMaisVelha);

    int posicaoCacheAux = 0;

    if(tamanhoCache == tamanhoCache1){
        for(int i = 0; i < tamanhoCache1; i++){
            if(cache1[i].segundos <= posicaoMaisVelha){
                posicaoMaisVelha = cache1[i].segundos;
                posicaoCacheAux = i;
            }
        }
    }else if(tamanhoCache == tamanhoCache2){
        for(int i = 0; i < tamanhoCache2; i++){
            if(cache2[i].segundos <= posicaoMaisVelha){
                posicaoMaisVelha = cache2[i].segundos;
                posicaoCacheAux = i;
            }
        }
    }else if(tamanhoCache == tamanhoCache3){
        for(int i = 0; i < tamanhoCache3; i++){
            if(cache3[i].segundos <= posicaoMaisVelha){
                posicaoMaisVelha = cache3[i].segundos;
                posicaoCacheAux = i;
            }
        }
    }

    return posicaoCacheAux;
}

//BlocoMemoria
void setCustoMemoria(BlocoMemoria *aux, int custoAux)
{
    aux->custo = custoAux;
}

void setEndBlocoMemoria(BlocoMemoria *aux, int endBlocoAux)
{
    aux->endBloco = endBlocoAux;
}

void setPalavrasMemoria(BlocoMemoria *aux, int* palavrasAux)
{
    aux->palavras = palavrasAux;
}

void setCacheHitMemoria(BlocoMemoria *aux, int cacheHitAux)
{
    aux->cacheHit = cacheHitAux;
}

void setAtualizadoMemoria1(BlocoMemoria *aux){
    aux->atualizado = false;
}

void setAtualizadoMemoria(BlocoMemoria *aux)
{
    aux->atualizado = true;
}

void setDesatualizadoMemoria(BlocoMemoria *aux)
{
    aux->atualizado = false;
}

void setSegundosMemoria(BlocoMemoria *aux, int segundosAux)
{
    aux->segundos = segundosAux;
}

int getCustoMemoria(BlocoMemoria *aux)
{
    return aux->custo;
}

int getCacheHitMemoria(BlocoMemoria *aux)
{
    return aux->cacheHit;
}

int* getPalavrasMemoria(BlocoMemoria *aux)
{
    return aux->palavras;
}

int getEndBlocoMemoria(BlocoMemoria *aux)
{
    return aux->endBloco;
}

bool isAtualizadoMemoria(BlocoMemoria *aux)
{
    return aux->atualizado;
}

//Instrucao
void setOpcodeInstrucao(Instrucao *umaInstrucao, int opcodeAux)
{
    umaInstrucao->opcode = opcodeAux;
}

void setAdd1Instrucao(Instrucao *umaInstrucao, Endereco *add1Aux)
{
    umaInstrucao->add1 = *add1Aux;
}

void setAdd2Instrucao(Instrucao *umaInstrucao, Endereco *add2Aux)
{
    umaInstrucao->add2 = *add2Aux;
}

void setAdd3Instrucao(Instrucao *umaInstrucao, Endereco *add3Aux)
{
    umaInstrucao->add3 = *add3Aux;
}

Endereco* getAdd1Instrucao(Instrucao *umaInstrucao)
{
    return &umaInstrucao->add1;
}

Endereco* getAdd2Instrucao(Instrucao *umaInstrucao)
{
    return &umaInstrucao->add2;
}

Endereco* getAdd3Instrucao(Instrucao *umaInstrucao)
{
    return &umaInstrucao->add3;
}

int getOpcodeInstrucao(Instrucao *umaInstrucao){
    return umaInstrucao->opcode;
}

//Endereco
void setEndBlocoEndereco(Endereco *end, int endBlocoAux)
{
    end->endBloco = endBlocoAux;
}

void setEndPalavraEndereco(Endereco *end, int endPalavraAux)
{
    end->endPalavra = endPalavraAux;
}

int getEndBlocoEndereco(Endereco *end)
{
    return end->endBloco;
}

int getEndPalavraEndereco(Endereco *end)
{
    return end->endPalavra;
}

//Cache --- MMU
void setEndBlocoCache(int endBlocoAux, int posicao, BlocoMemoria *qqCache){
    qqCache[posicao].endBloco = endBlocoAux;
}

void setCustoCache(int custoAux, int posicao, BlocoMemoria *qqCache)
{
    qqCache[posicao].custo = custoAux;
}

void setSegundosCache(int segundoAux, int posicao, BlocoMemoria *qqCache)
{
    qqCache[posicao].segundos = segundoAux;
}

void setCacheHitCache(int cacheHitAux, int posicao, BlocoMemoria *qqCache)
{
    qqCache[posicao].cacheHit = cacheHitAux;
}

void setAtualizadoCache(int posicao, BlocoMemoria *qqCache)
{
    qqCache[posicao].atualizado = true;
}

void setDesatualizadoCache(int posicao, BlocoMemoria *qqCache){
    qqCache[posicao].atualizado = false;
}

int getEndBlocoCache(int posicao, BlocoMemoria *qqCache){
    return qqCache[posicao].endBloco;
}

time_t getSegundosCache(int posicao, BlocoMemoria *qqCache){
    return qqCache[posicao].segundos;
}

bool isAtualizadoCache(int posicao, BlocoMemoria *qqCache)
{
    return qqCache[posicao].atualizado;
}
