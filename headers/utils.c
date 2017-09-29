#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "debug.h"

void readconf(config_struct* conf, char *file_name){

  FILE *fp;
  int file_line_len, content_index;
  char line[MAXFILEBUFFER];
  char buffer[MAXFILEBUFFER];
  char *temp_char_ptr; 
  if((fp = fopen(file_name, "r")) <= 0){
      perror("Error In Opening Config File: ");
  }

  while(fgets(line, sizeof(line), fp)){
    // Ignore in case of comments or empty lines
    if((strncmp(line, "#", 1) == 0) || (strncmp(line, "\n", 1) == 0) ) continue; 

    file_line_len = strlen(line);

    if(line[file_line_len - 1] == '\n') line[file_line_len - 1] = '\0';
      
    DEBUGS("Line:");
    DEBUGS(line);

    memset(buffer, 0, sizeof(buffer));

    get_second_string(&temp_char_ptr,line, ' ');
    strcpy(buffer, temp_char_ptr); 

    if(strncmp(line, LISTEN_PORT_CONF, LISTEN_PORT_CONF_LEN) == 0) {
      
      conf->port_number = atoi(buffer);

    } else if (strncmp(line, DOCUMENT_ROOT_CONF, DOCUMENT_ROOT_CONF_LEN) == 0) {
      
      conf->doc_root = strdup(buffer);
      remove_first_last_char(&conf->doc_root);

    } else if (strncmp(line, DIRECTORY_INDEX, DIRECTORY_INDEX_LEN) == 0) {
      
      conf->doc_index = strdup(buffer); 

    } else if (strncmp(line, KEEP_ALIVE_TIME_CONF, KEEP_ALIVE_TIME_CONF_LEN) == 0){
    
      conf->keep_alive_time = atoi(buffer);

    } else if (strncmp(line, CONTENT_TYPE_START, CONTENT_TYPE_START_LEN) == 0) {
      
      content_index = conf->content_type_index;
      conf->content_type[content_index] = (content_type_struct *) malloc(sizeof(content_type_struct));
      get_first_string(&conf->content_type[content_index]->extension, line, ' ');
      get_second_string(&conf->content_type[content_index]->type, line, ' ');
      conf->content_type_index++;

    }
  }

  fclose(fp);
}


void remove_first_last_char(char **str){
  int len = strlen(*str);

  DEBUGS("Remove First last Char");
  DEBUGS(*str); 
  // Remove the last character
  (*str)[len - 1] = '\0';

  DEBUGS(*str); 
  // Remove the fist character
  (*str) = (*str) + 1;

}

void get_second_string(char **dest, char *src, char delim){

  char * delim_char = strchr(src, delim);
  *dest = strdup(delim_char + 1);

}

void get_first_string(char **dest, char *src, char delim){
  
  char *delim_char = strchr(src, delim);
  int offset = delim_char - src;
  *dest = strndup(src, offset);

}

void print_config_struct(config_struct * conf){
  
  int i;
  DEBUGS("Printing Conf Struct"); 
  DEBUGS("Port Number");
  DEBUGN(conf->port_number);
  
  DEBUGS("Doc Root");
  DEBUGS(conf->doc_root);
  
  DEBUGS("Doc Index");
  DEBUGS(conf->doc_index);

  DEBUGS("Keep Alive Time");
  DEBUGN(conf->keep_alive_time);

  DEBUGS("Content-Type");

  for(i = 0; i < conf->content_type_index; i++){
    DEBUGSS(conf->content_type[i]->extension, conf->content_type[i]->type);
  } 

}
