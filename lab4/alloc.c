#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define error_handler(tag) do { perror(tag); exit(1); } while (0)

key_t key = 5;
int sem;
int initval = 1;
struct sembuf inc[1];
struct sembuf dec[1];

void sem_init(void)
{
    sem = semget(key,1,0666 | IPC_CREAT);
    semctl(sem,0,SETVAL,initval);

    inc[0].sem_op = 1;
    dec[0].sem_op = -1;

    inc[0].sem_num = 0;
    inc[0].sem_flg = 0;
    dec[0].sem_num = 0;
    dec[0].sem_flg = 0;
}

int main(void) {

int fd, checkval, type, num, found, sem, start;
char * addr, * curr;
struct stat buffer;
sem_init();

fd = open ("res.txt", O_RDWR);
if (fd == -1) error_handler("open");

checkval = fstat(fd,&buffer);
if (checkval == -1) error_handler("size");

size_t filesize = buffer.st_size;
printf("Filesize: %zu\n", filesize);

addr = mmap(NULL, filesize, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (addr == MAP_FAILED) error_handler("mmap");

close(fd);

while(1) {
    found = 0;
    printf("\n[Alloc]:\n");
    printf("Enter Any Number to Allocate New Resource: ");
    scanf("%d",&start);

    semop(sem,dec,1);

    printf("\nCurrent State of Resources:\n");
    printf("\nResource | Quantity\n");
    printf("-------------------\n");
    for (int i = 0; i < filesize; i=i+4) {
        printf("    %d    |    %d\n",addr[i]-48, addr[i+2]-48);
    }

    printf("\n");

    printf("Resource Type: ");
    scanf("%d",&type);
    printf("Resource Quantity: ");
    scanf("%d",&num);

    while (num < 0) {
        printf("Incorrect Quantity entered, please try again\n");
        printf("Quantity: ");
        scanf("%d",&num);
    }

    for (int i = 0; i <= filesize; i=i+4) {
        if ( type == (addr[i]-48) ) {
            found = 1;
            curr = &addr[i+2];
            printf("\nCurrent Quantity: %c\n",*curr);
            if ( num <= (int) *curr-48 ) {
                *curr -= num;
                printf("New Quantity: %c\n",*curr);
                checkval = msync(addr, filesize, MS_SYNC);
                if (checkval == -1) error_handler("msync");
            }
            else {
                printf("Not Enough Resources\n");
                break;
            }
        }
    }
    if (!found) printf("Resource Type not found\n");

    semop(sem,dec,1);
}
}
