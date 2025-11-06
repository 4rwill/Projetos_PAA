#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>

// --- Estrutura de Entrada 1 ---
typedef struct {
    char codigo[12];
    char cnpj[20]; 
    int peso;
    int ordem_cadastro;
} Container;

// --- Estrutura de Entrada 2 (NOVA) ---
// Precisamos armazenar todos os 'm' itens antes de processar
typedef struct {
    char codigo[12];
    char cnpj[20];
    int peso;
} Selecionado;

// --- Estrutura de Saída ---
typedef struct {
    char codigo[12];
    int tipo_divergencia;
    int ordem_cadastro;
    double diff_percentual;
    int diff_percentual_arredondado;
    char cnpj_registrado[20];
    char cnpj_selecionado[20];
    int peso_registrado;
    int peso_selecionado;
} Inspecao;


// --- Início: Merge Sort para 'Container' (por código) ---
// (Necessário para ordenar o array 'registrados')

int containerDeveVimAntes(const Container *ia, const Container *ib) {
    // Ordena A-Z pelo código
    return strcmp(ia->codigo, ib->codigo) < 0;
}

void merge_Container(Container* arr, Container* temp, int esq, int meio, int dir) {
    for (int i = esq; i <= dir; i++) temp[i] = arr[i];
    int i = esq;
    int j = meio + 1;
    for (int k = esq; k <= dir; k++) {
        if (i > meio) arr[k] = temp[j++];
        else if (j > dir) arr[k] = temp[i++];
        else if (containerDeveVimAntes(&temp[i], &temp[j])) arr[k] = temp[i++];
        else arr[k] = temp[j++];
    }
}

void mergeSort_Container(Container* arr, Container* temp, int esq, int dir) {
    if (dir <= esq) return;
    int meio = esq + (dir - esq) / 2;
    mergeSort_Container(arr, temp, esq, meio);
    mergeSort_Container(arr, temp, meio + 1, dir);
    merge_Container(arr, temp, esq, meio, dir);
}
// --- Fim: Merge Sort para 'Container' ---


// --- Início: Merge Sort para 'Selecionado' (por código) ---
// (Necessário para ordenar o array 'selecionados')

int selecionadoDeveVimAntes(const Selecionado *ia, const Selecionado *ib) {
    // Ordena A-Z pelo código
    return strcmp(ia->codigo, ib->codigo) < 0;
}

void merge_Selecionado(Selecionado* arr, Selecionado* temp, int esq, int meio, int dir) {
    for (int i = esq; i <= dir; i++) temp[i] = arr[i];
    int i = esq;
    int j = meio + 1;
    for (int k = esq; k <= dir; k++) {
        if (i > meio) arr[k] = temp[j++];
        else if (j > dir) arr[k] = temp[i++];
        else if (selecionadoDeveVimAntes(&temp[i], &temp[j])) arr[k] = temp[i++];
        else arr[k] = temp[j++];
    }
}

void mergeSort_Selecionado(Selecionado* arr, Selecionado* temp, int esq, int dir) {
    if (dir <= esq) return;
    int meio = esq + (dir - esq) / 2;
    mergeSort_Selecionado(arr, temp, esq, meio);
    mergeSort_Selecionado(arr, temp, meio + 1, dir);
    merge_Selecionado(arr, temp, esq, meio, dir);
}
// --- Fim: Merge Sort para 'Selecionado' ---


// --- Início: Merge Sort para 'Inspecao' (Seu código original) ---
// (Usado para a ordenação final da saída)

int deveVimAntes(const Inspecao *ia, const Inspecao *ib) {
    if (ia->tipo_divergencia < ib->tipo_divergencia) return 1;
    if (ia->tipo_divergencia > ib->tipo_divergencia) return 0;
    if (ia->tipo_divergencia == 1)
        return ia->ordem_cadastro < ib->ordem_cadastro;
    if (ia->diff_percentual_arredondado > ib->diff_percentual_arredondado) return 1;
    if (ia->diff_percentual_arredondado < ib->diff_percentual_arredondado) return 0;
    return ia->ordem_cadastro < ib->ordem_cadastro;
}

void merge(Inspecao* arr, Inspecao* temp, int esq, int meio, int dir) {
    for (int i = esq; i <= dir; i++) temp[i] = arr[i];
    int i = esq;
    int j = meio + 1;
    for (int k = esq; k <= dir; k++) {
        if (i > meio) arr[k] = temp[j++];
        else if (j > dir) arr[k] = temp[i++];
        else if (deveVimAntes(&temp[i], &temp[j])) arr[k] = temp[i++];
        else arr[k] = temp[j++];
    }
}

void mergeSort(Inspecao* arr, Inspecao* temp, int esq, int dir) {
    if (dir <= esq) return;
    int meio = esq + (dir - esq) / 2;
    mergeSort(arr, temp, esq, meio);
    mergeSort(arr, temp, meio + 1, dir);
    merge(arr, temp, esq, meio, dir);
}
// --- Fim: Merge Sort para 'Inspecao' ---


// --- Função auxiliar para processar uma discrepância ---
void processarDivergencia(Container reg, Selecionado sel, Inspecao* para_inspecionar, int* contador) {
    Inspecao item;
    strcpy(item.codigo, reg.codigo);
    item.ordem_cadastro = reg.ordem_cadastro;

    if (strcmp(sel.cnpj, reg.cnpj) != 0) {
        item.tipo_divergencia = 1;
        strcpy(item.cnpj_registrado, reg.cnpj);
        strcpy(item.cnpj_selecionado, sel.cnpj);
        para_inspecionar[(*contador)++] = item;
    } else {
        int diff_abs_kg = abs(sel.peso - reg.peso);
        double diff_perc = ((double)diff_abs_kg / reg.peso) * 100.0;
        if (diff_perc > 10.0) {
            item.tipo_divergencia = 2;
            item.peso_registrado = reg.peso;
            item.peso_selecionado = sel.peso;
            item.diff_percentual = diff_perc;
            item.diff_percentual_arredondado = (int)round(diff_perc);
            para_inspecionar[(*contador)++] = item;
        }
    }
}


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

    // --- ETAPA 1: LER TODOS OS 'N' REGISTRADOS ---
    int n;
    fscanf(in_f, "%d", &n);
    Container *registrados = malloc(n * sizeof(Container));
    if (registrados == NULL) {
        fclose(in_f); 
        return EXIT_FAILURE;
    }
    for (int i = 0; i < n; i++) {
        fscanf(in_f, "%s %s %d", registrados[i].codigo, registrados[i].cnpj, &registrados[i].peso);
        registrados[i].ordem_cadastro = i;
    }

    // --- ETAPA 2: LER TODOS OS 'M' SELECIONADOS ---
    int m;
    fscanf(in_f, "%d", &m);
    Selecionado *selecionados = malloc(m * sizeof(Selecionado));
    if (selecionados == NULL) {
        free(registrados);
        fclose(in_f);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < m; i++) {
        fscanf(in_f, "%s %s %d", selecionados[i].codigo, selecionados[i].cnpj, &selecionados[i].peso);
    }
    
    fclose(in_f); // Terminamos de ler o arquivo de entrada

    // --- ETAPA 3: ORDENAR OS DOIS ARRAYS ---
    
    // Ordenar 'registrados' (Custo: O(n log n))
    Container *temp_reg = malloc(n * sizeof(Container));
    if (temp_reg == NULL) { /*... tratar erro ...*/ }
    mergeSort_Container(registrados, temp_reg, 0, n - 1);
    free(temp_reg);

    // Ordenar 'selecionados' (Custo: O(m log m))
    Selecionado *temp_sel = malloc(m * sizeof(Selecionado));
    if (temp_sel == NULL) { /*... tratar erro ...*/ }
    mergeSort_Selecionado(selecionados, temp_sel, 0, m - 1);
    free(temp_sel);


    // --- ETAPA 4: PROCESSAR (MERGE-JOIN) ---
    // (Custo: O(n + m))
    
    // Alocar espaço para o pior caso (todas as 'm' inspeções são discrepâncias)
    Inspecao *para_inspecionar = malloc(m * sizeof(Inspecao));
    if (para_inspecionar == NULL) {
        free(registrados);
        free(selecionados);
        return EXIT_FAILURE;
    }

    int contador_inspecao = 0;
    int i = 0; // Ponteiro para 'registrados'
    int j = 0; // Ponteiro para 'selecionados'

    while (i < n && j < m) {
        int cmp = strcmp(registrados[i].codigo, selecionados[j].codigo);
        
        if (cmp < 0) {
            // O item 'registrados[i]' é menor, não tem par em 'selecionados'.
            i++;
        } else if (cmp > 0) {
            // O item 'selecionados[j]' é menor, não tem par em 'registrados'.
            j++;
        } else {
            // ENCONTRAMOS! Os códigos são iguais.
            // O item 'registrados[i]' pode corresponder a múltiplos 'selecionados[j]'
            
            Container reg = registrados[i]; // Guarda o container atual
            
            // Itera por todos os 'selecionados' que têm o mesmo código
            while (j < m && strcmp(reg.codigo, selecionados[j].codigo) == 0) {
                processarDivergencia(reg, selecionados[j], para_inspecionar, &contador_inspecao);
                j++; // Move para o próximo 'selecionado'
            }
            
            // Já processamos todos os 'selecionados' para este 'registrados[i]'
            i++; // Move para o próximo 'registrado'
        }
    }


    // --- ETAPA 5: ORDENAR A SAÍDA ---
    // (Custo: O(k log k), onde k = contador_inspecao)

    Inspecao *temp_inspecao = malloc(contador_inspecao * sizeof(Inspecao));
    if (temp_inspecao == NULL) {
        /*... tratar erro ...*/
    }
    
    // Usa o seu mergeSort original com a lógica de ordenação complexa
    mergeSort(para_inspecionar, temp_inspecao, 0, contador_inspecao - 1);
    free(temp_inspecao);


    // --- ETAPA 6: ESCREVER O RESULTADO ---

    FILE* out_f = fopen(output_file_name, "w");
    if (out_f == NULL) {
        perror("Nao foi possivel abrir o arquivo de saida");
        free(registrados);
        free(selecionados);
        free(para_inspecionar);
        return EXIT_FAILURE;
    }

    for (int k = 0; k < contador_inspecao; k++) {
        Inspecao item = para_inspecionar[k];
        if (item.tipo_divergencia == 1)
            fprintf(out_f, "%s:%s<->%s\n", item.codigo, item.cnpj_registrado, item.cnpj_selecionado);
        else {
            int diff_kg = abs(item.peso_selecionado - item.peso_registrado);
            fprintf(out_f, "%s:%dkg(%.0f%%)\n", item.codigo, diff_kg, item.diff_percentual);
        }
    }

    fclose(out_f);
    
    // --- ETAPA 7: LIBERAR MEMÓRIA ---
    free(registrados);
    free(selecionados);
    free(para_inspecionar);
    
    return EXIT_SUCCESS;
}