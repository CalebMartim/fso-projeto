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

// Header de funções e structs auxiliares:
#include "aux.h"

// Bibliotecas de mecanismos de comunicacao
#include "sys/types.h"
#include "sys/ipc.h"

// Biblioteca da shared memory
#include "sys/shm.h"

// Biblioteca de fila de mensagem
#include "sys/msg.h"

const char *exit_s = "exit_scheduler";
const char *list_s = "list_scheduler";
const char *exec_s = "execute_process";
const char *user_s = "create_user_scheduler";

// Chaves unicas para a criacao das shms
#define SHM_KEY_RR 0x1234
#define SHM_KEY_FP 0x5678
#define MSG_KEY 0x1233

int pid_sched;  // pid do sched para mandar mensagens

int main(){
  char command[100];

  // Cria a shared memory para o array de filas round-robin:
  long tamanho_round_robin = sizeof(Queue) * 3;
  int shm_id_round_robins = shmget(SHM_KEY_RR, tamanho_round_robin, IPC_CREAT | 0777);
  if(shm_id_round_robins < 0) {
    fprintf(stderr, "Erro: falha ao criar shm: array de filas round-robin.\n");
    exit(EXIT_FAILURE);
  }
  
  // Cria a shared memory para a fila de processos finalizados
  long tamanho_finished_processes = sizeof(Queue);
  int shm_id_finished_processes = shmget(SHM_KEY_FP, tamanho_finished_processes, IPC_CREAT | 0777);
  if(shm_id_finished_processes < 0) { 
    fprintf(stderr, "Erro: falha ao criar shm: fila de processos finalizados.\n");

    // frees necessarios
    shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin

    exit(EXIT_FAILURE);
  }

  // Cria a fila de mensagens
  int msg_id = msgget(MSG_KEY, IPC_CREAT | 0777);
  if(msg_id < 0) {
    fprintf(stderr, "Erro: falha ao criar a fila de mensagens.\n");
      
    // frees necessarios
    shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin
    shmctl(shm_id_finished_processes, IPC_RMID, NULL);  // free na fila de processos finalizados
    exit(EXIT_FAILURE);
  }

  // declaracao da struct mensagem
  mensagem mensagem_main;
  mensagem_main.pid = getpid();

  while (true) {
    printf(">shell_sched: ");
    if(fgets(command, sizeof(command), stdin) == NULL) { // fgets le toda a linha
      fprintf(stderr, "Erro: falha ao ler o comando.\n");

      // frees necessarios
      shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin
      shmctl(shm_id_finished_processes, IPC_RMID, NULL);  // free na fila de processos finalizados
      msgctl(msg_id, IPC_RMID, NULL); // free na fila de mensagens

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

      // le o ponteiro da array de filas round-robin
      Queue** round_robins = (Queue**) shmat(shm_id_round_robins, (char*) 0, 0);
      if(round_robins == (int *) -1) {
          fprintf(stderr, "Erro: falha ao ler da memória compartilhada (attach).\n");
          exit(EXIT_FAILURE);
      }

      /*Esta dando falha de segmentacao

      for(int i = 0; i < 3; i++) {
        printf("Fila %d: ", i+1);
        char* str_queue = to_string(round_robins[i]);
        printf("%s\n",str_queue);
        free(str_queue);
      }
      
      */

      printf("Processo em execução: ");
      
      continue;
    }
    char *token = strtok(command, " ");
    if(token != NULL) {
      if(strcmp(token, user_s) == 0) {
        char* n = strtok(NULL, " "); // numero de filas

        // salva o pid do sched
        pid_sched = fork();
        if (pid_sched == 0) {

          // Casting para char* das ids
          char arg_shm_id_rr[20], arg_shm_id_fp[20], arg_msg[20];
          sprintf(arg_shm_id_rr,"%d",shm_id_round_robins);
          sprintf(arg_shm_id_fp,"%d",shm_id_finished_processes);
          sprintf(arg_msg,"%d",msg_id);

          // chama o executavel do "sched.c"
          execl("source/sched","sched",n,arg_shm_id_rr,arg_shm_id_fp,arg_msg,NULL);

          fprintf(stderr, "Erro: falha ao executar o comando 'execl'.\n");
          
          // frees necessarios
          shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin
          shmctl(shm_id_finished_processes, IPC_RMID, NULL);  // free na fila de processos finalizados
          msgctl(msg_id, IPC_RMID, NULL); // free na fila de mensagens

          exit(EXIT_FAILURE);
        }

        /*Teste da troca de mensagens
        
        sleep(5);
        printf("Main: vou mandar mensagem...\n");
        strcpy(mensagem_main.msg, "Oie quer conversar? XP\n");
        msgsnd(msg_id, &mensagem_main, sizeof(mensagem_main), 0);
        
        */

        continue;
      }
      if(strcmp(token, exec_s) == 0) {
        char* pr = strtok(NULL, " "); // prioridade
        /*TODO - mandar pro processo filho as infos do novo processo a ser escalonado*/

        /*TODO - corrigir essa parte
        
        voltar_para_o_inicio = true;
        
        */

        continue;
      }
    }
    fprintf(stderr,"Erro: Comando não definido.\n");

    // frees necessarios
    shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin
    shmctl(shm_id_finished_processes, IPC_RMID, NULL);  // free na fila de processos finalizados
    msgctl(msg_id, IPC_RMID, NULL); // free na fila de mensagens

    exit(EXIT_FAILURE); // exit(1) | return 1
  }

  /*TODO - frees necessarios*/
  shmctl(shm_id_round_robins, IPC_RMID, NULL);  // free nas filas round-robin
  shmctl(shm_id_finished_processes, IPC_RMID, NULL);  // free na fila de processos finalizados
  msgctl(msg_id, IPC_RMID, NULL); // free na fila de mensagens

  return EXIT_SUCCESS; // return 0
}
