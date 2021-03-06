#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define NLINHAS 6144
#define NCOLUNAS 6144
#define THREADS_PER_BLOCK 1024
//32x32
#define NTHREADS 32

__global__ void vector_mul(int *a, int *b, int *c) {
    int i, soma = 0;

    int coluna = blockIdx.x * blockDim.x + threadIdx.x;
    int linha =  blockIdx.y * blockDim.y + threadIdx.y;

    int beginA = NLINHAS * linha;
    int beginB = coluna;

    for (i = 0; i < NLINHAS; i++) {
        soma += a[beginA + i] * b[NLINHAS * i + beginB];
    }

    //printf("%d %d\n", linha, coluna);
    c[linha * NLINHAS + coluna] = soma;
}

int main(){
    int *a, *b, *c;
    int *d_a, *d_b, *d_c;
    int size = NLINHAS * NCOLUNAS * sizeof(int);
    int i, j, n;

    struct timeval timevalA;
	struct timeval timevalB;

    cudaMalloc((void **) &d_a, size);
    cudaMalloc((void **) &d_b, size);
    cudaMalloc((void **) &d_c, size);

    a = (int *)malloc(size);
    b = (int *)malloc(size);
    c = (int *)malloc(size);

    for(i = 0; i < NLINHAS*NCOLUNAS; i++){
//        a[i] = b[i] = i % 10;
        c[i] = 0;
    }

    scanf("%d", &n);

    for (int i = 0; i < NLINHAS; i++) {
        for (j = 0; j < NLINHAS; j++) {
            scanf("%d", &a[i * NLINHAS + j]);
        }
    }

    for (int i = 0; i < NLINHAS; i++) {
        for (j = 0; j < NLINHAS; j++) {
            scanf("%d", &b[i * NLINHAS + j]);
        }
    }

    gettimeofday(&timevalA,NULL);
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

    dim3 tbloco = dim3(NLINHAS/NTHREADS, NLINHAS/NTHREADS,1);
    dim3 tthreads = dim3(NTHREADS, NTHREADS, 1);
    vector_mul<<<tbloco,tthreads>>>(d_a, d_b, d_c);
    
    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
    gettimeofday(&timevalB,NULL);
    
    // imprimir primeira coluna
    // for (i = 0; i < NLINHAS; i++) {
    //     for (j = 0; j < NLINHAS; j++) {
    //         printf("%d ", c[i * NLINHAS + j]);
    //     }
    //     printf("\n");
    // }
    //printf("\n");

    printf("%.5lf\n", timevalB.tv_sec-timevalA.tv_sec+(timevalB.tv_usec-timevalA.tv_usec)/(double)1000000);

    free(a); free(b); free(c);

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    return 0;
}
