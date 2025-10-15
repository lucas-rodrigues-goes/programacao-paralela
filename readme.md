# Programação Paralela em C

## Execução

No projeto, foram criadas três versões de execução para a resolução de um problema. Dada uma lista de tamanho **N** preenchida com números aleatoriamente gerados, uma função é executada sobre todos os valores desta lista, substituindo cada elemento pelo próximo número primo.

* A **versão sequencial** simplesmente percorre a lista, executando a função de cálculo do próximo primo em cada elemento.
* A **versão paralela** divide a lista em partes iguais entre diversas threads, e cada thread percorre uma fração da lista.
* A **versão com biblioteca** utiliza a Pool de threads implementada em `/library`.

Abaixo seguem as instruções de compilação utilizando **GCC** para cada versão.

###### Programa Sequencial

```bash
gcc programa_sequencial.c -o programa_sequencial
```

###### Programa Paralelo

```bash
gcc programa_paralelo.c -o programa_paralelo
```

###### Programa com Biblioteca (Pool de Threads)

```bash
gcc programa_biblioteca.c library/thread_pool.o -o programa_biblioteca -lpthread
```

---
