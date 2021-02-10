#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#define GET_TIME_H
long int get_time();
double get_full_time();

void output(int n, int l, int m, double *A);
double norm_abs(int n, double *A, double *b, double *x);
double norm_sqr(int n, double *A, double *b, double *x);
double norm_max(int n, double *A, double *b, double *x);
int min(int x, int y);
void matr_file(int n, char *filename, double *A);
void matr_input(int k, int n, double *A);
int jordan_method(int n, double *A, double *b, double *x);

int main(int argc, char *argv[]) {
	
	double norm_A, *A, *b, *x;
	int i, j, n, m, k;
	double start, end;  // таймеры

	start = get_full_time();
	
	if ( (argc < 4) || (argc > 5) )  {
		printf("Error! Incorrect number of parameters.\n");
		return -1;
	}
	
	n = atoi(argv[1]);  // размерность
	m = atoi(argv[2]);  // число выводимых значений в матрице
	k = atoi(argv[3]);  // номер формулы инициализации
	
	if ( ( (k == 0) && (argc != 5) ) || ( (k != 0) && (argc != 4) ) ) {
		printf("Error! Incorrect number of formula.\n");
		return -1;
	}
	
	
	A = (double*)malloc(n*n*sizeof(double));  // выделение памяти под матрицу
	if (A == NULL) return -1;
	
	if (k != 0) {
		matr_input(k, n, A);
	}
	else {
		matr_file(n, argv[4], A);
	}
	printf("The matrix A : \n");
	output(n, n, m, A);
	
	x = (double*)malloc(n*sizeof(double));
	if (x == NULL) return -1;
	
	b = (double*)malloc(n*sizeof(double));
	if (b == NULL) return -1;
	
	for (i = 0; i < n; i++) {  // инициализация вектора b
		b[i] = 0;
		for (j = 0; j < n; j += 2) {
			b[i] += A[i*n+j];
		}
	}
	
	if (jordan_method(n, A, b, x) == EXIT_FAILURE) return -1;
	
	if (k != 0) {
		matr_input(k, n, A);
	}
	else {
		matr_file(n, argv[4], A);
	}
	for (i = 0; i < n; i++) {
		b[i] = 0;
		for (j = 0; j < n; j += 2) {
			b[i] += A[i*n+j];
		}
	}
	
	printf("The vector x : \n");
	output(n, 1, m, x);
	
	norm_A = norm_abs(n, A, b, x);
	printf("The l1 norm of the discrepancy equals  %10.3e \n", norm_A);
	
	norm_A = norm_sqr(n, A, b, x);
	printf("The l2 norm of the discrepancy equals  %10.3e \n", norm_A);
	
	norm_A = norm_max(n, A, b, x);
	printf("The l-inf norm of the discrepancy equals  %10.3e \n", norm_A);
	
	end = get_full_time();
	printf("Program execution time :  %lf \n", (end - start) );
	
	return 0;
}

void output(int n, int l, int m, double *A) {

	int i, counter_i = 0;
	int newl = min(l, m);
	int newc = min(n, m);
	
	for (i = 0; i < newl*newc; i++){
		printf(" %10.3e", A[i + (n - newc) * counter_i]);
		if ( (i+1) % newc == 0 ) {
			counter_i += 1;
			printf("\n");
		}
	}
}


int min(int x, int y){
	if (x >= y) return y;
	else return x;
}

double norm_abs(int n, double *A, double *b, double *x) {  // норма невязки в l_1
	int i, j;
	double Ax, norm = 0, norm_b = 0;
	
	for(i = 0; i < n; i++){
		Ax = 0;
		for (j = 0; j < n; j++){
			Ax += A[i*n+j] * x[j];
		}
		Ax -= b[i];
		norm += fabs(Ax);
		norm_b += fabs(b[i]);
	}
	
	norm = norm / norm_b;
	return norm;
}

double norm_sqr(int n, double *A, double *b, double *x) {  // норма невязки в l_2
	int i, j;
	double Ax, norm = 0, norm_b = 0;
	
	for(i = 0; i < n; i++){
		Ax = 0;
		for (j = 0; j < n; j++){
			Ax += A[i*n+j] * x[j];
		}
		Ax -= b[i];
		norm += Ax * Ax;
		norm_b += b[i] * b[i];
	}
	
	norm = sqrt(norm / norm_b);
	return norm;
}

double norm_max(int n, double *A, double *b, double *x) {  // норма невязки в l_inf
	int i, j;
	double Ax, norm = 0, norm_b = 0;
	
	for(i = 0; i < n; i++){
		Ax = 0;
		for (j = 0; j < n; j++){
			Ax += A[i*n+j] * x[j];
		}
		Ax -= b[i];
		Ax = fabs(Ax);
		b[i] = fabs(b[i]);
		if (Ax > norm) norm = Ax;
		if (b[i] > norm_b) norm_b = b[i];
	}
	
	norm = norm / norm_b;
	return norm;
}

long int get_time() {
	struct rusage buf;
	
	getrusage (RUSAGE_SELF, &buf);
	return buf.ru_utime.tv_sec * 100 + buf.ru_utime.tv_usec / 10000;
}

// Из учебника Богачева
double get_full_time() {
	struct timeval tm;
	
	gettimeofday(&tm, 0);
	
	return tm.tv_sec + (tm.tv_usec)/1000000.;
}
