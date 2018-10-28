#include "display.h"

void displayLinterError(char* fileName, unsigned int lineNb, char* line, unsigned int errorIndex){
  printf("%sFile : %s : %d%s\n", BACKGROUND_MAGENTA, fileName, lineNb, BACKGROUND_DEFAULT);
  printf("%s\n", line);
  for(unsigned int i = 0; i < (errorIndex - 1); i++){
    printf("~");
  }
  printf("%s%s^%s\n", FORMAT_RESET,COLOR_RED_BOLD, FORMAT_RESET);
}
