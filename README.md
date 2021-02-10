# JordanMethod
 The Jordan method of solving a linear system with the selection of the main element by column.
 
Программа должна получать все параметры в качестве аргументов командной строки. Аргументы командной строки:
1) n – размерность матрицы,
2) m – количество выводимых значений в матрице,
3) k – задает номер формулы для инициализации матрицы, должен быть равен 0 при вводе матрицы из файла
4) filename – имя файла, откуда надо прочитать матрицу. Этот аргумент отсутствует, если k! = 0.
Например, запуск
./a.out 4 4 0 a.txt
означает, что матрицу 4x4 надо прочитать из файла a.txt и выводить не более 4-х строк и столбцов матрицы, а запуск
./a.out 2000 6 1
означает, что матрицу 2000x2000 надо инициализировать по формуле номер 1 и выводить не более 6-ти строк и столбцов матрицы.