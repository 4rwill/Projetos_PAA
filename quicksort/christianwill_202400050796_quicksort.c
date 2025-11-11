#include <stdio.h>
#include <stdlib.h> // Para malloc, free, abs, qsort, exit
#include <string.h> // Para strcmp, memcpy

// Estrutura para contar trocas e chamadas
typedef struct {
    long long swaps;
    long long calls;
} Stats;

// Estrutura para armazenar o resultado final de cada algoritmo
typedef struct {
    char name[4]; // "LP", "HP", "LM", "HM", "LA", "HA"
    long long total_count;
} Result;

// --- Funções Auxiliares ----------------------------------------------------

// Função de troca com contagem
void swap(int* a, int* b, Stats* stats) {
    int temp = *a;
    *a = *b;
    *b = temp;
    stats->swaps++;
}

// Encontra o índice da mediana de três elementos
// (Conforme a especificação: n/4, n/2, 3n/4 do subarray atual)
int get_median_idx(int arr[], int ini, int fim) {
    int len = fim - ini + 1;
    if (len < 3) return ini; // Caso base, apenas retorna o início

    int i1 = ini + len / 4;
    int i2 = ini + len / 2;
    int i3 = ini + (3 * len) / 4;

    int v1 = arr[i1];
    int v2 = arr[i2];
    int v3 = arr[i3];

    // Lógica para encontrar o índice do valor mediano
    if ((v1 - v2) * (v3 - v1) >= 0) return i1; // v1 é mediana
    if ((v2 - v1) * (v3 - v2) >= 0) return i2; // v2 é mediana
    return i3; // v3 é mediana
}

// Encontra o índice "aleatório" (determinístico)
// (Conforme a especificação: V[ini + |V[ini]| mod n])
int get_random_idx(int arr[], int ini, int fim) {
    int len = fim - ini + 1;
    if (len <= 0) return ini;
    // abs() está em stdlib.h
    int pivot_idx = ini + (abs(arr[ini]) % len);
    return pivot_idx;
}

// --- Partições Lomuto ------------------------------------------------------

// Partição Lomuto Padrão (pivô é o último elemento)
int partition_lomuto_standard(int arr[], int ini, int fim, Stats* stats) {
    int pivo = arr[fim];
    int i = ini - 1;
    for (int j = ini; j < fim; j++) {
        if (arr[j] < pivo) {
            i++;
            swap(&arr[i], &arr[j], stats);
        }
    }
    swap(&arr[i + 1], &arr[fim], stats);
    return i + 1;
}

// Partição Lomuto com pivô pré-definido (move para o fim)
int partition_lomuto(int arr[], int ini, int fim, int pivot_idx, Stats* stats) {
    swap(&arr[pivot_idx], &arr[fim], stats); // Move pivô para o fim
    return partition_lomuto_standard(arr, ini, fim, stats);
}

// --- Partições Hoare -------------------------------------------------------

// Partição Hoare Padrão (pivô é o elemento do meio)
int partition_hoare_standard(int arr[], int ini, int fim, Stats* stats) {
    int pivo = arr[(ini + fim) / 2];
    int i = ini - 1;
    int j = fim + 1;
    while (1) {
        do { i++; } while (arr[i] < pivo);
        do { j--; } while (arr[j] > pivo);
        if (i >= j) return j;
        swap(&arr[i], &arr[j], stats);
    }
}

// Partição Hoare com valor do pivô pré-definido
int partition_hoare(int arr[], int ini, int fim, int pivo_val, Stats* stats) {
    int i = ini - 1;
    int j = fim + 1;
    while (1) {
        do { i++; } while (arr[i] < pivo_val);
        do { j--; } while (arr[j] > pivo_val);
        if (i >= j) return j;
        swap(&arr[i], &arr[j], stats);
    }
}


// --- 1. Lomuto Padrão (LP) -------------------------------------------------
void quicksort_LP(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++; // Conta a chamada
    if (ini < fim) {
        int p_idx = partition_lomuto_standard(arr, ini, fim, stats);
        quicksort_LP(arr, ini, p_idx - 1, stats);
        quicksort_LP(arr, p_idx + 1, fim, stats);
    }
}

// --- 2. Lomuto Mediana-3 (LM) ----------------------------------------------
void quicksort_LM(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++;
    if (ini < fim) {
        int pivot_idx = get_median_idx(arr, ini, fim);
        int p_idx = partition_lomuto(arr, ini, fim, pivot_idx, stats);
        quicksort_LM(arr, ini, p_idx - 1, stats);
        quicksort_LM(arr, p_idx + 1, fim, stats);
    }
}

// --- 3. Lomuto Aleatório (LA) ----------------------------------------------
void quicksort_LA(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++;
    if (ini < fim) {
        int pivot_idx = get_random_idx(arr, ini, fim);
        int p_idx = partition_lomuto(arr, ini, fim, pivot_idx, stats);
        quicksort_LA(arr, ini, p_idx - 1, stats);
        quicksort_LA(arr, p_idx + 1, fim, stats);
    }
}

// --- 4. Hoare Padrão (HP) --------------------------------------------------
void quicksort_HP(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++;
    if (ini < fim) {
        int p_idx = partition_hoare_standard(arr, ini, fim, stats);
        quicksort_HP(arr, ini, p_idx, stats);
        quicksort_HP(arr, p_idx + 1, fim, stats);
    }
}

// --- 5. Hoare Mediana-3 (HM) -----------------------------------------------
void quicksort_HM(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++;
    if (ini < fim) {
        int pivot_idx = get_median_idx(arr, ini, fim);
        int pivo_val = arr[pivot_idx];
        int p_idx = partition_hoare(arr, ini, fim, pivo_val, stats);
        quicksort_HM(arr, ini, p_idx, stats);
        quicksort_HM(arr, p_idx + 1, fim, stats);
    }
}

// --- 6. Hoare Aleatório (HA) -----------------------------------------------
void quicksort_HA(int arr[], int ini, int fim, Stats* stats) {
    stats->calls++;
    if (ini < fim) {
        int pivot_idx = get_random_idx(arr, ini, fim);
        int pivo_val = arr[pivot_idx];
        int p_idx = partition_hoare(arr, ini, fim, pivo_val, stats);
        quicksort_HA(arr, ini, p_idx, stats);
        quicksort_HA(arr, p_idx + 1, fim, stats);
    }
}

// --- Função de Comparação para qsort ---------------------------------------

int compare_results(const void* a, const void* b) {
    Result* resA = (Result*)a;
    Result* resB = (Result*)b;

    // Ordena primariamente pelo total_count
    if (resA->total_count < resB->total_count) return -1;
    if (resA->total_count > resB->total_count) return 1;

    // Se houver empate, ordena alfabeticamente pelo nome (ordenação estável)
    return strcmp(resA->name, resB->name);
}

// --- Função Principal (MODIFICADA) ------------------------------------------

/**
 * @brief Programa principal para comparar variantes do Quicksort.
 * * @param argc Número de argumentos da linha de comando.
 * @param argv Vetor de argumentos da linha de comando.
 * argv[0] é o nome do programa.
 * argv[1] é o nome do arquivo de entrada.
 * argv[2] é o nome do arquivo de saída.
 * @return int 0 em caso de sucesso, 1 em caso de erro.
 */
int main(int argc, char *argv[]) {
    // --- 1. Verificação de Argumentos ---
    if (argc != 3) {
        printf("Erro de uso!\n");
        printf("Formato correto: %s <arquivo_de_entrada> <arquivo_de_saida>\n", argv[0]);
        printf("Exemplo: %s entrada.txt saida.txt\n", argv[0]);
        return 1; // Retorna erro
    }

    // --- 2. Armazena os nomes dos arquivos ---
    char* input_filename = argv[1];
    char* output_filename = argv[2];

    // --- 3. Abre os arquivos usando os nomes de argv ---
    FILE *fin = fopen(input_filename, "r");
    FILE *fout = fopen(output_filename, "w");

    // Mensagens de erro atualizadas
    if (fin == NULL) {
        printf("Erro ao abrir arquivo de entrada: %s\n", input_filename);
        return 1;
    }
    if (fout == NULL) {
        printf("Erro ao criar arquivo de saida: %s\n", output_filename);
        fclose(fin);
        return 1;
    }

    int n_vectors;
    fscanf(fin, "%d", &n_vectors);

    // Processa cada vetor
    for (int i = 0; i < n_vectors; i++) {
        int N; // Tamanho do vetor atual
        fscanf(fin, "%d", &N);

        // Aloca o array original e um de cópia
        int* master_array = (int*)malloc(N * sizeof(int));
        int* arr_copy = (int*)malloc(N * sizeof(int));

        if (master_array == NULL || arr_copy == NULL) {
            printf("Erro de alocacao de memoria para N=%d\n", N);
            fclose(fin);
            fclose(fout);
            return 1;
        }

        // Lê os elementos do vetor
        for (int j = 0; j < N; j++) {
            fscanf(fin, "%d", &master_array[j]);
        }

        // Armazena os 6 resultados
        Result results[6];
        
        // Define as funções de ordenação e nomes
        void (*sort_functions[6])(int[], int, int, Stats*) = {
            quicksort_LP, quicksort_HP, quicksort_LM, 
            quicksort_HM, quicksort_LA, quicksort_HA
        };
        const char* names[6] = {"LP", "HP", "LM", "HM", "LA", "HA"};

        // Executa os 6 algoritmos
        for (int k = 0; k < 6; k++) {
            // Reseta estatísticas e copia o array
            Stats current_stats = {0, 0};
            memcpy(arr_copy, master_array, N * sizeof(int));

            // Roda o quicksort
            sort_functions[k](arr_copy, 0, N - 1, &current_stats);

            // Salva o resultado
            strcpy(results[k].name, names[k]);
            results[k].total_count = current_stats.swaps + current_stats.calls;
        }

        // Ordena os resultados
        qsort(results, 6, sizeof(Result), compare_results);

        // Imprime no arquivo de saída
        fprintf(fout, "[%d]:", N);
        for (int k = 0; k < 6; k++) {
            fprintf(fout, "%s(%lld)", results[k].name, results[k].total_count);
            if (k < 5) {
                fprintf(fout, ",");
            }
        }
        fprintf(fout, "\n");

        // Libera memória
        free(master_array);
        free(arr_copy);
    }

    fclose(fin);
    fclose(fout);

    return 0;
}