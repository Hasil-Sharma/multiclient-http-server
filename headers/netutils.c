#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "utils.h"
#include "netutils.h"
#include "debug.h"

int get_socket(config_struct * conf){

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


ssize_t update_buff(char * buff, char * content, ssize_t content_length){

  memcpy(buff, content, content_length);
  return content_length;

}


ssize_t update_buff_as_delim(char *buff){

  return update_buff(buff, HTTP_RES_DELIM, HTTP_RES_DELIM_LEN);

}

ssize_t update_buff_with_delim(char *buff, char *content, ssize_t content_length){

  content_length = update_buff(buff, content, content_length);
  content_length += update_buff_as_delim(buff + content_length); 
  return content_length;

}

int check_extension_type(char *extension, char **type, config_struct *conf){

  int i;
  int flag = FALSE;
  for(i = 0; i < MAXCONTENTTYPE; i++){ 

    if(!conf->content_type[i]) break; // in case of NULL
    if(strcmp(extension, conf->content_type[i]->extension) == 0){
      flag = TRUE;
      *type = strdup(conf->content_type[i]->type);
      break;
     }

  }

  return flag;
}

void fill_get_res_struct(req_struct *rq, res_struct *rs, config_struct *conf){

  char *file_name;
  char *extension;
  char *type;

  char file_path[MAXFILENAMEPATH];
  FILE *fp;
  
  if (strcmp(rq->uri, GET_URI_ROOT) == 0) file_name = strdup(conf->doc_index); 
  else file_name = strdup(rq->uri + 1); // +1 to skip the initial / and making it consistent with doc_index
  
  get_extension(&extension, file_name);

  if (check_extension_type(extension, &type, conf)){

    rs->content_type = strdup(type);

    if (sprintf(file_path, "%s/%s", conf->doc_root, file_name) < 0){
      perror("Error in making file path string:");
      exit(1);  
    }
    
    if (!(fp = fopen(file_path, "rb"))) {
      perror("Error in opening file path:");
      exit(1);
    }

    // TODO: Test it works fine
    rs->content_length = fill_res_body(fp, &(rs->body)); 
    rs->status_line = strdup(HTTP10_RES_OK); 

    fclose(fp);
    free(extension);
    free(file_name);

  } else {
    // Case when unsupported file type is requested
    fprintf(stderr, "Unsupported file type Not Implemented %s: %s", extension, strerror(errno));
    exit(1);
  }
}

size_t fill_res_body(FILE * fp, u_char ** buff){

  size_t buff_size;
  fseek(fp, 0L, SEEK_END);
  buff_size = ftell(fp);
  rewind(fp);
  

  // Check if buff_size + 1 is correct strategy
  if(!(*buff = (u_char *) malloc((buff_size + 1) * sizeof(u_char)))){
    perror("Error in mallocing buff:");
    exit(1);
  }

  if(fread(*buff, buff_size, 1, fp) < 0){
    perror("Error in copying data to the buff:");
    exit(1);
  }

  return buff_size;
}

ssize_t res_struct_to_buff(res_struct *rs, u_char * buff){
  ssize_t print_size;
  print_size = sprintf(buff, 
                      "%s%s"
                      "%s%s%s"
                      "%s%d%s"
                      "%s",
                      rs->status_line, HTTP_RES_DELIM,  
                      HTTP_RES_CONTENT_TYPE, rs->content_type, HTTP_RES_DELIM,
                      HTTP_RES_CONTENT_LENGTH, rs->content_length, HTTP_RES_DELIM,
                      HTTP_RES_DELIM);
  
  memcpy(buff + print_size, rs->body, rs->content_length);
  return print_size + rs->content_length;

}

ssize_t process_request(char * src_buff, u_char * dest_buff, ssize_t src_buff_len, config_struct *conf){ 

  int i;
  ssize_t dest_buff_size = 0;
  ssize_t dest_content_length = 0;
  char file_name[MAXFILENAMEPATH];
  req_struct rq;
  res_struct rs;
  
  memset(&rq, 0, sizeof(rq));
  memset(&rs, 0, sizeof(rs));

  get_req_struct(&rq, src_buff, src_buff_len);
  if (strncmp(rq.method, GET_HEADER, GET_HEADER_LEN) == 0) fill_get_res_struct(&rq, &rs, conf);
  else { perror("HTTP Method Not Implemented:"); exit(1); }  
  
  debug_res_struct(&rs);
  dest_buff_size = res_struct_to_buff(&rs, dest_buff);
  /*{*/
     
     /*dest_buff_size += updatebuffwithdelim(dest_buff + dest_buff_size, */
         /*HTTP10_RES_OK, HTTP10_RES_OK_LEN);*/
     
     /*dest_buff_size += updatebuff(dest_buff + dest_buff_size, */
         /*HTTP_RES_CONTENT_TYPE, HTTP_RES_CONTENT_TYPE_LEN);*/
     
     /*dest_buff_size += updatebuffwithdelim(dest_buff + dest_buff_size, */
         /*HTTP_RES_SAMPLE_CONTENT_TYPE, HTTP_RES_SAMPLE_CONTENT_TYPE_LEN);*/

     /*dest_buff_size += updatebuff(dest_buff + dest_buff_size, */
         /*HTTP_RES_CONTENT_LENGTH, HTTP_RES_CONTENT_LENGTH_LEN);*/
     
     /*dest_buff_size += updatebuffwithdelim(dest_buff + dest_buff_size,*/
         /*HTTP_RES_SAMPLE_CONTENT_LEN, 2);*/

     /*dest_buff_size += updatebuffasdelim(dest_buff + dest_buff_size);*/

     /*dest_buff_size += updatebuff(dest_buff + dest_buff_size, */
         /*HTTP_RES_SAMPLE_CONTENT, 11);*/
  /*}*/
  
  /*dest_buff_size += updatebuff(dest_buff + dest_buff_size, HTTP_RES_END, HTTP_RES_END_LEN);*/
  return dest_buff_size;
}

void get_req_struct(req_struct *rq, char *buff, ssize_t buff_len){

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

    //DEBUGSS("Part of Request", temp_str);
    
    if(strncmp(temp_str, GET_HEADER, GET_HEADER_LEN) == 0) fill_get_req_struct(temp_str, rq);
    free(temp_str);
  }

  free(temp_buff);
  debug_req_struct(rq);
}

void debug_req_struct(req_struct * rq){

  DEBUGS("Printing request struct");
  DEBUGSS("\tMethod", rq->method);
  DEBUGSS("\tURI", rq->uri);
  DEBUGSS("\tHTTP Version", rq->http_version);
  DEBUGSS("\tKeep Connection Alive", rq->connection_keep_alive);

}

void debug_res_struct(res_struct* res){
  
  DEBUGS("Printing Response Struct");
  DEBUGSS("\tSTATUS LINE", res->status_line);
  DEBUGSN("\tCONTENT-LENGTH", res->content_length);
  DEBUGSS("\tCONTENT-TYPE", res->content_type);
  DEBUGSS("\tBODY", res->body);

}
void fill_get_req_struct(char * str, req_struct *rq){
 
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

void send_response(int socket, char *buff, ssize_t buff_len){
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
