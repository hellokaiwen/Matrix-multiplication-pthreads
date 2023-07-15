#include <iostream>
#include <vector>
#include <chrono>

typedef struct {
    unsigned int matrix_size;
    double* squashed_a;
    double* squashed_b;
    unsigned start_row;
    unsigned end_row;
    double* squashed_result;
} Task;

void initialize_matrix(unsigned int, int, double**);
void to_squashed(unsigned int, double**, double*);
void to_vector(unsigned int, double**, std::vector<double>&);
void* mmul1(unsigned int, double*, double*, double*);
void* mmul2(unsigned int, double*, double*, double*);
void* mmul3(unsigned int, double*, double*, double*);
void* mmul4(std::vector<double>, std::vector<double>, std::vector<double>&);
void* multiply(void*);

int main(int argc, char* argv[]) {
    // Set up the matrices
    int seed = 42;
    unsigned int size = 2000, i;
    std::cout << size << std::endl;  /* Number of rows of the input matrices */

    double** a = new double*[size];
    double* squashed_a = new double[size * size];
    std::vector<double> vector_a {};
    double** b = new double*[size];
    double* squashed_b = new double[size * size];
    std::vector<double> vector_b {};
    for (i = 0; i < size; i++) {
        a[i] = new double[size];
        b[i] = new double[size];
    }
    initialize_matrix(size, seed, a);
    initialize_matrix(size, seed, b);
    to_squashed(size, a, squashed_a);
    to_squashed(size, b, squashed_b);
    to_vector(size, a, vector_a);
    to_vector(size, b, vector_b);

    double* result1 = new double[size * size];
    double* result2 = new double[size * size];
    double* result3 = new double[size * size];
    std::vector<double> result4 {};

    /* Print the time taken by mmul1 and the last element of the matrix produced by mmul1. */
    auto start = std::chrono::high_resolution_clock::now();
    mmul1(size, squashed_a, squashed_b, result1);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration.count() << std::endl;
    std::cout << result1[(size - 1) * size + size - 1] << std::endl;

    /* Print the time taken by mmul2 and the last element of the matrix produced by mmul2. */
    start = std::chrono::high_resolution_clock::now();
    mmul2(size, squashed_a, squashed_b, result2);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration.count() << std::endl;
    std::cout << result2[(size - 1) * size + size - 1] << std::endl;

    /* Print the time taken by mmul3 and the last element of the matrix produced by mmul3. */
    start = std::chrono::high_resolution_clock::now();
    mmul3(size, squashed_a, squashed_b, result3);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration.count() << std::endl;
    std::cout << result3[(size - 1) * size + size - 1] << std::endl;

    /* Print the time taken by mmul4 and the last element of the matrix produced by mmul4. */
    start = std::chrono::high_resolution_clock::now();
    mmul4(vector_a, vector_b, result4);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << duration.count() << std::endl;
    std::cout << result4[(size - 1) * size + size - 1] << std::endl;

    /* Multiply matrices using threads. */
    start = std::chrono::high_resolution_clock::now();
    double* result = new double[size * size];
    pthread_t threads[size];
    Task args[size];
    int rc;
    unsigned int t;
    for (t = 0; t < size; t++) {
        args[t] = {
                (unsigned)size, squashed_a, squashed_b, t, t + 1, result
        };
        rc = pthread_create(&threads[t], NULL, multiply, (void*)&args[t]);
        if (rc) {
            std::printf("ERROR: pthread_create rc is %d\n", rc);
            exit(-1);
        }
    }
    for (t = 0; t < size; t++) {
        pthread_join(threads[t], NULL);
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time of multi-threaded matrix multiplication: "
              << duration.count() << " milliseconds\n";

    return 0;
}

void initialize_matrix(unsigned int size, int seed, double** matrix) {
    unsigned int i, j;
    srand(seed);
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = (double) rand() / RAND_MAX;
        }
    }
}

void to_squashed(unsigned int size, double** matrix, double* squashed) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            squashed[i * size + j] = matrix[i][j];
        }
    }
}

void to_vector(unsigned int size, double** matrix, std::vector<double>& result) {
    int i, j;
    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            result.push_back(matrix[i][j]);
}

void* mmul1(unsigned int n, double* a, double* b, double* result) {
    /* ijk */
    unsigned int i, j, k;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0; k < n; k++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }

    return NULL;
}

void* mmul2(unsigned int n, double* a, double* b, double* result) {
    /* ikj */
    unsigned int i, j, k;

    for (i = 0; i < n; i++) {
        for (k = 0; k < n; k++) {
            for (j = 0; j < n; j++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }

    return NULL;
}

void* mmul3(unsigned int n, double* a, double* b, double* result) {
    /* jki */
    unsigned int i, j, k;

    for (j = 0; j < n; j++) {
        for (k = 0; k < n; k++) {
            for (i = 0; i < n; i++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }

    return NULL;
}

void* mmul4(std::vector<double> a, std::vector<double> b, std::vector<double>& result) {
    /* Precondition: a.size() == b.size(); */
    unsigned int n = (unsigned int)sqrt(a.size()), i, j, k;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            double sum = 0;
            for (k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n+ j];
            }
            result.push_back(sum);
        }
    }

    return NULL;
}

/* Multiply matrices in the given task using the loop ordering in mmul2. */
void* multiply(void* arg) {
    Task* task_info = (Task*)arg;
    unsigned int n = task_info->matrix_size;
    double* a = task_info->squashed_a;
    double* b = task_info->squashed_b;
    unsigned int start_row = task_info->start_row;
    unsigned int end_row = task_info->end_row;
    double* result = task_info->squashed_result;
    unsigned int i, j, k;

    for (i = start_row; i < end_row; i++) {
        for (k = 0; k < n; k++) {
            for (j = 0; j < n; j++) {
                result[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }

    pthread_exit(NULL);
}

//void print_matrix(unsigned int size, double* matrix) {
//    /* Precondition: matrix is in squashed format */
//    int i, j;
//    for (i = 0; i < size; i++) {
//        for (j = 0; j < size; j++) {
//            std::printf("%f ", matrix[i * size + j]);
//        }
//        std::printf("\n");
//    }
//}
//
//void print_matrix(std::vector<double> matrix) {
//    int n = (int)sqrt(matrix.size()), i, j;
//    for (i = 0; i < n; i++) {
//        for (j = 0; j < n; j++) {
//            std::printf("%f ", matrix[i * n + j]);
//        }
//        std::printf("\n");
//    }
//}
//
//void print(int size, double** matrix) {
//    int i, j;
//    for (i = 0; i < size; i++) {
//        for (j = 0; j < size; j++) {
//            std::printf("%f ", matrix[i][j]);
//        }
//        std::printf("\n");
//    }
//}