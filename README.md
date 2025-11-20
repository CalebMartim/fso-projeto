# fso-projeto
Projeto da disciplina Fundamentos de Sistemas Operacionais no semestre 2025.2

## Execução
Garanta a existência do diretório `bin` 

```bash
$ mkdir bin
```

Compile os programas necessários

```bash
$ gcc -o bin/sched source/sched.c | gcc -o shell_sched source/main.c | gcc -o bin/proc_exec source/proc_exec.c 
```

Inicialize a interface

```bash
$ ./shell_sched
```

## Instruções para a interface:

Inicialize o escalonador com:


```
>shell_sched: create_user_scheduler n
```

onde `n` é o número de filas round-robin desejadas no escalonador

(apenas um escalonador pode estar sendo executado por vez)

Inicialize um novo processo no escalonador com:
```
>shell_sched: execute_process pr
```
onde `pr` é a prioridade do processo, um inteiro entre 1 e `n`

Liste as informações dos processos carregados no escalonador com:
```
>shell_sched: list_scheduler
```

Termine a execução do escalonador atual com
```
>shell_sched: exit_scheduler
```

Saia da interface com
```
>shell_sched: exit
```
