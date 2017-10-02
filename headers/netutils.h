#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "utils.h"

#ifndef NETUTILS_H
#define NETUTILS_H

#define MAXCONNECTION 10
#define MAXREQRESBUFFER 1024
#define MAXFILENAMEPATH 1024

#define GET_HEADER "GET\0"
#define GET_HEADER_LEN strlen(GET_HEADER)


#define HTTP_REQ_END "\r\n\r\n\0"
#define HTTP_REQ_END_LEN strlen(HTTP_REQ_END)

#define HTTP_REQ_DELIM "\r\n\0"
#define HTTP_REQ_DELIM_LEN strlen(HTTP_REQ_DELIM)

#define HTTP_RES_END "\r\n\r\n\0"
#define HTTP_RES_END_LEN strlen(HTTP_RES_END)

#define HTTP_RES_DELIM "\r\n\0"
#define HTTP_RES_DELIM_LEN strlen(HTTP_RES_DELIM)

#define HTTP10_RES_OK "HTTP/1.0 200 OK\0" 
#define HTTP10_RES_OK_LEN strlen(HTTP10_RES_OK)

#define HTTP_RES_CONTENT_TYPE "Content-Type: \0" //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_TYPE_LEN strlen(HTTP_RES_CONTENT_TYPE)

#define HTTP_RES_CONTENT_LENGTH "Content-Length: \0" //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_LENGTH_LEN strlen(HTTP_RES_CONTENT_LENGTH)

#define HTTP_RES_SAMPLE_CONTENT_TYPE "text/html\0"
#define HTTP_RES_SAMPLE_CONTENT_TYPE_LEN strlen(HTTP_RES_SAMPLE_CONTENT_TYPE)

#define HTTP_RES_SAMPLE_CONTENT "Hello World\0"
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
  u_char *body;
};

typedef struct res_struct res_struct;

int getsocket(config_struct *);
ssize_t processrequest(char *, char *, ssize_t);
void sendresponse(int, char *, ssize_t);
void getreqstruct(req_struct *, char *, ssize_t);
void fillgetreqstruct(char *, req_struct *);
void fillgetresstruct(req_struct *, res_struct *);
ssize_t updatebuff(char *, char *, ssize_t);
ssize_t updatebuffwithdelim(char *, char *, ssize_t);
ssize_t updatebuffasdelim(char *);

void debugresstruct(res_struct *);
void debugreqstruct(req_struct *);
#endif
