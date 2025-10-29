#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>

typedef struct {
    char codigo[12];
    char cnpj[20]; 
    int peso;
    int ordem_cadastro;
} Container;

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

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_saida.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* output_file_name = argv[1];

    int n;
    scanf("%d", &n);
    Container *registrados = malloc(n * sizeof(Container));
    if (registrados == NULL) return EXIT_FAILURE;

    for (int i = 0; i < n; i++) {
        scanf("%s %s %d", registrados[i].codigo, registrados[i].cnpj, &registrados[i].peso);
        registrados[i].ordem_cadastro = i;
    }

    int m;
    scanf("%d", &m);
    Inspecao *para_inspecionar = malloc(m * sizeof(Inspecao));
    if (para_inspecionar == NULL) {
        free(registrados);
        return EXIT_FAILURE;
    }

    int contador_inspecao = 0;
    for (int i = 0; i < m; i++) {
        char codigo_sel[12];
        char cnpj_sel[20];
        int peso_sel;
        scanf("%s %s %d", codigo_sel, cnpj_sel, &peso_sel);

        for (int j = 0; j < n; j++) {
            if (strcmp(codigo_sel, registrados[j].codigo) == 0) {
                Container reg = registrados[j];
                Inspecao item;
                strcpy(item.codigo, reg.codigo);
                item.ordem_cadastro = reg.ordem_cadastro;

                if (strcmp(cnpj_sel, reg.cnpj) != 0) {
                    item.tipo_divergencia = 1;
                    strcpy(item.cnpj_registrado, reg.cnpj);
                    strcpy(item.cnpj_selecionado, cnpj_sel);
                    para_inspecionar[contador_inspecao++] = item;
                } else {
                    int diff_abs_kg = abs(peso_sel - reg.peso);
                    double diff_perc = ((double)diff_abs_kg / reg.peso) * 100.0;
                    if (diff_perc > 10.0) {
                        item.tipo_divergencia = 2;
                        item.peso_registrado = reg.peso;
                        item.peso_selecionado = peso_sel;
                        item.diff_percentual = diff_perc;
                        item.diff_percentual_arredondado = (int)round(diff_perc);
                        para_inspecionar[contador_inspecao++] = item;
                    }
                }
                break;
            }
        }
    }

    Inspecao *temp = malloc(contador_inspecao * sizeof(Inspecao));
    if (temp == NULL) {
        fprintf(stderr, "Erro de alocação de memória para o array temp.\n");
        free(registrados);
        free(para_inspecionar);
        return EXIT_FAILURE;
    }

    mergeSort(para_inspecionar, temp, 0, contador_inspecao - 1);

    FILE* out_f = fopen(output_file_name, "w");
    if (out_f == NULL) {
        perror("Nao foi possivel abrir o arquivo de saida");
        free(registrados);
        free(para_inspecionar);
        free(temp);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < contador_inspecao; i++) {
        Inspecao item = para_inspecionar[i];
        if (item.tipo_divergencia == 1)
            fprintf(out_f, "%s:%s<->%s\n", item.codigo, item.cnpj_registrado, item.cnpj_selecionado);
        else {
            int diff_kg = abs(item.peso_selecionado - item.peso_registrado);
            fprintf(out_f, "%s:%dkg(%.0f%%)\n", item.codigo, diff_kg, item.diff_percentual);
        }
    }

    fclose(out_f);
    printf("Relatorio de inspecao salvo em: %s\n", output_file_name);

    free(registrados);
    free(para_inspecionar);
    free(temp);
    
    return EXIT_SUCCESS;
}