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

// Biblioteca da shared memory
#include "sys/shm.h"

// Biblioteca de fila de mensagem
#include "sys/msg.h"

int pid = 0;
bool voltar_para_o_inicio = false;

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);                          // numero de filas round-robins
    int shm_id_round_robins = atoi(argv[2]);        // id da shm das filas roud-robins
    int shm_id_finished_processes = atoi(argv[3]);  // id da shm da fila de precessos finalizados
    int msg_id = atoi(argv[4]);                     // id da fila de mensagem

    // le o ponteiro da array de filas round-robin
    Queue** round_robins = (Queue**) shmat(shm_id_round_robins, (char*) 0, 0);
    if(round_robins == (int *) -1) {
        fprintf(stderr, "Erro: falha ao ler da memória compartilhada (attach).\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        round_robins[i] = new_queue();
    }

    /*TODO - logica do dettach da shm. Temos que dar shmdt toda vez que terminarmos de usar?*/

    Queue* finished_processes = (Queue*) shmat(shm_id_finished_processes, (char*) 0, 0);
    if(finished_processes == (int*) -1) {
        fprintf(stderr, "Erro: falha ao ler da memória compartilhada (attach).\n");
        exit(EXIT_FAILURE);
    }

    /*TODO - logica do dettach da shm. Temos que dar shmdt toda vez que terminarmos de usar?*/

    // declaracao da struct mensagem
    mensagem mensagem_shed;
    mensagem_shed.pid = getpid();

    /*Teste da troca de mensagens

    printf("Scheduler: vou ler uma mensagem...\n");
    msgrcv(msg_id, &mensagem_shed, sizeof(mensagem_shed), 0, 0);
    printf("Mensagem recebida de 'main.c': %s\n", mensagem_shed.msg);
    exit(0);
    
    */

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

            /*TODO - corrigir essa parte
            
            p->time_remaining -= 5;
            if (p->time_remaining == 0) {
                enqueue(finished_processes, p);
            } else {
                enqueue(round_robins[i], p);
            }

            if (!is_empty(round_robins[i])) i -= 1;      
            
            */
        }
    }
}