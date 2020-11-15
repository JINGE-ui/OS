#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>

using namespace std;
union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

int num=1000,semid;
union semun arg;
struct sembuf sem;

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

void *subp(void*){
	while(num>0){
		P(semid,0);
		cout<<"subp sell,leave "<<num-1<<endl;
		num--;
		V(semid,0);
	}
	cout<<"the num now is:"<<num<<endl;
	//pthread_exit(NULL);
}

void *subpf3(void*){
	int x3 = 0;
	while(num>0){
		P(semid,0);
		if(!num){
			V(semid,0);
			break;
		}
		cout<<"subp3 sell,remaining: "<<num-1<<endl;
		num--;
		x3++;
		V(semid,0);
	}
	cout<<"pthread 3 sold a total of:"<<x3<<endl;
}
void *subpf1(void*){
	int x1 = 0;
	while(num>0){
		P(semid,0);
		if(!num){
			V(semid,0);
			break;
		}
		cout<<"subp1 sell,remaining: "<<num-1<<endl;
		num--;
		x1++;
		V(semid,0);
	}
	cout<<"pthread 1 sold a total of:"<<x1<<endl;
}
void *subpf2(void*){
	int x2=0;
	while(num>0){
		P(semid,0);
		if(!num){
			V(semid,0);
			break;
		}
		cout<<"subp2 sell,remaining: "<<num-1<<endl;
		num--;
		x2++;
		V(semid,0);
	}
	cout<<"pthread 2 sold a total of:"<<x2<<endl;
}

int main(){
	pthread_t subp1,subp2,subp3;
	int key=ftok("/tmp",0x66);
	if(key<0){
		cout<<"ftok error!";
		return 1;
	}
	semid = semget(key,1,IPC_CREAT|0666);
	if(semid==-1){
		cout<<"semget error!";
		return 1;
	}
	arg.val = 1;
	if(semctl(semid,0,SETVAL,arg)<0){
		cout<<"semctl error!";
		return 1;
	}
	//创建信号灯
	pthread_create(&subp1,NULL,subpf1,NULL);
	pthread_create(&subp2,NULL,subpf2,NULL);
	pthread_create(&subp3,NULL,subpf3,NULL);

	pthread_join(subp1,NULL);
	pthread_join(subp2,NULL);
	pthread_join(subp3,NULL);

	semctl(semid,0,IPC_RMID,arg);
	return 0;

}