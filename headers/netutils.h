#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "utils.h"

#ifndef NETUTILS_H
#define NETUTILS_H

#define MAXCONNECTION 10
#define MAXREQRESBUFFER 1000000
#define MAXFILENAMEPATH 1024
#define MAX_TEMPLATES 20
#define GET_HEADER "GET"
#define GET_HEADER_LEN strlen(GET_HEADER)
#define GET_URI_ROOT "/"

#define HTTP_REQ_CONNECTION_PARAM "Connection: "
#define HTTP_REQ_CONNECTION_PARAM_LEN strlen(HTTP_REQ_CONNECTION_PARAM)

#define HTTP_REQ_CONNECTION_KEEP_ALIVE "Keep-alive"
#define HTTP_REQ_CONNECTION_KEEP_ALIVE_LEN strlen(HTTP_REQ_CONNECTION_KEEP_ALIVE)

#define HTTP_REQ_END "\r\n\r\n"
#define HTTP_REQ_END_LEN strlen(HTTP_REQ_END)

#define HTTP_REQ_DELIM "\r\n"
#define HTTP_REQ_DELIM_LEN strlen(HTTP_REQ_DELIM)

#define HTTP_RES_END "\r\n\r\n"
#define HTTP_RES_END_LEN strlen(HTTP_RES_END)

#define HTTP_RES_DELIM "\r\n"
#define HTTP_RES_DELIM_LEN strlen(HTTP_RES_DELIM)

#define HTTP_RES_CONTENT_TYPE "Content-Type: " //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_TYPE_LEN strlen(HTTP_RES_CONTENT_TYPE)

#define HTTP_RES_CONTENT_LENGTH "Content-Length: " //SPACE IS IMPORTANT
#define HTTP_RES_CONTENT_LENGTH_LEN strlen(HTTP_RES_CONTENT_LENGTH)

#define HTTP_RES_CONNECTION "Connection: "
#define HTTP_RES_CONNECTION_KEEP_ALIVE "keep-alive"

#define HTTP_GENERIC_TEMPLATE "<html><body>%s</body></html>"

#define HTTP_RES_OK "HTTP/1.1 200 OK" 
#define HTTP10_RES_OK_LEN strlen(HTTP_RES_OK)

#define HTTP_RES_BAD_REQ "HTTP/1.1 400 Bad Request"
#define HTTP_RES_BAD_REQ_TYPE "text/html"

#define HTTP_RES_404_FLAG "404 Flag"
#define HTTP_RES_404 "HTTP/1.1 404 Not Found"
#define HTTP_RES_404_TYPE "text/html"
#define HTTP_RES_404_FILE_TEMPLATE "404 Not Found Reason URL does not exist: %s"

#define HTTP_BAD_REQ_FLAG "Bad Request"
#define HTTP_BAD_REQ_FLAG_LEN strlen(HTTP_BAD_REQ_FLAG)

#define HTTP_BAD_REQ_INVALID_METHOD_FLAG HTTP_BAD_REQ_FLAG" Invalid Method"
#define HTTP_BAD_REQ_INVALID_URI_FLAG HTTP_BAD_REQ_FLAG" Invalid URI"
#define HTTP_BAD_REQ_INVALID_TYPE_FLAG HTTP_BAD_REQ_FLAG" Invalid type"

#define HTTP_RES_BAD_REQ_TEMPLATE "400 Bad Request Reason: %s"
#define HTTP_RES_BAD_REQ_INVALID_METHOD_TEMPLATE "Invalid Method: %s"
#define HTTP_RES_BAD_REQ_INVALID_URI_TEMPLATE "Invalid URL: %s"
#define HTTP_RES_BAD_REQ_INVALID_HTTP_TEMPLATE "Invalid HTTP-Version: %s"
#define HTTP_RES_BAD_REQ_INVALID_FILE_TYPE_TEMPLATE "Invalid file-type: %s"

struct req_struct{

  char *method;
  char *uri;
  char *connection;
  char *http_version;

};

typedef struct req_struct req_struct;

struct res_struct{

  char *status_line;
  size_t content_length;
  char *content_type;
  u_char *body;

};

typedef struct res_struct res_struct;

int get_socket(config_struct *);
ssize_t process_request(char *, u_char *, ssize_t, config_struct *);
void send_response(int, char *, ssize_t);
void get_req_struct(req_struct *, char *, ssize_t);

ssize_t update_buff(char *, char *, ssize_t);
ssize_t update_buff_with_delim(char *, char *, ssize_t);
ssize_t update_buff_as_delim(char *);

void fill_req_struct(char *, req_struct *);
void fill_error_bad_req_res_struct(req_struct *, res_struct *, config_struct *, char *);
void fill_get_res_struct(req_struct *, res_struct *, config_struct *);
size_t fill_res_body(FILE *, u_char **);


ssize_t res_struct_to_buff(res_struct *, u_char *);
void rest_struct_to_buff(res_struct*, char *);

void free_res_struct(res_struct *);
void free_req_struct(req_struct *);
void debug_res_struct(res_struct *);
void debug_req_struct(req_struct *);

#endif
