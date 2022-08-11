#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<stdlib.h>

void Error(const char *mes);
int number = 0; //�Ϲ���º���
sem_t bin_sem;  //�������� ����
void *sem_snd(void *arg);
void *sem_rev(void *arg);

int main()
{
	int state = 0;
	pthread_t t1,t2,t3; //3���� ������ ����
	char thread1[] = "Thread_A";
	char thread2[] = "Thread_B";
	char thread3[] = "Thread_C";
	void * result_t = NULL;

	printf("sizeof(sem_t) = %d\n", sizeof(sem_t) );
	printf("sizeof(sem_t) = %d\n", sizeof(int) );
	exit(0);

	/*
	 * sem_init(sem_t *sem, int pshared, unsigned int value);
	 *
	 * sem�� ����Ű�� �������� ��ü�� �ʱ�ȭ �Ѵ�.
	 * �Ʒ� �ʱ�ȭ�������� ����° ���� 1�� ��ü�� �ʱ�ȭ �Ǹ� �ι�° ���� 0��
	 * ������� ���� ���μ��������� ������(���� ���μ��������� ��� = 0)
	 * �ƴϸ� ���� ���μ������� �������� �����Ѵ�.
	 * LinuxThreads�� ������� ���� ���μ��� ���� �����Ǵ� ���� �������� �ʱ⶧����
	 * �ι�° ���ڰ� 0�� �ƴ� ��� sem_init �� �׻� ENOSYS �����ڵ带 ��ȯ�Ѵ�.
	 */

	state = sem_init(&bin_sem,0,0);           //���� ���� ����
	
	if(state != 0)
		Error("sem_init Error\n");
	pthread_create(&t1,NULL,sem_snd,thread1); //������ 1 ���� (sem_snd ȣ��)
	pthread_create(&t2,NULL,sem_rev,thread2); //������ 2 ���� (sem_rev ȣ��)
	pthread_create(&t3,NULL,sem_rev,thread3); //������ 3 ���� (sem_rev ȣ��)

	pthread_join(t1,&result_t); //�����尡 ����Ǳ⸦ ��ٸ� �� ���� ���μ����� ����ǵ���
	pthread_join(t2,&result_t); //pthread_join �� ȣ��
	pthread_join(t3,&result_t); //�� ����� 3���� ������ ������ ��� ����ǰ� ���� ���μ����� ����ȴ�.

	printf("Finale Number : %d\n",number);
	/*
	 * int sem_destroy(sem_t *sem);
	 *
	 * ��������� �̿� ���õ� ���ҽ����� �Ҹ��Ų��.
	 */
	sem_destroy(&bin_sem);
	return 0;
}

void *sem_snd(void *arg)
{
	int i = 0;
	for(i=0; i<4; i++)
	{
		while(number != 0)
			sleep(1);
		number++;
		printf("%s Running : %d\n",(char*)arg,number);
		/*
		 * int sem_post(sem_t *sem);
		 * ���������� ���� �ϳ� ������Ų��.
		 */
		sem_post(&bin_sem);
	}
}
void *sem_rev(void *arg)
{
	int i = 0;
	for(i=0; i<2; i++)
	{
		/*
		 * int sem_wait(sem_t *sem);
		 *
		 * �������� ���� �ϳ� ���ҽ�Ų��
		 * ���� 0�� ��쿡�� 1 �̻��� �� ������ ��� ���¿� �ִٰ�
		 * 1�̻��� �Ǹ� ���� ���ҽ�Ű�� �����¿��� ���������� �ȴ�.
		 * �� ��������� 0���� ���� ���� �� �� ����.
		 */
		sem_wait(&bin_sem);
		number--;
		printf("%s Running : %d\n",(char*)arg,number);
	}
}
void Error(const char *mes)
{
	printf("%s\n",mes);
	exit(0);
}
