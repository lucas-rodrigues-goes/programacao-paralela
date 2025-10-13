Quanto a execução dos programas:

Programa Sequencial

* gcc programa\_sequencial.c -o programa\_sequencial
* ./programa\_sequencial.exe



Programa Paralelo

* gcc programa\_paralelo.c -o programa\_paralelo
* ./programa\_paralelo.exe



Programa Biblioteca (Pool de threads)

* gcc programa\_biblioteca.c library/thread\_pool.o -o programa\_biblioteca -lpthread
* ./programa biblioteca.exe
