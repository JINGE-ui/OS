#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/times.h>
#include <sys/time.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};

int create_Sem(int key, int size)
{
	int id;
	id = semget(key, size, IPC_CREAT|0666);//����size���ź���
	if(id < 0) {//�ж��Ƿ񴴽��ɹ�
		printf("create sem %d,%d error\n", key, size);//����ʧ�ܣ���ӡ����
	}
	return id;
}

void destroy_Sem(int semid)
{
	int res = semctl(semid,0,IPC_RMID,0);//��ϵͳ��ɾ���ź���
	if (res < 0) {//�ж��Ƿ�ɾ���ɹ�
		printf("destroy sem %d error\n", semid);//�ź���ɾ��ʧ�ܣ������Ϣ
	}
	return;
}

int get_Sem(int key, int size)
{
	int id;
	id = semget(key, size, 0666);//��ȡ�Ѿ��������ź���
	if(id < 0) {
		printf("get sem %d,%d error\n", key, size);
	}
	return id;
}

void set_N(int semid, int index, int n)
{
	union semun semopts; 
	semopts.val = n;//�趨�ź����ĳ�ֵΪn
	semctl(semid,index,SETVAL,semopts);//��ʼ���ź������ź������Ϊindex
	return;
}

void P(int semid, int index)
{
	struct sembuf sem;//�ź�����������
	sem.sem_num = index;//�ź������
	sem.sem_op = -1;//�ź���������-1ΪP����
	sem.sem_flg = 0;//������ǣ�0��IPC_NOWAIT��
	semop(semid,&sem,1);//1:��ʾִ������ĸ���
	return;
}

void V(int semid, int index)
{
	struct sembuf sem;//�ź�����������
	sem.sem_num = index;//�ź������
	sem.sem_op =  1;//�ź���������1ΪV����
	sem.sem_flg = 0; //�������
        semop(semid,&sem,1);//1:��ʾִ������ĸ���
	return;
}

int get_time_ms(void)
{
static struct timeval start_t;
	struct timeval now_t;
	if((start_t.tv_sec == 0)&&(start_t.tv_usec == 0)) {
		gettimeofday(&start_t, NULL);
	}
	gettimeofday(&now_t, NULL);
	return (now_t.tv_sec-start_t.tv_sec)*1000+(now_t.tv_usec-start_t.tv_usec)/1000;
}

