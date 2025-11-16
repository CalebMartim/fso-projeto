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

// Biblioteca de fila de mensagem
#include "sys/msg.h"

// Biblioteca de sinais
#include "signal.h"

const char *exit_s = "exit_scheduler";
const char *list_s = "list_scheduler";
const char *exec_s = "execute_process";
const char *user_s = "create_user_scheduler";

// Chave para a fila de mensagens
#define MSG_KEY 0x1233

int pid_sched;  // pid do sched para mandar mensagens
int n_filas;    // numero de filas round-robin
int msg_id;     // id da fila de mensagens

void exit_prog() {
  // frees necessarios
  msgctl(msg_id, IPC_RMID, NULL); // free na fila de mensagens
  kill(pid_sched, SIGKILL); // mata o sched
}

int main(){
  signal(SIGSEGV, exit_prog);  // define de tratamento para segfault
  signal(SIGUSR1, exit_prog);  // define o tratamento de erros gerais

  char command[100];

  // Cria a fila de mensagens
  msg_id = msgget(MSG_KEY, IPC_CREAT | 0777);
  if(msg_id < 0) {
    fprintf(stderr, "Erro: falha ao criar a fila de mensagens.\n");
    exit(EXIT_FAILURE);
  }

  // declaracao da struct mensagem
  mensagem mensagem_main;
  mensagem_main.pid = getpid();

  while (true) {
    printf(">shell_sched: ");
    if(fgets(command, sizeof(command), stdin) == NULL) { // fgets le toda a linha
      fprintf(stderr, "Erro: falha ao ler o comando.\n");

      kill(getpid(), SIGUSR1);   // rotina de saida
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

      kill(pid_sched, SIGUSR1);                                       // pede as informacoes para o shed
      msgrcv(msg_id, &mensagem_main, sizeof(mensagem_main), 0, 0);    // recebe a resposta (espera ate receber)
      printf("%s\n", mensagem_main.msg);                              // printa as filas

      continue;
    }
    char *token = strtok(command, " ");
    if(token != NULL) {
      if(strcmp(token, user_s) == 0) {
        char* n = strtok(NULL, " ");  // numero de filas
        n_filas = atoi(n);            // casting pra int

        // salva o pid do sched
        pid_sched = fork();
        if (pid_sched == 0) {

          // Casting para char* das ids
          char arg_msg[20];
          sprintf(arg_msg,"%d",msg_id);

          // chama o executavel do "sched.c"
          execl("source/sched","sched",n,arg_msg,NULL);

          fprintf(stderr, "Erro: falha ao executar o comando 'execl'.\n");
          
          // frees necessarios
          kill(getpid(), SIGUSR1);   // rotina de saida
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

        kill(pid_sched, SIGUSR2);   // avisa o sched q ha um novo processo
        strcpy(mensagem_main.msg, pr);  // copia a prioridade pro buffer de mensagem
        msgsnd(msg_id, &mensagem_main, sizeof(mensagem_main), 0); // envia a prioridade pro shed

        /*TODO - corrigir essa parte
        
        voltar_para_o_inicio = true;
        
        */

        continue;
      }
    }
    
    fprintf(stderr,"Erro: Comando não definido.\n");

    // frees necessarios
    kill(getpid(), SIGUSR1);   // rotina de saida
    exit(EXIT_FAILURE); // exit(1) | return 1
  }


  kill(getpid(), SIGUSR1);   // rotina de saida

  return EXIT_SUCCESS; // return 0
}
