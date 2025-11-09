#ifndef AUX_H
#define AUX_H

#include "string.h"

int string_to_int(char* s) {
  int len = strlen(s);
  int pot_10 = 1;
  int ret = 0;
  while (--len >= 0) {
    ret += pot_10 * (s[len] - '0');
    pot_10 *= 10;
  }
  return ret; 
}

#endif