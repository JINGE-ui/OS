#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdio.h>
#include<unistd.h>
#define N 10

const char* input_addr = "input.txt";

union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};
struct shared_use_ds{
	int start;  //read,start指向待读的
	int end;   //write,end指向待写区
	char text[N];  //环形缓冲区
};

void P(int semid,int index)
{	  
	struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op = -1;	
    sem.sem_flg = 0;	//操作标记：0或IPC_NOWAIT等	
    semop(semid,&sem,1);	//1:表示执行命令的个数
    return;
}
void V(int semid,int index)
{	 
	struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;	
    semop(semid,&sem,1);	
    return;
}
union semun arg;
struct sembuf sem;


int main(){

	int semid;
	if((semid = semget((key_t)5678,3,IPC_CREAT|0666))==-1){  //获取信号量
		printf("get.cpp semget error!\n");
		exit(2);
	}

	int shmid = shmget((key_t)1234,sizeof(struct shared_use_ds),0666|IPC_CREAT);  //获取共享内存
	if(shmid==-1){
		printf("shmget error!\n");
		exit(2);
	}

	struct shared_use_ds* write_addr = NULL;
	write_addr = (struct shared_use_ds*)shmat(shmid,0,0);
	

	FILE* fpr=NULL;
	fpr = fopen(input_addr,"r");
	if(fpr==NULL){
		printf("fopen input.txt error!\n");
		exit(2);
	}

	char ch;
	int put = 0;
	while(!feof(fpr)){
		ch = fgetc(fpr);
		//fread(&ch,sizeof(char),1,fpr);
		P(semid,1); //缓冲区空余位置数
		//P(semid,0);
		write_addr->text[write_addr->end]=ch;
		write_addr->end=(write_addr->end+1)%N;
		//V(semid,0);
		V(semid,2); //缓冲区已写入数
		put++;
		if(put%10==0){
			printf("PUT: have put %d bytes to buf\n",put);
		}
		if(ch==EOF){
			break;
		}
	}
	fclose(fpr);
	printf("success read input.txt!\n");
	return 0;

}