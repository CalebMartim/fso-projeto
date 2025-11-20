#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "unistd.h"

// Structs que organizam as informações dos processos
// e filas round-robin #include "process.h"
#include "queue.h"

// Header de funções auxiliares:
#include "aux.h"

// Bibliotecas de mecanismos de comunicação
#include "sys/ipc.h"
#include "sys/types.h"

// Biblioteca de fila de mensagem
#include "sys/msg.h"

// Biblioteca de sinais
#include "signal.h"

// Variável booleana utilizada no caso em que um processo esteja sendo executado
// e um outro processo entra no escalonador que possa ter maior prioridade do 
// que está sendo executado
bool voltar_para_o_inicio = false;

int n; // numero de filas round-robins

Queue **round_robins; // Filas do escalonador
Queue *finished_processes; // Fila de processos finalizados

// declaração da struct mensagem
mensagem mensagem_sched;

Process *processo_atual; // Manterá informações do processo sendo executado no momento
bool process_running = false; // Variável booleana que diz se há um processo rodando ou não 

int msg_id; // id da fila de mensagens

// exit_scheduler
void exit_sched() {

  bool printou_algo = false; // Usado apenas por questão de apresentação

  // Passa pela lista de processos finalizados:
  if (!is_empty(finished_processes)) {
    printou_algo = true;

    printf("\nProcessos finalizados:\n");

    Node *atual = finished_processes->head;
    while (atual != NULL) {
      Process *proc = atual->proc;
      printf("\tProcesso: %d, prioridade: %d, turnaround: %lds\n", proc->pid, proc->priority, proc->turnaround);
      atual = atual->nxt;
    }
  }

  // Processo atual:
  if (process_running) { 
    printou_algo = true;
    printf("Processo interrompido: %d, prioridade: %d\n", processo_atual->pid, processo_atual->priority);
  }

  bool all_vazio = true; // Verifica se todas as filas estão ou não vazias
  for (int i = 0; i < n; i++)
    all_vazio &= is_empty(round_robins[i]);

  if (!all_vazio) {
    printou_algo = true;
    printf("Processos não finalizados:\n");
    for (int pr = 0; pr < n; ++pr) {
      if (!is_empty(round_robins[pr])) {
        printf("\tFila de prioridade %d\n", pr + 1);
        Node *atual = round_robins[pr]->head;

        while (atual != NULL) {
          Process *proc = atual->proc;
          printf("\t\tProcesso: %d\n", proc->pid);
          atual = atual->nxt;
        }
      }
    }
  }

  // Feito por questão de apresentação
  if (printou_algo) {
    printf("\n>shell_sched: ");
    fflush(stdout);
  }

  // Frees necessarios
  for (int i = 0; i < n; i++)
    free_queue(round_robins[i]); // Cada processo no escalonador
  free_queue(finished_processes); // Cada processo já finalizado
  free(round_robins); // Array que guardava as filas
  if (process_running)
    free(processo_atual);

  exit(EXIT_SUCCESS);
}

// list_scheduler
void info_sched() {
  bool printou_algo = false; // Usado apenas por questão de apresentação

  // Processo atual:
  if (process_running) {
    printou_algo = true;
    printf("\nProcesso sendo executado: %d, prioridade: %d\n", processo_atual->pid, processo_atual->priority);
  }

  // Processos nas filas:
  for (int pr = 0; pr < n; ++pr) {
    if (!is_empty(round_robins[pr])) {
      printou_algo = true;
      printf("Fila de prioridade %d\n", pr + 1);
      Node *atual = round_robins[pr]->head;

      while (atual != NULL) {
        Process *proc = atual->proc;
        printf("\tProcesso: %d\n", proc->pid);
        atual = atual->nxt;
      }
    }
  }

  // Feito por questão de apresentação
  if (printou_algo) {
    printf(">shell_sched: ");
    fflush(stdout);
  }
}

// execute_process
void add_proc() {
  strcpy(mensagem_sched.msg, ""); // limpa o buffer de mensagem
  // recebe a prioridade do processo (enviada pela main)
  msgrcv(msg_id, &mensagem_sched, sizeof(mensagem_sched.msg), 0, 0); 

  int pid_new_process = fork();
  if (pid_new_process == 0) { // Processo filho 
    kill(getpid(), SIGSTOP); // espera receber SIGCONT

    execl("bin/proc_exec", "proc_exec", (char *) 0);
    fprintf(stderr, "Erro: falha ao executar o comando 'execl'.\n");

  } else {  // Processo pai
    Process *proc = new_process(pid_new_process, atoi(mensagem_sched.msg));

    // Coloca as informações deste processo na sua devida fila round-robin:
    int res = push(round_robins[proc->priority - 1], proc);
    if (res) {
      fprintf(stderr, "Erro ao colocar processo na fila round_robin\n");
    }

    // Para o escalonador verificar se este processo tem prioridade maior
    // do que o que está sendo executado:
    voltar_para_o_inicio = true;
  }
}

int main(int argc, char *argv[]) {
  mensagem_sched.pid = getpid();

  n = atoi(argv[1]);      // número de filas round-robins
  msg_id = atoi(argv[2]); // id da fila de mensagem

  round_robins = (Queue **)malloc(sizeof(Queue *) * n); // cria os arrays de filas rr
  //processo_default = new_process(2147483647, -1);
  processo_atual = new_process(2147483647, -1);

  signal(SIGINT, (void *)exit_sched);  // rotina de saida
  signal(SIGUSR1, (void *)info_sched); // rotina de listar os processos
  signal(SIGUSR2, (void *)add_proc);   // rotina de adicionar novo processo

  for (int i = 0; i < n; i++) {
    round_robins[i] = new_queue();
    if (round_robins[i] == NULL) {
      fprintf(stderr, "Erro em inicializar filas round robin. Finalizando escalonador\n");
      kill(getpid(), SIGUSR1);
    }
  }

  finished_processes = new_queue();
  if (finished_processes == NULL) {
    fprintf(stderr, "Erro em inicializar fila de processos finalizados. Finalizando escalonador\n");
    kill(getpid(), SIGUSR1);
  }

  // Loop principal do escalonador
  while (true) {
    for (int pr = 0; pr < n; pr++) {
      if (voltar_para_o_inicio) {
        pr = -1; // reseta o for
        voltar_para_o_inicio = false;
        continue;
      }

      // executamos todos os processos de prioridade == pr
      if (!is_empty(round_robins[pr])) {
        processo_atual = pop(round_robins[pr]);
        if (processo_atual == NULL) {
          fprintf(stderr, "Falha em carregar processo para execução. Fechando escalonador\n");
          kill(getpid(), SIGUSR1);
        }
        process_running = true;

        kill(processo_atual->pid, SIGCONT); // Acorda o processo_atual

        // quantum 4s, signal proof
        int falta_dormir = 4;
        do {
          falta_dormir = sleep(falta_dormir);
        } while (falta_dormir);

        // Status do processo após o quantum
        int status = waitpid(processo_atual->pid, (int*)0, WNOHANG);

        if (status == processo_atual->pid) { // O processo terminou a sua execução
          process_running = false;
          processo_atual->turnaround = time(NULL) - processo_atual->time_init; // Cálculo do turnaround
          int res = push(finished_processes, processo_atual); // Coloca na fila de processos finalizados
          if (res) {
            fprintf(stderr, "Erro ao colocar processo em finished_processes\n");
          }
        } else { // O processo ainda não terminou a sua execução
          kill(processo_atual->pid, SIGSTOP);     // para o processo
          process_running = false;
          int res = push(round_robins[pr], processo_atual); // coloca no final da fila
          if (res) {
            fprintf(stderr, "Erro ao colocar processo na fila round_robin\n");
          }
        }
        if (!is_empty(round_robins[pr])) 
          pr--; // Nermanece na prioridade pr na próxima iteração
      }
    }
  }
}
