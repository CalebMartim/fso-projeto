/*
Nomes:
  Caleb Martim -- 221017060
  Gabriel Henrique do Nascimento -- 221029140
  Guilherme da Rocha -- 221030007
  Hiago Sousa -- 221002049

Compilador: gcc 13.3.0

Sistema Operacional: LINUX 6.14.0-33-generic
*/


#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define true 1
#define false 0

const char *exit_s = "exit_scheduler";
const char *list_s = "list_scheduler";
const char *exec_s = "execute_process";
const char *user_s = "user_scheduler";

int main(){
  char command[100];

  while (true) {
    printf(">shell_sched: ");
    
    if(fgets(command, sizeof(command), stdin) == NULL) { // fgets le toda a linha
      fprintf(stderr, "Erro: falha ao ler o comando.\n");
      exit(EXIT_FAILURE);
    }
    command[strcspn(command,"\n")] = '\0'; // remove o '\n'

    if(strcmp(command, exit_s) == 0) break;
    
    if(strcmp(command, list_s) == 0) {
      printf("Lista de processos: [...]\n");

      printf("Processo em execução: [...]\n");

      continue;
    }

    char *token = strtok(command, " ");
    if(token != NULL) {
      if(strcmp(token, user_s) == 0) {
        token = strtok(NULL, " "); // numero de listas

        printf("Criando %s filas round-robin.\n",token);

        continue;
      }

      if(strcmp(token, exec_s) == 0) {
        token = strtok(NULL, " "); // prioridade

        printf("Executando processos de prioridade %s.\n",token);

        continue;
      }

    }

    fprintf(stderr,"Erro: Comando não definido.\n");
    exit(EXIT_FAILURE); // exit(1) | return 1
  }

  return EXIT_SUCCESS; // return 0
}
