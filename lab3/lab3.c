#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#define N 4             // num rows and columns in input matrix

int input[N][N];
pthread_mutex_t mutex;
pthread_cond_t cond;
int count = 0;

void printarr(void) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ",input[i][j]);
        }
        printf("\n");
    }
}

void scanarr(void) {
    FILE * file = fopen("input.txt", "r");

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(file, "%d ", &input[i][j]);
        }
    }
    fclose(file);
}

int num_phase() {
    return ((log(N*N)/log(2))+1);
}

void swap (int *a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void row_bubblesort (int row) {
    for (int i = 0; i < N-1; i++) {
        for (int j = 0; j < N-i-1; j++) {
            if (row % 2 == 0) {
                if (input[row][j] > input[row][j+1]) swap(&input[row][j],&input[row][j+1]);
            }
            else {
                if (input[row][j] < input[row][j+1]) swap(&input[row][j],&input[row][j+1]);
            }
        }
    }
}

void col_bubblesort (int col) {
    for (int j = 0; j < N-1; j++) {
        for (int i = 0; i < N-j-1; i++) {
            if (input[i][col] > input [i+1][col]) swap(&input[i][col],&input[i+1][col]);
        }
    }
}

 void * shearsort(void * arg) {

     int index = (int) arg;
     for (int phase = 1; phase <= num_phase(); phase ++) {

         switch (phase % 2) {
             case 1: row_bubblesort(index);
                    break;
             case 0: col_bubblesort(index);
                    break;
         }

         count++;
         if(count != N) {
             pthread_mutex_lock(&mutex);
             pthread_cond_wait(&cond,&mutex);
             pthread_mutex_unlock(&mutex);
         }
         else {
             count = 0;
             printf("Phase : %d\n", phase);
             printf("\n");
             printarr();
             printf("\n");
             pthread_cond_broadcast(&cond);
         }
     }
     pthread_exit(NULL);
 }

int main(void) {
    scanarr();
    printf("\nInitial Array\n\n");
    printarr();
    printf("\n");
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t thread[N];
    for (int i = 0; i < N; i++) {
        int test = pthread_create(&thread[i], NULL, shearsort, (void*) i);
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
