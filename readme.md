# Thread Pool

## 1. Implementação da Thread Pool

### Thread Pool

O pool de threads gerencia múltiplas threads trabalhadoras (workers) que executam tarefas da fila.

**Worker (`pool_worker`)**
- Cada thread executa esta função em loop contínuo.
- A thread espera enquanto a fila estiver vazia e o pool não estiver sendo encerrado.
- Ao receber uma tarefa, retira-a da fila (`task_queue_pop`) e executa sua função.
- Se o pool estiver em processo de desligamento (`stop`) e não houver mais tarefas, a thread encerra o loop.

**Inicialização do Pool (`pool_init`)**
- Define o número de threads e aloca o vetor de threads.
- Inicializa a fila de tarefas com a capacidade desejada.
- Cria as threads, associando cada uma à função `pool_worker`.

**Adição de Tarefas (`pool_run`)**
- Cria uma estrutura `task` com a função e argumento fornecidos.
- Adiciona a tarefa à fila usando `task_queue_push`.
- Caso o pool já esteja encerrado, não permite adicionar novas tarefas.

**Encerramento do Pool (`pool_shutdown`)**
- Bloqueia o mutex da fila.
- Aguarda a fila esvaziar, garantindo que todas as tarefas em execução terminem.
- Sinaliza para o pool que não serão aceitas novas tarefas (`stop = true`) e acorda threads em espera.
- Espera todas as threads terminarem (`pthread_join`).
- Libera a memória alocada e destrói mutexes e variáveis de condição.

**Considerações**
- O uso de **mutexes** garante que apenas uma thread manipule a fila por vez.
- As **condições** permitem que threads esperem eficientemente sem consumir CPU.
- O pool garante que todas as tarefas sejam concluídas antes do encerramento.
- A fila circular evita desperdício de memória ao reutilizar espaços já utilizados.

Esta implementação fornece uma base eficiente e segura para execução paralela de tarefas em C utilizando **POSIX threads (pthreads)**.

---

## 2. Execução das Versões do Programa

No projeto, foram criadas três versões de execução para a resolução de um problema. Dada uma lista de tamanho **N**, preenchida com números aleatórios, uma função é aplicada sobre todos os valores da lista, substituindo cada elemento pelo próximo número primo.

- A **versão sequencial** percorre a lista executando a função de cálculo do próximo primo em cada elemento.  
- A **versão paralela** divide a lista em partes iguais entre diversas threads, e cada thread processa uma fração da lista.  
- A **versão com biblioteca** utiliza a **Thread Pool** implementada conforme descrito anteriormente.  

A seguir, as instruções de compilação utilizando **GCC** para cada versão:

### Programa Sequencial

```bash
gcc programa_sequencial.c -o programa_sequencial
````

### Programa Paralelo

```bash
gcc programa_paralelo.c -o programa_paralelo
```

### Programa com Biblioteca (Pool de Threads)

```bash
gcc programa_biblioteca.c library/thread_pool.o -o programa_biblioteca -lpthread
```

---
