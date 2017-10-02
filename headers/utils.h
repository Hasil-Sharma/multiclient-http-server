#include <stdio.h>
#include <stdlib.h>
#ifndef UTILS_H
#define UTILS_H

#define TRUE 1
#define FALSE 0

#define MAXFILEBUFF 256
#define MAXCONTENTTYPE 20
#define LISTEN_PORT_CONF "Listen"
#define LISTEN_PORT_CONF_LEN 6
#define DOCUMENT_ROOT_CONF "DocumentRoot"
#define DOCUMENT_ROOT_CONF_LEN 12
#define DIRECTORY_INDEX "DirectoryIndex"
#define DIRECTORY_INDEX_LEN 14
#define KEEP_ALIVE_TIME_CONF "Keep-Alive-Time"
#define KEEP_ALIVE_TIME_CONF_LEN 15
#define CONTENT_TYPE_START "."
#define CONTENT_TYPE_START_LEN 1

typedef struct content_type_struct content_type_struct;

struct config_struct {
  int port_number;
  char* doc_root;
  char* doc_index;
  content_type_struct *content_type[MAXCONTENTTYPE];
  int keep_alive_time;
  int content_type_index;
};

struct content_type_struct {
  char *extension;
  char *type;
};

typedef struct config_struct config_struct;

void read_conf(config_struct *, char *);
void get_second_string(char **, char *, char);
void get_first_string(char **, char *, char);
void get_extension(char **, char *);
void print_config_struct(config_struct *);
void remove_first_last_char(char **);
void checkforerror(int, char *);

#endif


