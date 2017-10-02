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

#define HTTP_RES_DELIM "\r\n"
#define HTTP_RES_DELIM_LEN 2

#define HTTP_RES_CONTENT_TYPE "Content-Type: " //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_TYPE_LEN 14

#define HTTP_RES_CONTENT_LENGTH "Content-Length: " //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_LENGTH_LEN 16

#define HTTP_RES_SAMPLE_CONTENT_TYPE "text/html"
#define HTTP_RES_SAMPLE_CONTENT_TYPE_LEN 9

#define HTTP_RES_SAMPLE_CONTENT "Hello World"
#define HTTP_RES_SAMPLE_CONTENT_LEN "11"

struct req_struct{
  char *method;
  char *uri;
  char *connection_keep_alive;
  char *http_version;
};

typedef struct req_struct req_struct;

struct res_struct{

  char *status_line;
  char *content_length;
  char *content_type;
  char *body;
};

typedef struct res_struct res_struct;

int getsocket(config_struct*);
ssize_t processrequest(char *, char *, ssize_t);
void sendresponse(int, char *, ssize_t);
void getreqstruct(req_struct *, char *, ssize_t);
void debugreqstruct(req_struct *);
void fillgetreqstruct(char *, req_struct *);
ssize_t updatebuff(char *, char *, ssize_t);
ssize_t updatebuffwithdelim(char *, char *, ssize_t);
ssize_t updatebuffasdelim(char *);
#endif
