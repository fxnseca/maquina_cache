
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>

typedef struct{
    int *palavras;
    int endBloco;
    bool atualizado;
    int custo;
    int cacheHit;
    time_t segundos;
} BlocoMemoria;

typedef struct{
    int endBloco;
    int endPalavra;
} Endereco;

typedef struct{
    Endereco add1;
    Endereco add2;
    Endereco add3;
    int opcode;
} Instrucao;

int tamanhoRAM = 1000;
int qdePalavrasBloco = 4;
int tamanhoCache1 = 16;
int tamanhoCache2 = 32;
int tamanhoCache3 = 64;
int tamanhoPrograma = 10002;

Instrucao *memoriaInstrucoes;
BlocoMemoria *cache1;
BlocoMemoria *cache2;
BlocoMemoria *cache3;
BlocoMemoria *RAM;

//BlocoMemoria
void setCustoMemoria(BlocoMemoria *aux, int custoAux);
void setEndBlocoMemoria(BlocoMemoria *aux, int endBlocoAux);
void setPalavrasMemoria(BlocoMemoria *aux, int * palavrasAux);
void setCacheHitMemoria(BlocoMemoria *aux, int cacheHitAux);
void setSegundosMemoria(BlocoMemoria *aux, int segundosAux);
void setAtualizadoMemoria1(BlocoMemoria *aux);
void setAtualizadoMemoria(BlocoMemoria *aux);
int getCustoMemoria(BlocoMemoria *aux);
int* getPalavrasMemoria(BlocoMemoria *aux);
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
void alocaMemorias();
void desalocaMemorias();
void montarRAM();
void montarCacheVazio(int tamanho, BlocoMemoria* qqCache);
void montarInstrucoesProgramaAleatorio();
void montarInstrucaoGerador(FILE *arquivo);
void maquina();
BlocoMemoria *buscarNasMemorias(Endereco *end);
BlocoMemoria *testaCache1Cache2(int posicaoCache1, int posicaoCache2, int posicaoCache3, int custo, int cacheHit);
int algoritmoSubstituicaoLRU(int tamanhoCache);

int main(){
    FILE *arquivo = fopen("instrucoes.txt", "r");

    int opcao;
    do{
        alocaMemorias();
        montarRAM();
        montarCacheVazio(tamanhoCache1, cache1);
        montarCacheVazio(tamanhoCache2, cache2);
        montarCacheVazio(tamanhoCache3, cache3);

        menu();

        scanf("%d", &opcao);
		getchar();
		switch (opcao){
            case 1:
                montarInstrucoesProgramaAleatorio();
                maquina();
                desalocaMemorias();
                fclose(arquivo);
                break;
            
            case 2:
                montarInstrucaoGerador(arquivo);
                maquina();
                desalocaMemorias();
                fclose(arquivo);
                break;
        }

    }while (opcao != 0);

    desalocaMemorias();
    fclose(arquivo);
    return 0;
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

            printf("VALOR = %d\n", getCacheHitMemoria(dadoMemoriaAdd1));
            printf("VALOR = %d\n", getCacheHitMemoria(dadoMemoriaAdd2));
            printf("VALOR = %d\n", getCacheHitMemoria(dadoMemoriaAdd3));

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
            printf("%d\n", PC);

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

    int somaHit = hitC1 + hitC2 + hitC3;
    int somaMiss = missC1 + missC2 + missC3;
    printf("Custo total do programa: %d\n", custo);
    printf("Hits e Misses do programa - C1 hit | C1 miss | C2 hit | C2 miss | C3 hit | C3 miss: %d | %d | %d | %d | %d | %d\n", hitC1 , missC1, hitC2, missC2, hitC3, missC3);
    printf("Total de hits: %d\n", somaHit);
    printf("Total de miss(RAM): %d\n", somaMiss);
    printf("Total Hits e miss: %d\n", somaHit + somaMiss);
}

BlocoMemoria *buscarNasMemorias(Endereco *end){
    int posicaoCache1 = getEndBlocoEndereco(end) % tamanhoCache1;
    int posicaoCache2 = getEndBlocoEndereco(end) % tamanhoCache2;
    int posicaoCache3 = getEndBlocoEndereco(end) % tamanhoCache3;
    int custo = 0;
    int cacheHit = 0;

    BlocoMemoria *aux = malloc(sizeof(BlocoMemoria));

    if(getEndBlocoCache(posicaoCache1, cache1) == getEndBlocoEndereco(end)){
        custo = 10;
        cacheHit = 1;

        setCustoCache(custo, posicaoCache1, cache1);
        setCacheHitCache(cacheHit, posicaoCache1, cache1);

        printf("CACHE 1\n");
        return &cache1[posicaoCache1];
    }
    else{
        if(getEndBlocoCache(posicaoCache2, cache2) == getEndBlocoEndereco(end)){
            custo = 110;
            cacheHit = 2;
            
            printf("CACHE 2\n");

            return testaCache1Cache2(posicaoCache1, posicaoCache2, posicaoCache3, custo, cacheHit);
        }
        else{
            if(getEndBlocoCache(posicaoCache3, cache3) == getEndBlocoEndereco(end)){
                custo = 1110;
                cacheHit = 3;

                printf("CACHE 3\n");

                return testaCache1Cache2(posicaoCache1, posicaoCache2, posicaoCache3, custo, cacheHit);

                return &cache3[posicaoCache3];
            }
            else{
                //buscar na RAM e o repassa a cache2 e depois a cache1
                custo = 11100;
                cacheHit = 4;

                if(!isAtualizadoCache(posicaoCache3, cache3)){
                    cache3[posicaoCache3] = RAM[getEndBlocoEndereco(end)];
                    setCacheHitCache(4, posicaoCache3, cache3);

                    printf("RAM\n");

                    return testaCache1Cache2(posicaoCache1, posicaoCache2, posicaoCache3, custo, cacheHit);
                }
                else{
                    if(!isAtualizadoCache(posicaoCache2, cache2)){
                        cache2[posicaoCache2] = RAM[getEndBlocoEndereco(end)];
                        setCacheHitCache(4, posicaoCache2, cache2);

                        printf("RAM\n");

                        return testaCache1Cache2(posicaoCache1, posicaoCache2, posicaoCache3, custo, cacheHit);
                    }
                    else{
                        setDesatualizadoCache(posicaoCache3, cache3);
                        RAM[getEndBlocoCache(posicaoCache3, cache3)] = cache3[posicaoCache3];

                        cache3[posicaoCache3] = RAM[getEndBlocoEndereco(end)];
                        setCacheHitCache(4, posicaoCache3, cache3);

                        printf("RAM\n");

                        return testaCache1Cache2(posicaoCache1, posicaoCache2, posicaoCache3, custo, cacheHit);
                    }
                }
            }
        }
    }
}

BlocoMemoria *testaCache1Cache2(int posicaoCache1, int posicaoCache2, int posicaoCache3, int custo, int cacheHit){
    BlocoMemoria *auxZero = malloc(sizeof(BlocoMemoria));
    auxZero->atualizado = false;
    auxZero->cacheHit = 0;
    auxZero->custo = 0;
    auxZero->endBloco = INT_MIN;

    if(!isAtualizadoCache(posicaoCache2, cache2)){
        if(cache2[posicaoCache2].endBloco == cache3[posicaoCache3].endBloco){
            *auxZero = cache3[posicaoCache3];
            cache2[posicaoCache2] = cache3[posicaoCache3];
            cache3[posicaoCache3] = *auxZero;
        }

        cache2[posicaoCache2] = cache3[posicaoCache3];

        setCacheHitCache(cacheHit, posicaoCache2, cache2);
        setCustoCache(custo, posicaoCache2, cache2);

        return &cache2[posicaoCache2];
    }else{
        if(!isAtualizadoCache(posicaoCache1, cache1)){
            if(cache2[posicaoCache2].endBloco == cache3[posicaoCache3].endBloco){
                *auxZero = cache2[posicaoCache2];
                cache1[posicaoCache1] = cache2[posicaoCache2];
                cache2[posicaoCache2] = *auxZero;
            }

            cache1[posicaoCache1] = cache2[posicaoCache2];

            setCacheHitCache(cacheHit, posicaoCache1, cache1);
            setCustoCache(custo, posicaoCache1, cache1);



            return &cache1[posicaoCache1];
        }
        else{
            BlocoMemoria *aux = malloc(sizeof(BlocoMemoria));
            *aux = cache3[posicaoCache3];
            cache2[posicaoCache2] = cache3[posicaoCache3];
            cache3[posicaoCache2] = *aux;
        }
    }

    setCustoCache(custo, posicaoCache2, cache2);
    setCacheHitCache(cacheHit, posicaoCache2, cache2);

    return &cache2[posicaoCache2];
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