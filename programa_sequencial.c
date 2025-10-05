#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

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

int main(void){
	struct timeval t1, t2;
	srand(time(NULL));

	// Inicio captura de tempo
	gettimeofday(&t1, NULL);

	// Criação da Lista com números aleatórios
	int tam_lista = 10;
	int lista[tam_lista];
	int lista_length = sizeof(lista) / sizeof(lista[0]);
	int i = 0;
	while (i < tam_lista) {
		lista[i] = rand() % 10;
		//printf("Número %d adicionado a lista.\n", lista[i]);
		i = i + 1;
	}
	printf("Lista original:   ");
	mostra_lista(lista, lista_length);

	// Substituição dos valores da lista pelos seus próximos primos
	i = 0;
	while (i < tam_lista) {
		lista[i] = prox_primo(lista[i]);
		i = i + 1;
	}
	printf("Lista atualizada: ");
	mostra_lista(lista, lista_length);

	// Fim captura de tempo
	gettimeofday(&t2, NULL);
	double t_total = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
	printf("\ntempo total = %fs\n", t_total);

	return 0;
}