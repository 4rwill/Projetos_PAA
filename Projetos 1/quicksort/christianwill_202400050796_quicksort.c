#include <stdio.h>
#include <stddef.h> 
#include <stdlib.h> 
#include <string.h> 

#define ID_LEN 3
#define STACK_MAX_DEPTH 2048 

typedef struct {
    char algo_id[ID_LEN];
    long metric_val;
} SortOutcome;

typedef struct {
    int* data;
    size_t count;
} IntArray;

typedef struct {
    int start;
    int end;
} QuicksortFrame;


IntArray* parse_buffer_fast(char* buffer, int* num_arrays_out);

void exchange_elements(int* ptr_a, int* ptr_b); 
int get_median_of_three_idx(int* arr_ptr, int start_idx, int end_idx); 
size_t partition_lomuto_variant(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt); 
int partition_hoare_variant(int* arr_ptr, int left_idx, int right_idx, long* swap_cnt); 

void quicksort_lomuto_pivot_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);
void quicksort_hoare_pivot_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);
void quicksort_lomuto_absolute_random_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);
void quicksort_hoare_absolute_random_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);
void quicksort_lomuto_tertile_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);
void quicksort_hoare_tertile_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt);

typedef void (*QuicksortFunc)(int*, int, int, long*, long*);

void insertion_sort_outcomes(SortOutcome* results_matrix, size_t num_algorithms);


IntArray* parse_buffer_fast(char* buffer, int* num_arrays_out) {
    char* p = buffer; 
    char* end_ptr;     

    int total_arrays_count = (int)strtol(p, &end_ptr, 10);
    p = end_ptr; 

    if (total_arrays_count <= 0) {
        fprintf(stderr, "Error: Invalid or missing number of arrays.\n");
        *num_arrays_out = 0;
        return NULL;
    }

    IntArray* collection_of_arrays = (IntArray*)malloc(total_arrays_count * sizeof(IntArray));
    if (collection_of_arrays == NULL) {
        perror("Error allocating memory for array collection");
        *num_arrays_out = 0;
        return NULL;
    }
    *num_arrays_out = total_arrays_count;

    for (int current_array_idx = 0; current_array_idx < total_arrays_count; ++current_array_idx) {
        int current_array_size = (int)strtol(p, &end_ptr, 10);
        p = end_ptr;

        if (current_array_size < 0) {
            fprintf(stderr, "Error reading size for array %d.\n", current_array_idx + 1);
            for (int cleanup_idx = 0; cleanup_idx < current_array_idx; ++cleanup_idx) {
                free(collection_of_arrays[cleanup_idx].data);
            }
            free(collection_of_arrays);
            *num_arrays_out = 0;
            return NULL;
        }

        collection_of_arrays[current_array_idx].count = (size_t)current_array_size;

        if (current_array_size == 0) {
            collection_of_arrays[current_array_idx].data = NULL;
            continue; 
        }

        collection_of_arrays[current_array_idx].data = (int*)malloc((size_t)current_array_size * sizeof(int));
        if (collection_of_arrays[current_array_idx].data == NULL) {
            perror("Error allocating memory for array elements");
            free(collection_of_arrays);
            *num_arrays_out = 0;
            return NULL;
        }

        for (int element_idx = 0; element_idx < current_array_size; ++element_idx) {
            collection_of_arrays[current_array_idx].data[element_idx] = (int)strtol(p, &end_ptr, 10);
            p = end_ptr;
        }
    }

    return collection_of_arrays;
}

void exchange_elements(int* ptr_a, int* ptr_b) {
    if (ptr_a != ptr_b) {
        int temp = *ptr_a;
        *ptr_a = *ptr_b;
        *ptr_b = temp;
    }
}

int get_median_of_three_idx(int* arr_ptr, int start_idx, int end_idx) {
    int sub_array_span = end_idx - start_idx + 1;
    
    int q1_idx = start_idx + (sub_array_span / 4);
    int mid_idx = start_idx + (sub_array_span / 2);
    int q3_idx = start_idx + (3 * sub_array_span / 4);

    int val_q1 = arr_ptr[q1_idx];
    int val_mid = arr_ptr[mid_idx];
    int val_q3 = arr_ptr[q3_idx];

    if ((val_q1 > val_mid) ^ (val_q1 > val_q3)) {
        return q1_idx;
    } 
    else if ((val_mid < val_q1) ^ (val_mid < val_q3)) {
        return mid_idx;
    } 
    else {
        return q3_idx;
    }
}

size_t partition_lomuto_variant(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt) {
    int pivot_val = arr_ptr[end_idx];
    int partition_idx = start_idx; 

    for (int current_idx = start_idx; current_idx < end_idx; ++current_idx) {
        if (arr_ptr[current_idx] <= pivot_val) {
            exchange_elements(&arr_ptr[partition_idx], &arr_ptr[current_idx]);
            (*swap_cnt)++;
            partition_idx++;
        }
    }
    exchange_elements(&arr_ptr[partition_idx], &arr_ptr[end_idx]);
    (*swap_cnt)++;
    return partition_idx;
}

int partition_hoare_variant(int* arr_ptr, int left_idx, int right_idx, long* swap_cnt) {
    int pivot_val = arr_ptr[left_idx]; 
    int ptr_a = left_idx - 1;
    int ptr_b = right_idx + 1;

    while (1) {
        do {
            ptr_b--;
        } while (arr_ptr[ptr_b] > pivot_val);

        do {
            ptr_a++;
        } while (arr_ptr[ptr_a] < pivot_val);

        if (ptr_a < ptr_b) {
            exchange_elements(&arr_ptr[ptr_a], &arr_ptr[ptr_b]);
            (*swap_cnt)++;
        } else {
            return ptr_b;
        }
    }
}

void quicksort_lomuto_pivot_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH]; 
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;
        
        (*call_cnt)++; 
        
        if (start >= end) {
            continue; 
        }
        
        int p = partition_lomuto_variant(arr_ptr, start, end, swap_cnt);
        
        stack[++top] = (QuicksortFrame){start, p - 1};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void quicksort_hoare_pivot_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH];
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;
        
        (*call_cnt)++;
        
        if (start >= end) {
            continue;
        }
        
        int p = partition_hoare_variant(arr_ptr, start, end, swap_cnt);
        
        stack[++top] = (QuicksortFrame){start, p};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void quicksort_lomuto_absolute_random_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH];
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;

        (*call_cnt)++;
        
        if (start >= end) {
            continue;
        }

        int sub_span = end - start + 1;
        int pivot_select_idx = start + (abs(arr_ptr[start]) % sub_span);
        exchange_elements(&arr_ptr[pivot_select_idx], &arr_ptr[end]);
        (*swap_cnt)++;
        
        int p = partition_lomuto_variant(arr_ptr, start, end, swap_cnt);
        stack[++top] = (QuicksortFrame){start, p - 1};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void quicksort_hoare_absolute_random_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH];
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;

        (*call_cnt)++;
        
        if (start >= end) {
            continue;
        }

        int sub_span = end - start + 1;
        int pivot_select_idx = start + (abs(arr_ptr[start]) % sub_span);
        exchange_elements(&arr_ptr[pivot_select_idx], &arr_ptr[start]); 
        (*swap_cnt)++;
        
        int p = partition_hoare_variant(arr_ptr, start, end, swap_cnt);
        stack[++top] = (QuicksortFrame){start, p};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void quicksort_lomuto_tertile_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH];
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;

        (*call_cnt)++;
        
        if (start >= end) {
            continue;
        }

        int median_elem_idx = get_median_of_three_idx(arr_ptr, start, end);
        exchange_elements(&arr_ptr[median_elem_idx], &arr_ptr[end]);
        (*swap_cnt)++;
        
        int p = partition_lomuto_variant(arr_ptr, start, end, swap_cnt);
        stack[++top] = (QuicksortFrame){start, p - 1};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void quicksort_hoare_tertile_iterative(int* arr_ptr, int start_idx, int end_idx, long* swap_cnt, long* call_cnt) {
    QuicksortFrame stack[STACK_MAX_DEPTH];
    int top = -1;
    stack[++top] = (QuicksortFrame){start_idx, end_idx};

    while (top >= 0) {
        QuicksortFrame frame = stack[top--];
        int start = frame.start;
        int end = frame.end;

        (*call_cnt)++;
        
        if (start >= end) {
            continue;
        }

        int median_elem_idx = get_median_of_three_idx(arr_ptr, start, end);
        exchange_elements(&arr_ptr[median_elem_idx], &arr_ptr[start]);
        (*swap_cnt)++;
        
        int p = partition_hoare_variant(arr_ptr, start, end, swap_cnt);
        stack[++top] = (QuicksortFrame){start, p};
        stack[++top] = (QuicksortFrame){p + 1, end};
    }
}

void insertion_sort_outcomes(SortOutcome* results_matrix, size_t num_algorithms) {
    for (size_t i = 1; i < num_algorithms; i++) {
        SortOutcome key = results_matrix[i];
        int j = i - 1;
        while (j >= 0 && (results_matrix[j].metric_val > key.metric_val || 
              (results_matrix[j].metric_val == key.metric_val && strcmp(results_matrix[j].algo_id, key.algo_id) < 0))) 
        {
            results_matrix[j + 1] = results_matrix[j];
            j = j - 1;
        }
        results_matrix[j + 1] = key;
    }
} 

int main (int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE* output_destination = fopen(argv[2], "w");
    if (output_destination == NULL) {
        perror("Error creating output file");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    char* file_buffer = (char*)malloc(file_size + 1); 
    if (file_buffer == NULL) {
        perror("Error allocating memory for file buffer");
        fclose(input_file);
        fclose(output_destination);
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(file_buffer, 1, file_size, input_file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error reading file into buffer.\n");
        free(file_buffer);
        fclose(input_file);
        fclose(output_destination);
        return EXIT_FAILURE;
    }
    file_buffer[file_size] = '\0'; 

    fclose(input_file); 

    int total_processed_arrays = 0;
    
    IntArray* array_collection = parse_buffer_fast(file_buffer, &total_processed_arrays);
    
    free(file_buffer); 

    if (array_collection == NULL || total_processed_arrays == 0) {
        fprintf(stderr, "Error: Failed to process input file or file was empty.\n");
        fclose(output_destination); 
        return EXIT_FAILURE;
    }


    struct AlgoTest {
        const char* name;
        QuicksortFunc func;
    } test_algorithms[] = {
        {"LP", quicksort_lomuto_pivot_iterative}, 
        {"HP", quicksort_hoare_pivot_iterative}, 
        {"LA", quicksort_lomuto_absolute_random_iterative}, 
        {"HA", quicksort_hoare_absolute_random_iterative}, 
        {"LM", quicksort_lomuto_tertile_iterative}, 
        {"HM", quicksort_hoare_tertile_iterative} 
    };
    const size_t num_algorithms = sizeof(test_algorithms) / sizeof(test_algorithms[0]);

    for (int current_array_idx = 0; current_array_idx < total_processed_arrays; ++current_array_idx) {
        IntArray current_target_array = array_collection[current_array_idx];

        if (current_target_array.count <= 1) { 
            if (current_target_array.count == 1) { 
                fprintf(output_destination, "[%zu]:", current_target_array.count);
                for (size_t res_idx = 0; res_idx < num_algorithms; ++res_idx) {
                    fprintf(output_destination, "%s(%ld)", test_algorithms[res_idx].name, 0L);
                    if (res_idx < num_algorithms - 1) fprintf(output_destination, ",");
                }
                fprintf(output_destination, "\n");
            }
             continue; 
        }

        SortOutcome results_matrix[num_algorithms];
        long current_swaps, current_calls;

        int* workspace_buffer = (int*)malloc(current_target_array.count * sizeof(int));
        if (workspace_buffer == NULL) {
            perror("Error allocating memory for workspace buffer");
            fclose(output_destination);
            return EXIT_FAILURE;
        }

        for (size_t algo_idx = 0; algo_idx < num_algorithms; ++algo_idx) {
            
            memcpy(workspace_buffer, current_target_array.data, current_target_array.count * sizeof(int));
            
            current_swaps = 0; 
            current_calls = 0;
            
            test_algorithms[algo_idx].func(workspace_buffer, 0, (int)current_target_array.count - 1, &current_swaps, &current_calls);
            
            strcpy(results_matrix[algo_idx].algo_id, test_algorithms[algo_idx].name);
            results_matrix[algo_idx].metric_val = current_swaps + current_calls;
            
        }

        free(workspace_buffer);

        insertion_sort_outcomes(results_matrix, num_algorithms);

        fprintf(output_destination, "[%zu]:", current_target_array.count);
        for (size_t res_idx = 0; res_idx < num_algorithms; ++res_idx) {
            fprintf(output_destination, "%s(%ld)", results_matrix[res_idx].algo_id, results_matrix[res_idx].metric_val);
            if (res_idx < num_algorithms - 1) fprintf(output_destination, ",");
        }
        fprintf(output_destination, "\n");
    }

    fclose(output_destination);
    for (int k = 0; k < total_processed_arrays; ++k) {
        free(array_collection[k].data);
    }
    free(array_collection);

    return EXIT_SUCCESS;
}