#include <stdio.h>
#include <string.h>
#include "../headers/utils.h"

int main(){

  config_struct conf;
  memset(&conf, 0, sizeof(conf));
  readconf(&conf, "ws.conf");
  print_config_struct(&conf);
  return 0; 

}
