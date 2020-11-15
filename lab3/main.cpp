#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;
#define N 10  //缓冲区大小
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
char c;

int main(){
	int shmid;
	int semid;  //信号灯使用
	pid_t readbuf,writebuf;
	struct shmid_ds shmids;
	shmid = shmget((key_t)1234,sizeof(struct shared_use_ds),0666|IPC_CREAT);  //创建共享内存
	if(shmid==-1){
		printf("shmget error!\n");
		exit(1);
	}

	struct shared_use_ds* main_attach_addr = NULL;
	main_attach_addr = (struct shared_use_ds*)shmat(shmid,0,0); //启动主进程main.cpp对共享内存的访问
	if(main_attach_addr==(void*)-1){
		printf("main.cpp shmat error!\n");
		exit(1);
	}
	main_attach_addr->end = main_attach_addr->start = 0;  //init


	//下面创建信号灯
	semid = semget((key_t)5678,3,IPC_CREAT|0666);//使用2个信号量
	//0号：init=1,缓冲区读写锁
	//1号：init=N,空闲位置数
	//2号：init=0,缓冲区已有数据数
	arg.val = 1;
	if(semctl(semid,0,SETVAL,arg)<0){
		printf("semctl 0 error!\n");
		exit(0);
	}
	arg.val = N;
	if(semctl(semid,1,SETVAL,arg)<0){
		printf("semctl 1 error!\n");
		exit(0);
	}
	arg.val = 0;
	if(semctl(semid,2,SETVAL,arg)<0){
		printf("semctl 2 error!\n");
		exit(0);
	}

	//进程创建
	if((readbuf=fork())==0){
		printf("readbuf get created!\n");
		execl("./get","get",NULL);
		exit(0);
	}else if((writebuf=fork())==0){
		printf("writebuf get created!\n");
		execl("./put","put",NULL);
		exit(0);
	}else{  //father
		wait(NULL);
		wait(NULL);

		semctl(semid,3,IPC_RMID,arg);
		shmctl(shmid,IPC_RMID,NULL);
	}



}