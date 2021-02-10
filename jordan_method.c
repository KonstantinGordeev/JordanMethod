#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define EPS 2.20e-16 // машинный эпсилон
#define T_NUM 1      // число потоков
sem_t sem, calculation, next_step;
static pthread_mutex_t mutex = {PTHREAD_MUTEX_INITIALIZER};
int t_num = T_NUM;
int *indi;	   // indi - массив длины n, хранящий номера строк матрицы А

typedef struct _arg {
    int loc_id;
    int n;
	double *A;
	double *b;
	double *x;
} ARG;

void *thread_func(void *arg_t)
{
	ARG *ar = (ARG*) arg_t;
	int start = ar->loc_id * ar->n / T_NUM;
	int finish = (ar->loc_id + 1) * ar->n / T_NUM;
	int nt = ar->n;
	double a;
	int i, j, k, m, t, buf;
	
	sem_post(&sem);
	printf("Thread %d created\n", ar->loc_id);
	
	for (i = 0; i < nt; i++){ // (n-i) - ранг подматрицы
		pthread_mutex_lock(&mutex);
		t_num--;
		if (t_num == 0) {
			a = ar->A[indi[i]*nt+i];
			for (j = i; j < nt; j++) {  //переставляем максимальный элемент на первую позицию в подматрице
				if (fabs(ar->A[j*nt+i]) > fabs(a)) {
					a = ar->A[j*nt+i];
					buf = indi[i];
					indi[i] = indi[j];
					indi[j] = buf;
				}
			}
			
			if (fabs(a) < EPS) { 
				printf("Error! The matrix is degenerate\n");
				exit(1);
			}
			
			for (k = 0; k < nt; k++) {  // нормируем по первому коэффициенту
				ar->A[indi[i] * nt + k] /= a;
			}
			ar->b[indi[i]] /= a;
			for (t = 0; t < T_NUM; t++) sem_post(&calculation);
			t_num = T_NUM;
		}
		pthread_mutex_unlock(&mutex);
		sem_wait(&calculation);
		
		for (m = start; m < finish; m++){  // вычитаем строку из всех остальных с подходящим коэффициентом
			if (m != indi[i]){
				a = ar->A[m*nt+i];
				for (k = i; k < nt; k++) ar->A[m*nt+k] -= a * ar->A[indi[i]*nt+k];
				ar->b[m] -= a * ar->b[indi[i]];
			}
		}
		
		pthread_mutex_lock(&mutex); // без этого мьютекса моя программа работает некорректно, t_num считается неправильно
		t_num--;
		if (t_num == 0){
			for (t = 0; t < T_NUM; t++) sem_post(&next_step);
			t_num = T_NUM;
		}
		pthread_mutex_unlock(&mutex);
		sem_wait(&next_step);
	}
	
	// запись решения системы
	for (i = start; i < finish; i++) {
		ar->x[i] = ar->b[i];
	}
	
    return NULL;
}

int jordan_method(int n, double *A, double *b, double *x) {
	int i, id, result;
	pthread_t threads[T_NUM];
	ARG arg[T_NUM];
	sem_init(&sem, 0, 0);
	sem_init(&calculation, 0, 0);
	sem_init(&next_step, 0, 0);
	indi = malloc (n * sizeof(double));
	for (i = 0; i < n; i++){
		indi[i] = i;
	}
	for (i = 0; i < T_NUM; i++) {
		arg[i].A = A;
		arg[i].b = b;
		arg[i].x = x;
		arg[i].n = n;
		arg[i].loc_id = i;
	}
	
	for (id = 0; id < T_NUM; id++) {
		result = pthread_create(&threads[id], NULL, thread_func, &arg[id]);
		if (result != 0) {
			printf("Creating the %d thread failed\n", id); 
			return EXIT_FAILURE;
		}
		sem_wait(&sem);
	}
	for (id = 0; id < T_NUM; id++) {
		result = pthread_join(threads[id], NULL);
		if (result != 0) {
			printf("Joining the %d thread failed",id); 
			return EXIT_FAILURE;
		}
	}
}