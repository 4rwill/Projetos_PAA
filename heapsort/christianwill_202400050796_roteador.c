#include <stdio.h>
#include <stdlib.h>
/* string.h não é necessário para este problema */

/* * Estrutura para armazenar um pacote.
 * Os dados são lidos como bytes hexadecimais (0-255), 
 * então 'unsigned char' é o tipo de dado perfeito.
 */
typedef struct {
    int prioridade;
    int tamanho;
    unsigned char dados[512]; /* Tamanho máximo de 512 bytes */
} Pacote;

/* --- Início: Funções do Heapsort (para Pacotes) --- */

/* Função auxiliar para trocar dois pacotes */
void swap_pacote(Pacote* a, Pacote* b) {
    Pacote temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Função heapify (min-heap)
 * * Para ordenar em ordem DECRESCENTE (63, 32, 15...), 
 * nós construímos uma Min-Heap (baseada na prioridade). 
 * * Quando extraímos o menor (ex: 0) e o colocamos no fim do array,
 * o resultado final é um array ordenado do maior para o menor.
 */
void heapify_desc(Pacote* arr, int n, int i) {
    int menor = i;       /* Nó raiz */
    int esq = 2 * i + 1; /* Filho da esquerda */
    int dir = 2 * i + 2; /* Filho da direita */

    /* Se o filho da esquerda for menor que a raiz */
    if (esq < n && arr[esq].prioridade < arr[menor].prioridade) {
        menor = esq;
    }

    /* Se o filho da direita for menor que o 'menor' atual */
    if (dir < n && arr[dir].prioridade < arr[menor].prioridade) {
        menor = dir;
    }

    /* Se o 'menor' não for mais a raiz original */
    if (menor != i) {
        swap_pacote(&arr[i], &arr[menor]);
        /* Continua a 'afundar' o nó recursivamente */
        heapify_desc(arr, n, menor);
    }
}

/* Função principal do Heapsort para ordenação descendente */
void heapSort_desc(Pacote* arr, int n) {
    /* 1. Construir a Min-Heap (reorganizar o array) */
    /* Começa do último nó não-folha e vai subindo */
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify_desc(arr, n, i);
    }

    /* 2. Extrair elementos um por um */
    for (int i = n - 1; i > 0; i--) {
        /* Move a raiz atual (o menor) para o fim */
        swap_pacote(&arr[0], &arr[i]);

        /* Chama heapify no heap reduzido (tamanho 'i') na raiz (índice 0) */
        heapify_desc(arr, i, 0);
    }
}

/* --- Fim: Funções do Heapsort --- */


/**
 * Função auxiliar para imprimir um lote de pacotes
 * no formato de saída especificado.
 */
void imprimir_batch(FILE* out_f, Pacote* buffer, int num_pacotes) {
    if (num_pacotes == 0) return;

    for (int i = 0; i < num_pacotes; i++) {
        fprintf(out_f, "|");
        for (int j = 0; j < buffer[i].tamanho; j++) {
            /* * %02X imprime um 'unsigned char' como hexadecimal
             * com 2 dígitos, preenchendo com zero à esquerda (ex: 0A, 0F, 03)
             */
            fprintf(out_f, "%02X", buffer[i].dados[j]);
            
            if (j < buffer[i].tamanho - 1) {
                fprintf(out_f, ",");
            }
        }
    }
    fprintf(out_f, "|\n");
}


/* --- Função Principal --- */

int main(int argc, char* argv[]) {

    /* 1. Verificação de argumentos e abertura de arquivos */
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

    /* 2. Ler cabeçalho: N total de pacotes e Capacidade do Roteador */
    int n_total_pacotes;
    int capacidade_maxima;

    if (fscanf(in_f, "%d %d", &n_total_pacotes, &capacidade_maxima) != 2) {
        fprintf(stderr, "Erro: Arquivo de entrada vazio ou mal formatado.\n");
        fclose(in_f);
        fclose(out_f);
        return EXIT_FAILURE;
    }
    
    if (n_total_pacotes <= 0) {
        /* Não há o que processar */
        fclose(in_f);
        fclose(out_f);
        return EXIT_SUCCESS;
    }

    /* 3. Alocar o "buffer do roteador" (pior caso: todos os pacotes cabem) */
    /* (Poderíamos alocar um buffer menor, mas 'n' é o mais seguro) */
    Pacote* buffer_roteador = malloc(n_total_pacotes * sizeof(Pacote));
    if (buffer_roteador == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memoria para o buffer.\n");
        fclose(in_f);
        fclose(out_f);
        return EXIT_FAILURE;
    }

    int pacotes_no_buffer = 0;
    int bytes_no_buffer = 0;

    /* 4. Loop principal: ler todos os pacotes do arquivo */
    for (int i = 0; i < n_total_pacotes; i++) {
        
        Pacote pacote_atual;
        
        /* Lê a prioridade e o tamanho */
        if (fscanf(in_f, "%d %d", &pacote_atual.prioridade, &pacote_atual.tamanho) != 2) {
             fprintf(stderr, "Erro: Falha ao ler o pacote %d.\n", i);
             break;
        }

        /* Lê os bytes de dados (em hexadecimal) */
        unsigned int temp_byte; /* Variável temporária para o fscanf */

        for (int j = 0; j < pacote_atual.tamanho; j++) {
            
            /* 1. Lê o valor para o 'unsigned int' temporário */
            if (fscanf(in_f, "%x", &temp_byte) != 1) {
                fprintf(stderr, "Erro: Falha ao ler dados do pacote %d.\n", i);
                break;
            }
            
            /* 2. Atribui o valor ao 'unsigned char' */
            pacote_atual.dados[j] = (unsigned char)temp_byte;
        }

        /* * LÓGICA DO ROTEADOR:
         * Se o buffer NÃO ESTIVER VAZIO e o novo pacote ESTOURAR a capacidade...
         */
        if (pacotes_no_buffer > 0 && (bytes_no_buffer + pacote_atual.tamanho > capacidade_maxima)) {
            
            /* 1. Processa o lote atual: Ordena e Imprime */
            heapSort_desc(buffer_roteador, pacotes_no_buffer);
            imprimir_batch(out_f, buffer_roteador, pacotes_no_buffer);

            /* 2. Esvazia o buffer */
            pacotes_no_buffer = 0;
            bytes_no_buffer = 0;
        }

        /* * Adiciona o pacote atual ao buffer (que agora está vazio ou ainda tem espaço)
         * Nota: Se um único pacote for maior que a capacidade, ele será processado sozinho.
         */
        buffer_roteador[pacotes_no_buffer] = pacote_atual;
        bytes_no_buffer += pacote_atual.tamanho;
        pacotes_no_buffer++;
    }

    /* 5. Processar o último lote restante */
    /* Após o fim do loop, pode haver pacotes no buffer */
    if (pacotes_no_buffer > 0) {
        heapSort_desc(buffer_roteador, pacotes_no_buffer);
        imprimir_batch(out_f, buffer_roteador, pacotes_no_buffer);
    }


    /* 6. Limpeza */
    free(buffer_roteador);
    fclose(in_f);
    fclose(out_f);

   

    return EXIT_SUCCESS;
}