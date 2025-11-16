#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

#include "process.h"
#include "queue.h"

// Header de funções auxiliares:
#include "aux.h"

// Bibliotecas de mecanismos de comunicacao
#include "sys/types.h"
#include "sys/ipc.h"

// Biblioteca de fila de mensagem
#include "sys/msg.h"

// Biblioteca de sinais
#include "signal.h"

/*TODO - corrigir essa parte

bool voltar_para_o_inicio = false;  

*/
int n;                              // numero de filas round-robins

Queue* round_robins[3];
Queue* finished_processes;

// declaracao da struct mensagem
mensagem mensagem_shed;

int msg_id;     // id da fila de mensagens

// exit_scheduler
void exit_sched() {
    /*TODO - enviar como mensagem para main os processos terminados e filas*/

    for(int i = 0; i < n; i++) free_queue(round_robins[i]);
    free_queue(finished_processes);
}

// list_scheduler
void info_shed() {
    /*TODO - enviar como mensagem as filas e o processo sendo executado no momento*/
    strcpy(mensagem_shed.msg, "");  // limpa o buffer de mensagem

}

// execute_process
void add_proc() {
    /*TODO - criar processo e adicionar na fila*/
    strcpy(mensagem_shed.msg, "");  // limpa o buffer de mensagem
    msgrcv(msg_id, &mensagem_shed, sizeof(mensagem_shed), 0, 0);    // recebe a prioridade da main


}

int main(int argc, char* argv[]) {
    mensagem_shed.pid = getpid();

    n = atoi(argv[1]);                          // numero de filas round-robins
    msg_id = atoi(argv[2]);                     // id da fila de mensagem

    signal(SIGKILL, exit_sched);                // rotina de saida
    signal(SIGUSR1, info_shed);                 // rotina de listar os processos
    signal(SIGUSR2, add_proc);                  // rotina de adicionar novo processo

    for (int i = 0; i < n; i++) round_robins[i] = new_queue();
    finished_processes = new_queue();

    /*Teste da troca de mensagens

    printf("Scheduler: vou ler uma mensagem...\n");
    msgrcv(msg_id, &mensagem_shed, sizeof(mensagem_shed), 0, 0);
    printf("Mensagem recebida de 'main.c': %s\n", mensagem_shed.msg);
    exit(0);
    
    */

    while(true) {}
   
    /*TODO - corrigir essa parte
    while (true) {
        for (int i = 0; i < n; ++i) {
            // Sinal para matar scheduler
            
            if (voltar_para_o_inicio) {
                voltar_para_o_inicio = false;
                i = -1;
                continue;
            }

            if (is_empty(round_robins[i])) continue;

            // Processa o cara no início
            Process *p = dequeue(round_robins[i]);

            sleep(4); // quantum de 4s

            
            p->time_remaining -= 5;
            if (p->time_remaining == 0) {
                enqueue(finished_processes, p);
            } else {
                enqueue(round_robins[i], p);
            }

            if (!is_empty(round_robins[i])) i -= 1;      
            
        }
    }
    */
}