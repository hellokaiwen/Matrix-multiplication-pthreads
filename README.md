# Matrix-multiplication-pthreads
This program performs matrix multiplication using pthreads. This program also explores the most efficient loop ordering for multiplying matrices. See functions mmul1, mmul2, mmul3, and mmul4 for further details. 
All matrices are squared, stored in a squashed format, meaning that matrices are stored using one-dimensional arrays. To access the entry at i-th row and j-th column, one requests the entry at index i * n + j in the corresponding one-dimensional array , where n is the number of rows/columns in that matrix.
