#include "stdio.h"

// Processo que ficara ocupando a CPU

int main() {
    for(int i = 0; i < 20 * 1e8; i++);

    return 0;
}