#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>

// Definições
int lista[18];
int num_threads = 6;

// Funções de ajuda
void mostra_lista (int lista[], int length) {
	int i = 0;
	printf("[");
	while (i < length) {
		printf("%d", lista[i]);
		if (i != (length - 1)) printf(", ");
		i = i + 1;
	}
	printf("]\n");
}
int prox_primo (int numero) {
	while (true) {
		numero = numero + 1;
		int i = 2;
		while (i <= numero) {
			if (i == numero) return i;

			// Se o número é divisível por qualquer um número que não seja 1 ou sí mesmo não é primo.
			else if ((numero % i) == 0) break;
			i = i + 1;
		}
	}
}

// Função de substituição (a ser executada pela thread)
void* atualiza_lista (void* arg) {
	int id = *(int*)arg;
	int lista_length = sizeof(lista) / sizeof(lista[0]);
	for (int i = id; i < lista_length; i += num_threads) {
		lista[i] = prox_primo(lista[i]);
	}
}

int main(void){
	struct timeval t1, t2;
	srand(time(NULL));

	// Inicio captura de tempo
	gettimeofday(&t1, NULL);

	// Inicia números aleatórios na lista
	int lista_length = sizeof(lista) / sizeof(lista[0]);
	for (int i = 0; i < lista_length; i++) {
		lista[i] = (rand() % 100) * 134718;
	}
	printf("Lista original:   ");
	mostra_lista(lista, lista_length);

	// Inicialização threads
	pthread_t threads[num_threads];
	int tid[num_threads];
	for (int i = 0; i < num_threads; i++) {
		tid[i] = i;
		pthread_create(&threads[i], NULL, atualiza_lista, &tid[i]);
	}
	
	// Finalização sincronizada das threads
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	// Exibir novos valores da lista
	printf("Lista atualizada: ");
	mostra_lista(lista, lista_length);

	// Fim captura de tempo
	gettimeofday(&t2, NULL);
	double t_total = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
	printf("\ntempo total = %fs\n", t_total);

	return 0;
}