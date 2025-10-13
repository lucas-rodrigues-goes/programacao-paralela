#### Introdução



No projeto foram criadas 3 versões de execução para a resolução de um mesmo problema, onde dado uma lista de tamanho N preenchida com números aleatoriamente gerados, uma função é executada sobre todos os valores desta lista substituindo seus itens pelo próximo valor primo.

A versão sequencial simplesmente percorre a lista executando a função de calculo de próximo primo em cada um dos elementos, a versão paralela divide a lista em partes iguais entre diversas threads, e cada thread percorre uma fração da lista.

A versão Biblioteca, utiliza uma implementação de pool de threads, que cria uma interface de programação de nível mais alto para a atribuição de tarefas às threads.



#### Execução



Abaixo seguem as instruções de compilação utilizando GCC para cada uma das versões.

###### 

###### Programa Sequencial

* gcc programa\_sequencial.c -o programa\_sequencial



###### Programa Paralelo

* gcc programa\_paralelo.c -o programa\_paralelo



###### Programa Biblioteca (Pool de threads)

* gcc programa\_biblioteca.c library/thread\_pool.o -o programa\_biblioteca -lpthread
