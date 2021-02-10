all :  lineareq

lineareq : main.o input.o jordan_method.o
	gcc main.o input.o jordan_method.o -lpthread -lm -o lineareq -O3

main.o : main.c
	gcc -c main.c
	
input.o : input.c
	gcc -c input.c

gauss_method.o : jordan_method.c
	gcc -c jordan_method.c
	
clean:
	rm -rf *.o lineareq

#.o - объектные файлы
