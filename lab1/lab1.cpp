#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<iostream>
#include<signal.h>

#define MAX_SEND 10

using namespace std;
int pipefd[2];
pid_t p1,p2;
int x = 1,x1=0,x2=0;

void INT_hander(int sig){
	while(1);
}
void father_hander(int sig){
	close(pipefd[1]);
	kill(p1,SIGUSR1);
	wait(NULL);

	kill(p2,SIGUSR2);
	wait(NULL);

	printf("\nparent send %d times",x-1);
	cout <<endl<< "Parent Process is Killed!"<<endl;
	exit(0);
}
void child1_hander(int sig){
	close(pipefd[0]);
	cout<<endl<<"child1 received "<<x1<<" times!";
	cout<<endl<<"Child Process 1 is Killed by Parent!";
	exit(0);
}
void child2_hander(int sig){
	close(pipefd[0]);
	cout<<endl<<"child2 received "<<x2<<" times!";
	cout<<endl<<"Child Process 2 is Killed by Parent!"<<endl;
	exit(0);
}

int main(){
	if(pipe(pipefd)<0){
		cout<<"pipe error!";
		return 0;
	}
	cout<<"Seting: the max send times is "<<MAX_SEND<<endl;

	if((p1=fork())==0){  //child1
		close(pipefd[1]);
		signal(SIGUSR1,child1_hander);
		signal(SIGINT,INT_hander);
		char msg1[10];
		//int x1=0;
		while(1){
			memset(msg1,10,0);
			read(pipefd[0],msg1,10);
			cout<<"child1 receive x:"<<msg1<<endl;
			x1++;
		}
	}else if((p2=fork())==0){  //child2
		close(pipefd[1]);
		signal(SIGUSR2,child2_hander);
		signal(SIGINT,INT_hander);
		char msg2[10];
		//int x2=0;
		while(1){
			memset(msg2,10,0);
			read(pipefd[0],msg2,10);
			cout<<"child2 receive x:"<<msg2<<endl;
			x2++;
		}
	}else{   //father
		signal(SIGINT,father_hander);
		//int x=1;
		close(pipefd[0]);
		while(x-1<MAX_SEND){
			string str = to_string(x);
			write(pipefd[1],str.data(),strlen(str.data()));
			x++;
			sleep(1);
		}
		close(pipefd[1]);
		kill(p1,SIGINT);
		kill(p2,SIGINT);

		kill(p1,SIGUSR1);
		wait(NULL);

		kill(p2,SIGUSR2);
		wait(NULL);

		printf("parent send %d times\n",x-1);
		cout<<"the send times is at the upper limit,parent process exit"<<endl;
	}
}