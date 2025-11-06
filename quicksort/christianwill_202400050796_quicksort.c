#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <time.h> 

typedef struct {
    char nome_variante[5]; 
    long contagem_total;   
    int ordem_estavel;     
} Resultado;

// --- Funções Auxiliares (Swap, Pivos) ---
// (Sem alterações)

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int pivo_mediana3(int* V, int ini, int fim) {
    int n = fim - ini + 1;
    if (n <= 0) return V[ini]; 
    int idx1 = ini + n / 4;
    int idx2 = ini + n / 2;
    int idx3 = ini + (3 * n) / 4;
    if (idx1 < ini) idx1 = ini;
    if (idx2 < ini) idx2 = ini;
    if (idx3 > fim) idx3 = fim;
    int v1 = V[idx1], v2 = V[idx2], v3 = V[idx3];
    if ((v1 >= v2 && v1 <= v3) || (v1 <= v2 && v1 >= v3)) return v1;
    if ((v2 >= v1 && v2 <= v3) || (v2 <= v1 && v2 >= v3)) return v2;
    return v3;
}

int pivo_aleatorio(int* V, int ini, int fim) {
    int n = fim - ini + 1;
    if (n <= 0) return V[ini];
    int idx = ini + (abs(V[ini]) % n);
    if (idx > fim || idx < ini) idx = ini; 
    return V[idx];
}

// --- Funções de Partição ---

int particiona_lomuto(int* V, int ini, int fim, long* trocas, int pivo_val) {
    // (Sem alterações)
    int pivo_idx = fim; 
    if (pivo_val != V[fim]) {
        for (int i = ini; i < fim; i++) {
            if (V[i] == pivo_val) {
                pivo_idx = i;
                break;
            }
        }
        if (pivo_idx != fim) {
            swap(&V[pivo_idx], &V[fim]);
            (*trocas)++;
        }
    }
    int pivo = V[fim];
    int i = ini - 1;
    for (int j = ini; j < fim; j++) {
        if (V[j] <= pivo) {
            i++;
            swap(&V[i], &V[j]);
            (*trocas)++;
        }
    }
    swap(&V[i + 1], &V[fim]);
    (*trocas)++;
    return i + 1;
}

// --- CORREÇÃO ESTÁ AQUI ---
int particiona_hoare(int* V, int ini, int fim, long* trocas, int pivo) {
    int i = ini - 1;
    int j = fim + 1;
    while (1) {
        // Verifica se 'i' ultrapassa o limite 'fim'
        do { 
            i++; 
        } while (i <= fim && V[i] < pivo); // Adicionado: i <= fim
        
        // Verifica se 'j' ultrapassa o limite 'ini'
        do { 
            j--; 
        } while (j >= ini && V[j] > pivo); // Adicionado: j >= ini
        
        if (i >= j) return j;
        
        swap(&V[i], &V[j]);
        (*trocas)++;
    }
}
// --- FIM DA CORREÇÃO ---


// --- 6 Variantes do Quicksort (Para os números) ---
// (Sem alterações)

void quicksort_lp(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo_idx = particiona_lomuto(V, ini, fim, trocas, V[fim]); 
        quicksort_lp(V, ini, pivo_idx - 1, trocas, chamadas);
        quicksort_lp(V, pivo_idx + 1, fim, trocas, chamadas);
    }
}

void quicksort_hp(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo = V[ini + (fim - ini) / 2]; 
        int q = particiona_hoare(V, ini, fim, trocas, pivo);
        quicksort_hp(V, ini, q, trocas, chamadas);
        quicksort_hp(V, q + 1, fim, trocas, chamadas);
    }
}

void quicksort_lm(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo = pivo_mediana3(V, ini, fim);
        int pivo_idx = particiona_lomuto(V, ini, fim, trocas, pivo);
        quicksort_lm(V, ini, pivo_idx - 1, trocas, chamadas);
        quicksort_lm(V, pivo_idx + 1, fim, trocas, chamadas);
    }
}

void quicksort_hm(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo = pivo_mediana3(V, ini, fim);
        int q = particiona_hoare(V, ini, fim, trocas, pivo);
        quicksort_hm(V, ini, q, trocas, chamadas);
        quicksort_hm(V, q + 1, fim, trocas, chamadas);
    }
}

void quicksort_ha(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo = pivo_aleatorio(V, ini, fim);
        int q = particiona_hoare(V, ini, fim, trocas, pivo);
        quicksort_ha(V, ini, q, trocas, chamadas);
        quicksort_ha(V, q + 1, fim, trocas, chamadas);
    }
}

void quicksort_la(int* V, int ini, int fim, long* trocas, long* chamadas) {
    (*chamadas)++;
    if (ini < fim) {
        int pivo = pivo_aleatorio(V, ini, fim);
        int pivo_idx = particiona_lomuto(V, ini, fim, trocas, pivo);
        quicksort_la(V, ini, pivo_idx - 1, trocas, chamadas);
        quicksort_la(V, pivo_idx + 1, fim, trocas, chamadas);
    }
}

// --- Funções Quicksort (Para os 6 Resultados) ---
// (Sem alterações)

void swapResultados(Resultado* a, Resultado* b) {
    Resultado t = *a;
    *a = *b;
    *b = t;
}

void quicksort_resultados(Resultado* arr, int ini, int fim) {
    if (ini < fim) {
        long pivo_val = arr[fim].contagem_total;
        int pivo_ordem = arr[fim].ordem_estavel;
        int i = ini - 1;
        for (int j = ini; j < fim; j++) {
            if (arr[j].contagem_total < pivo_val) {
                i++;
                swapResultados(&arr[i], &arr[j]);
            } 
            else if (arr[j].contagem_total == pivo_val && arr[j].ordem_estavel < pivo_ordem) {
                i++;
                swapResultados(&arr[i], &arr[j]);
            }
        }
        swapResultados(&arr[i + 1], &arr[fim]);
        int pivo_idx = i + 1;
        quicksort_resultados(arr, ini, pivo_idx - 1);
        quicksort_resultados(arr, pivo_idx + 1, fim);
    }
}

// --- Função Principal ---
// (Sem alterações, pois as checagens de segurança já estavam na v2)

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada.txt> <arquivo_de_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char* input_file_name = argv[1];
    const char* output_file_name = argv[2]; 

    FILE* in_f = fopen(input_file_name, "r");
    if (in_f == NULL) {
        perror("Nao foi possivel abrir o arquivo de entrada");
        return EXIT_FAILURE;
    }

    FILE* out_f = fopen(output_file_name, "w");
    if (out_f == NULL) {
        perror("Nao foi possivel abrir o arquivo de saida");
        fclose(in_f); 
        return EXIT_FAILURE;
    }

    int n_vetores;
    
    if (fscanf(in_f, "%d", &n_vetores) != 1) {
        fprintf(stderr, "Erro: Arquivo de entrada esta vazio ou mal formatado.\n");
        fclose(in_f);
        fclose(out_f);
        return EXIT_FAILURE; 
    }

    for (int i = 0; i < n_vetores; i++) {
        int N;
        
        if (fscanf(in_f, "%d", &N) != 1) {
            fprintf(stderr, "Erro: Nao foi possivel ler o tamanho N do vetor %d.\n", i);
            break; 
        }
        
        if (N <= 0) {
            fprintf(stderr, "Erro: Tamanho N invalido (%d) para o vetor %d.\n", N, i);
            // Imprime uma linha vazia para o resultado, se N=0
            if (N == 0) {
                 fprintf(out_f, "[0]:LP(0),HP(0),LM(0),HM(0),HA(0),LA(0)\n");
            }
            continue; 
        }

        int* vetor = malloc(N * sizeof(int));
        if (vetor == NULL) {
            fprintf(stderr, "Erro de alocacao para o vetor (N=%d).\n", N);
            continue; 
        }
        
        int elementos_lidos = 0;
        for (int j = 0; j < N; j++) {
            if (fscanf(in_f, "%d", &vetor[j]) != 1) {
                 fprintf(stderr, "Erro: Falha ao ler elementos do vetor %d (esperava %d, leu %d).\n", i, N, j);
                 N = j; // Ajusta N para o número de elementos lidos
                 break; 
            }
            elementos_lidos = j + 1;
        }

        // Se N foi ajustado para 0 (ou menos) por falha na leitura, pula
        if (N <= 0) {
            fprintf(stderr, "Erro: Nenhum elemento lido para o vetor %d.\n", i);
            free(vetor);
            if (N == 0) fprintf(out_f, "[0]:LP(0),HP(0),LM(0),HM(0),HA(0),LA(0)\n");
            continue;
        }


        int* copia = malloc(N * sizeof(int));
        if (copia == NULL) {
            fprintf(stderr, "Erro de alocacao para a copia (N=%d).\n", N);
            free(vetor);
            continue;
        }

        Resultado resultados[6]; 
        long trocas, chamadas;

        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_lp(copia, 0, N - 1, &trocas, &chamadas);
        resultados[0] = (Resultado){"LP", trocas + chamadas, 0}; 

        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_hp(copia, 0, N - 1, &trocas, &chamadas);
        resultados[1] = (Resultado){"HP", trocas + chamadas, 1}; 

        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_lm(copia, 0, N - 1, &trocas, &chamadas);
        resultados[2] = (Resultado){"LM", trocas + chamadas, 2};

        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_hm(copia, 0, N - 1, &trocas, &chamadas);
        resultados[3] = (Resultado){"HM", trocas + chamadas, 3};

        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_ha(copia, 0, N - 1, &trocas, &chamadas);
        resultados[4] = (Resultado){"HA", trocas + chamadas, 4}; 
        
        trocas = 0; chamadas = 0;
        memcpy(copia, vetor, N * sizeof(int));
        quicksort_la(copia, 0, N - 1, &trocas, &chamadas);
        resultados[5] = (Resultado){"LA", trocas + chamadas, 5}; 

        quicksort_resultados(resultados, 0, 5); 
        
        fprintf(out_f, "[%d]:", N);
        for (int j = 0; j < 6; j++) {
            fprintf(out_f, "%s(%ld)", resultados[j].nome_variante, resultados[j].contagem_total);
            if (j < 5) {
                fprintf(out_f, ",");
            }
        }
        fprintf(out_f, "\n");

        free(vetor);
        free(copia);
    }

    fclose(in_f);
    fclose(out_f); 
    
    printf("Relatorio de quicksort lido de '%s' e salvo em: '%s'\n", input_file_name, output_file_name);
    
    return EXIT_SUCCESS;
}