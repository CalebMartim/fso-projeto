/*
Nomes: 
  Caleb Martim de Oliveira -- 221017060
  Gabriel Henrique do Nascimento Neres -- 221029140
  Guilherme da Rocha Cunha -- 221030007
  Hiago Sousa Rocha -- 221002049

Compilador: gcc 13.3.0
Compilador de verdade: gcc 15.2.1

Sistema operacional: LINUX 6.14.0-33-generic
Sistema operacional de verdade: linux-6.17.7.arch1-1-x86_64 
LOONA - [+ +]
https://youtu.be/yymyRBvD79A
*/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

#include "process.h"
#include "queue.h"



// Header de funções auxiliares:
#include "aux.h"

// Bibliotecas da shared memory
#include "sys/ipc.h"
#include "sys/shm.h"

const char *exit_s = "exit_scheduler";
const char *list_s = "list_scheduler";
const char *exec_s = "execute_process";
const char *user_s = "create_user_scheduler";

int pid = 0;

#define SHM_KEY 0x1234

int pipeline[2]; // pipeline[0] : leitura ; pipeline[1] : escrita

bool voltar_para_o_inicio = false;
Queue* finished_processes;
Queue* round_robins[3];

void tratamento() {
  char* mensagem;
  read(pipeline[0], mensagem, sizeof(mensagem));
  int pr = string_to_int(mensagem);
  Process *p = new_process(++pid, pr);

  enqueue(round_robins[pr], p);
}

void scheduler(int n) {
  for (int i = 0; i < n; i++) round_robins[i] = new_queue();

  while (true) {
    for (int i = 0; i < n; ++i) {
      // Sinal para matar schedular
      
      if (voltar_para_o_inicio) {
        voltar_para_o_inicio = false;
        i = -1;
        continue;
      }

      if (is_empty(round_robins[i])) continue;

      // Processa o cara no início
      Process *p = dequeue(round_robins[i]);
      sleep(5); // quantum de 5s
      p->time_remaining -= 5;
      if (p->time_remaining == 0) {
        enqueue(finished_processes, p);
      } else {
        enqueue(round_robins[i], p);
      }

      if (!is_empty(round_robins[i])) i -= 1;
    }
  }
}

int main(){
  char command[100];

  // Cria a shared memory para o array de filas round-robin:
  long tamanho_round_robin = sizeof(round_robins);
  int shmid = shmget(SHM_KEY, tamanho_round_robin, IPC_CREAT | 0777);
  // TODO: tratar erro de criação de memória compart

  // Cria o pipeline
  pipe(pipeline);

  while (true) {
    printf(">shell_sched: ");
    if(fgets(command, sizeof(command), stdin) == NULL) { // fgets le toda a linha
      fprintf(stderr, "Erro: falha ao ler o comando.\n");
      exit(EXIT_FAILURE);
    }
    command[strcspn(command, "\n")] = '\0'; // remove o '\n'
    if(strcmp(command, exit_s) == 0) {
      /*TODO - termina o escalonador (processo filho) e mostra o turnaround de cada processos 
      e os processos não finalizados*/
      
      break;
    }
    if(strcmp(command, list_s) == 0) {
      /*TODO - mostrar os processos nas filas e o processo em execucao*/
      continue;
    }
    char *token = strtok(command, " ");
    if(token != NULL) {
      if(strcmp(token, user_s) == 0) {
        int n = string_to_int(strtok(NULL, " ")); // numero de filas

        int pid = fork();
        if (pid == 0) {
          scheduler(n);
        }

        continue;
      }
      if(strcmp(token, exec_s) == 0) {
        char* pr = strtok(NULL, " "); // prioridade
        /*TODO - mandar pro processo filho as infos do novo processo a ser escalonado*/

        write(pipeline[0], pr, sizeof(pr));
        voltar_para_o_inicio = true;

        continue;
      }
    }
    fprintf(stderr,"Erro: Comando não definido.\n");
    exit(EXIT_FAILURE); // exit(1) | return 1
  }
  /*TODO - frees necessarios*/

  shmclt(shmid, IPC_RMID);  // free shm

  return EXIT_SUCCESS; // return 0
}
