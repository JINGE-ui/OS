//readbuf 
//fprintf to output.txt
#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdio.h>
#include<unistd.h>
#define N 10
const char* output_addr = "output.txt";

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

	struct shared_use_ds* read_addr = NULL;
	read_addr = (struct shared_use_ds*)shmat(shmid,0,0);
	

	FILE* fpw=NULL;
	fpw = fopen(output_addr,"w");
	if(fpw==NULL){
		printf("fopen output.txt error!\n");
		exit(2);
	}
	int get =0;
	char a;
	while(1){
		P(semid,2); //缓冲区写入的数的个数
		//P(semid,0);
		a = read_addr->text[read_addr->start];
		read_addr->start=(read_addr->start+1)%N;
		//V(semid,0);
		V(semid,1); //缓冲区空余位置数
		get++;
		if((get%10==0)){
			printf("GET: have get %d bytes from buf\n",get);
		}
		if(a==EOF){
			break;
		}
		fputc(a,fpw);
		//putchar(a);
		//fwrite(&a,sizeof(char),1,fpw);

	}
	fclose(fpw);
	printf("success write to output.txt!\n");
	return 0;

}