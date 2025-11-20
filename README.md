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
