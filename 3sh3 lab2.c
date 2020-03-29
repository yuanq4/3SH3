# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>

int main(void){
	pid_t pid;
	int input=0;
	int fd[2];
	pipe(fd);
	if (( pid = fork ()) == -1){
		perror (" fork ");
		exit (0);
	}
	if(pid==0){
		int result;
		while(input!=-1){
			scanf("%d",&input);
			close(fd[0]);
			write(fd[1],&input,sizeof(int));	
		}
		close(fd[1]);
		read(fd[0],&result,sizeof(int));
		printf("%d\n",result);
	}
	if(pid!=0){
		int count=0;
		char arr[80];
		do{
			close(fd[1]);
			read(fd[0],&input, sizeof(int));
			arr[count++]=input;
		}while(input!=-1);
		int sum=0;
		for (int i=0;i<count-1;i++){
			sum=sum+arr[i];
		}
		close(fd[0]);
		write(fd[1],&sum,sizeof(int));
		printf("%d",sum);
	}

}
