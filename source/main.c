#include "stdio.h"

#define true 1
#define false 0

char *quit = "quit";

int len(char *s) {
  int n = 0;
  while (s[n] != '\0')
    ++n;
  return n;
}

int verify(char *aux) {
  int i = 0;
  while (aux[i] != '\0' && aux[i] == quit[i]) 
    ++i;
  if (i == len(quit) && aux[i] == '\0') 
    return 0;

  return 1;
}

int main(){
  char aux[100];
  while (true) {
    printf(">shell_sched: ");
    scanf("%s", aux);
    printf("%d\n", verify(aux));
    if (verify(aux) == 0) 
      return 0;
  }
}
