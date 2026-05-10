#include "cell.h"

char *cellGetName(char kind) {
  switch (kind) {
    case 0:
      return "Nothingness";
    case 1:
      return "Sand";
    default:
      return "Unknown";
  }
}
