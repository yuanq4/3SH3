#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#define N 4

int input[N][N];
pthread_mutex_t mutex;
pthread_mutex_t print;
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

void row_bubblesort (int arr[N][N], int row){
    for (int i = 0; i < N-1; i++) {
        for (int j = 0; j < N-i-1; j++) {
            if (row % 2 == 0) {
                if (arr[row][j] > arr [row][j+1]) swap(&arr[row][j],&arr[row][j+1]);
            }
            else {
                if (arr[row][j] < arr [row][j+1]) swap(&arr[row][j],&arr[row][j+1]);
            }
        }
    }
}

void col_bubblesort (int arr[N][N], int col){
    for (int i = 0; i < N-1; i++) {
        for (int j = 0; j < N-i-1; j++) {
            if (arr[j][col] > arr [j+1][col]) swap(&arr[j][col],&arr[j+1][col]);
        }
    }
}

 void * shearsort(void * arg) {

     int index = (int) arg;
     for (int phase = 1; phase <= num_phase(); phase ++) {

         switch (phase % 2) {
             case 1: row_bubblesort(input,index);
             case 0: col_bubblesort(input,index);
         }

         pthread_mutex_lock(&print);
         printf("phase = %d, index = %d\n", phase,index);
         printf("\n");
         printarr();
         printf("\n");
         pthread_mutex_unlock(&print);

         count++;
         if(count != N) {
             pthread_mutex_lock(&mutex);
             pthread_cond_wait(&cond,&mutex);
             pthread_mutex_unlock(&mutex);
         }
         else {
             count = 0;
             pthread_cond_broadcast(&cond);
         }
     }
     pthread_exit(NULL);
 }

int main(void) {
    scanarr();
    printarr();
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&print, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_t thread[N];
    for (int i = 0; i < N; i++) {
        int test = pthread_create(&thread[i], NULL, shearsort, (void*) i);
    }
    pthread_exit(NULL);
}
