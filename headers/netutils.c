#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utils.h"
#include "netutils.h"
#include "debug.h"

int getsocket(config_struct * conf){
  int sockfd;
  struct sockaddr_in sin;
  int yes = 1;
  socklen_t remotelength;

  memset(&sin, 0, sizeof(sin));

  sin.sin_family = AF_INET;
  sin.sin_port = htons(conf->port_number);
  sin.sin_addr.s_addr = INADDR_ANY;

  checkforerror(sockfd = socket(AF_INET, SOCK_STREAM, 0), "Unable to create socket"); 

  // Avoiding the "Address Already in use" error message
  if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0 ){
    perror("Unable to set so_reuseaddr:");
    exit(1);
  }

  if ( bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0){
    perror("Unable to bind the socket:");
    exit(1);
  }

  if (listen(sockfd, MAXCONNECTION) < 0) {
    perror("Unable to call listen on socket:");
    exit(1);
  }

  return sockfd;

}

ssize_t processrequest(char * src_buff, char * dest_buff, ssize_t src_buff_len){ 
  int i;
  ssize_t dest_buff_size = 0;
  ssize_t dest_content_length = 0;
  char file_name[MAXFILENAMEPATH];
  req_struct rq;  
  memset(&rq, 0, sizeof(rq));

  getreqstruct(&rq, src_buff, src_buff_len);
  if (strncmp(rq.method, GET_HEADER, GET_HEADER_LEN) == 0){
     strcpy(dest_buff, HTTP10_RES_OK);
     dest_buff_size += HTTP10_RES_OK_LEN;
  }
  
  
  dest_buff[dest_buff_size++] = '\n';
  dest_buff[dest_buff_size++] = '\n';
  dest_buff[dest_buff_size++] = '\n';
 
  return dest_buff_size;
  /*char *temp_string = "Server Replied: ";*/
  /*ssize_t temp_string_len = strlen(temp_string);*/

  /*memcpy(dest_buff, temp_string, temp_string_len);*/
  /*memcpy(dest_buff + temp_string_len + 1, src_buff, src_buff_len);*/
  /*return temp_string_len + src_buff_len + 1;*/
}

void getreqstruct(req_struct *rq, char *buff, ssize_t buff_len){

  char *temp_char, *temp_str, *temp_buff;
  ssize_t temp_buff_len, temp_len;
  int i, index;

  temp_buff = (char *) malloc((buff_len + 1)*sizeof(char));
  memset(temp_buff, 0, sizeof(temp_buff));
  // copying the entire buffer
  memcpy(temp_buff, buff, buff_len);
  temp_buff_len = buff_len;
  
  /*for(i = 0; i < HTTP_REQ_END_LEN; i++)*/
    /*temp_buff[--temp_buff_len] = '\0';*/
  index = 0;
  while((temp_char = strstr(temp_buff + index, HTTP_REQ_DELIM))){
    
    // Length of single line
    temp_len = temp_char - temp_buff - index;

    if (temp_len == 0) break;

    temp_str = (char *) malloc((temp_len+1) *sizeof(char));
    memset(temp_str, 0, sizeof(temp_str));
    memcpy(temp_str, temp_buff + index, temp_len);
    temp_str[temp_len] = '\0';

    // Skip the entire string which whas just processed
    index += temp_len;
    // Skip the HTTP_REQ_DELIM
    index += HTTP_REQ_DELIM_LEN;

    DEBUGSS("Part of Request", temp_str);
    
    if(strncmp(temp_str, GET_HEADER, GET_HEADER_LEN) == 0) fillgetvars(temp_str, rq);
    free(temp_str);
  }

  free(temp_buff);
  debugreqstruct(rq);
}

void debugreqstruct(req_struct * rq){
  DEBUGS("Printing request struct");
  DEBUGSS("\tMethod", rq->method);
  DEBUGSS("\tURI", rq->uri);
  DEBUGSS("\tHTTP Version", rq->http_version);
  DEBUGSS("\tKeep Connection Alive", rq->connection_keep_alive);
}
void fillgetvars(char * str, req_struct *rq){
 
  char *temp1_str, *temp2_str;
  int uri_len;
  rq->method = (char *) malloc((GET_HEADER_LEN + 1) * sizeof(char));
  memcpy(rq->method, GET_HEADER, GET_HEADER_LEN);
  rq->method[GET_HEADER_LEN] = '\0';
  
  // Last occurance of space handling cases when file names have spaces
  temp1_str = strrchr(str,' '); 

  // Pointer just after the first space
  temp2_str = str + GET_HEADER_LEN + 1; 

  uri_len = temp1_str - temp2_str;
  rq->uri = strndup(temp2_str, uri_len);
  rq->http_version = strdup(temp1_str + 1);
}

void sendresponse(int socket, char *buff, ssize_t buff_len){
  ssize_t resbytes;
  resbytes = send(socket, buff, buff_len, 0);

  checkforerror(resbytes, "Unable to reply data");

  // TODO: Case when partial response is sent
  checkforerror(resbytes - buff_len, "Unable to send complete data");

}

int checkifget(char * buff){
  if(strncmp(buff, GET_HEADER, GET_HEADER_LEN) == 0) return TRUE;
  return FALSE;
}
