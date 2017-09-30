#include <stdio.h>
#include <sys/types.h>
#include "utils.h"
#ifndef NETUTILS_H
#define NETUTILS_H

#define MAXCONNECTION 10
#define MAXREQRESBUFFER 1024
#define MAXFILENAMEPATH 1024

#define GET_HEADER "GET"
#define GET_HEADER_LEN 3

#define NEW_LINE_INT 10
#define HTTP_REQ_END "\r\n\r\n"
#define HTTP_REQ_END_LEN 4
#define HTTP_REQ_DELIM "\r\n"
#define HTTP_REQ_DELIM_LEN 2

#define HTTP10_RES_OK "HTTP/1.0 200 OK" 
#define HTTP10_RES_OK_LEN 15
#define HTTP_RES_DELIM '\n'
#define HTTP_RES_DELIM_LEN 1
struct req_struct{
  char *method;
  char *uri;
  char *connection_keep_alive;
  char *http_version;
};

typedef struct req_struct req_struct;

int getsocket(config_struct*);
ssize_t processrequest(char *, char *, ssize_t);
void sendresponse(int, char *, ssize_t);
void getreqstruct(req_struct *, char *, ssize_t);
void debugreqstruct(req_struct *);
void fillgetvars(char *, req_struct *);
#endif
