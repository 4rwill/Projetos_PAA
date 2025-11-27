#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POS(i, w, v) ((size_t)(i) * (largura_W) * (largura_V) + (size_t)(w) * (largura_V) + (size_t)(v))

typedef struct {
    char placa[20];
    int cap_peso;
    int cap_vol;
} Veiculo;

typedef struct {
    int id;
    char codigo[50];
    double valor;
    int peso;
    int volume;
    int status; 
} Pacote;

void processarCarga(Veiculo *veiculos, int n_veiculos, Pacote *pacotes, int n_pacotes, FILE *saida);

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* in_f = fopen(argv[1], "r");
    if (!in_f) { perror("Erro entrada"); return EXIT_FAILURE; }

    FILE* out_f = fopen(argv[2], "w");
    if (!out_f) { perror("Erro saida"); fclose(in_f); return EXIT_FAILURE; }

    int n_veiculos;
    fscanf(in_f, "%d", &n_veiculos);
    Veiculo *veiculos = (Veiculo *)malloc(n_veiculos * sizeof(Veiculo));
    for (int i = 0; i < n_veiculos; i++) {
        fscanf(in_f, "%s %d %d", veiculos[i].placa, &veiculos[i].cap_peso, &veiculos[i].cap_vol);
    }

    int n_pacotes;
    fscanf(in_f, "%d", &n_pacotes);
    Pacote *pacotes = (Pacote *)malloc(n_pacotes * sizeof(Pacote));
    for (int i = 0; i < n_pacotes; i++) {
        fscanf(in_f, "%s %lf %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].id = i;
        pacotes[i].status = 0;
    }
    fclose(in_f);

    processarCarga(veiculos, n_veiculos, pacotes, n_pacotes, out_f);

    fclose(out_f); 
    free(veiculos);
    free(pacotes);

    return EXIT_SUCCESS;
}

void processarCarga(Veiculo *veiculos, int n_veiculos, Pacote *pacotes, int n_pacotes, FILE *saida) {
    
    int max_W = 0;
    int max_V = 0;
    for(int k=0; k<n_veiculos; k++){
        if(veiculos[k].cap_peso > max_W) max_W = veiculos[k].cap_peso;
        if(veiculos[k].cap_vol > max_V)  max_V = veiculos[k].cap_vol;
    }

    size_t largura_W = max_W + 1;
    size_t largura_V = max_V + 1;

    double *dp = (double *)malloc(largura_W * largura_V * sizeof(double));

    char *escolha = (char *)malloc((size_t)n_pacotes * largura_W * largura_V * sizeof(char));

    if (!dp || !escolha) {
        fprintf(stderr, "Erro fatal: Memoria insuficiente para as dimensoes do problema.\n");
        exit(1);
    }

    int *indices_levados = (int*)malloc(n_pacotes * sizeof(int));

    for (int k = 0; k < n_veiculos; k++) {
        Veiculo *v_atual = &veiculos[k];
        int W = v_atual->cap_peso;
        int V = v_atual->cap_vol;

        memset(dp, 0, largura_W * largura_V * sizeof(double));
        memset(escolha, 0, (size_t)n_pacotes * largura_W * largura_V * sizeof(char));

        for (int i = 0; i < n_pacotes; i++) {
            if (pacotes[i].status == 1) continue; 

            int p = pacotes[i].peso;
            int vol = pacotes[i].volume;
            double val = pacotes[i].valor;

            for (int w = W; w >= p; w--) {
                size_t offset_w_v = (size_t)w * largura_V; 
                size_t offset_prev_w_v = (size_t)(w - p) * largura_V;

                for (int v = V; v >= vol; v--) {
                    
                    double valor_sem_item = dp[offset_w_v + v];
                    double valor_com_item = dp[offset_prev_w_v + (v - vol)] + val;

                    if (valor_com_item > valor_sem_item) {
                        dp[offset_w_v + v] = valor_com_item;
                        
                        escolha[POS(i, w, v)] = 1;
                    }
                }
            }
        }
        
        double valor_total = dp[W * largura_V + V];

        int w_rest = W;
        int v_rest = V;
        int peso_total = 0;
        int vol_total = 0;
        int qtd = 0;

        for (int i = n_pacotes - 1; i >= 0; i--) {
            if (escolha[POS(i, w_rest, v_rest)] == 1) {
                if (pacotes[i].status == 0) {
                    pacotes[i].status = 1; 
                    indices_levados[qtd++] = i;
                    
                    peso_total += pacotes[i].peso;
                    vol_total += pacotes[i].volume;
                    w_rest -= pacotes[i].peso;
                    v_rest -= pacotes[i].volume;
                }
            }
        }

        double p_peso = (W > 0) ? ((double)peso_total / W * 100.0) : 0;
        double p_vol = (V > 0) ? ((double)vol_total / V * 100.0) : 0;

        fprintf(saida, "[%s]R$%.2f,%dKG(%.0f%%),%dL(%.0f%%)->", 
               v_atual->placa, valor_total, 
               peso_total, p_peso,
               vol_total, p_vol);

        for (int i = qtd - 1; i >= 0; i--) {
            fprintf(saida, "%s%s", pacotes[indices_levados[i]].codigo, (i > 0) ? "," : "");
        }
        fprintf(saida, "\n");
    }

    double pen_valor = 0;
    int pen_peso = 0;
    int pen_vol = 0;
    int tem_pendente = 0;

    for(int j = 0; j < n_pacotes; j++) {
        if(pacotes[j].status == 0) {
            pen_valor += pacotes[j].valor;
            pen_peso += pacotes[j].peso;
            pen_vol += pacotes[j].volume;
            tem_pendente = 1;
        }
    }

    if (tem_pendente) {
        fprintf(saida, "PENDENTE:R$%.2f,%dKG,%dL->", pen_valor, pen_peso, pen_vol);
        int primeiro = 1;
        for(int j = 0; j < n_pacotes; j++) {
            if(pacotes[j].status == 0) {
                if(!primeiro) fprintf(saida, ",");
                fprintf(saida, "%s", pacotes[j].codigo);
                primeiro = 0;
            }
        }
        fprintf(saida, "\n");
    } else {
        fprintf(saida, "PENDENTE:R$0.00,0KG,0L->\n");
    }

    free(dp);
    free(escolha);
    free(indices_levados);
}