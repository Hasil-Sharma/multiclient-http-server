#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"
#include "netutils.h"
#include "debug.h"

int main(){

  struct timeval tv;
  int listen_fd, conn_fd;
  int post_flag, first_flag;
  pid_t pid;
  ssize_t recvbytes, resbytes, nbytes;
  config_struct conf;
  struct sockaddr_in remote_address;
  socklen_t addr_size;
  char req_buff[MAXREQRESBUFFER], *remote_ip;
  u_char res_buff[MAXREQRESBUFFER];
  process_req_res_struct res_method;
  addr_size = sizeof(struct sockaddr_in);

  memset(&conf, 0, sizeof(config_struct));
  read_conf(&conf, "ws.conf");
  /*print_config_struct(&conf);*/
  
  tv.tv_sec = conf.keep_alive_time;
  listen_fd = get_socket(&conf);

  while(TRUE){
    
    conn_fd = accept(listen_fd, (struct sockaddr *)&remote_address, &addr_size);

    memset(req_buff, 0, sizeof(req_buff));
    memset(res_buff, 0, sizeof(res_buff));
    
    checkforerror(conn_fd, "Eror in accepting connections"); 
 
    remote_ip = inet_ntoa(remote_address.sin_addr);
    
    DEBUGSN("Parent PID:", getpid());
    DEBUGSS("Accepted Connection from", remote_ip);
    DEBUGSN("Port", ntohs(remote_address.sin_port));
    DEBUGSS("Waiting for input from", remote_ip);
    DEBUGSN("Port", ntohs(remote_address.sin_port));
    
    pid = fork(); 

    if(pid != 0){
      // Close connection on parent
      close(conn_fd);
      continue; 

    } else {

      setsockopt(conn_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));
  
      post_flag = FALSE;
      first_flag = TRUE;
      while(TRUE){
      
        nbytes = 0;  
        while(TRUE){

          DEBUGSN("Reading Child PID:", getpid());
          recvbytes = recv(conn_fd, req_buff + nbytes, MAXREQRESBUFFER, 0);
          if(recvbytes == 0 | recvbytes == -1) break;
          checkforerror(recvbytes, "Error with recv"); 
          nbytes += recvbytes;

          if(first_flag == TRUE){
            first_flag = FALSE;
            post_flag = TRUE ? !strncmp(req_buff, POST_HEADER, POST_HEADER_LEN) : FALSE;
          }
          /*req_buff[nbytes] = '\0';*/ // No need buffer is already null char terminated
          if (strstr(req_buff, HTTP_REQ_END)){

           if(!post_flag) break;
           post_flag = FALSE;

          }

        }
        
        post_flag = FALSE;
        first_flag = TRUE;

        if(recvbytes == 0 || recvbytes == -1) {
          DEBUGSN("Closing Child PID:", getpid());
          if (recvbytes == -1) DEBUGS("Socket timing out");
          DEBUGSS("Connection Closed by", remote_ip);
          DEBUGSN("Port", ntohs(remote_address.sin_port));
          close(conn_fd);
          break;
        }
        
        /*DEBUGSS("Client Sent", req_buff);*/
        memset(&res_method, 0, sizeof(struct process_req_res_struct));
        process_request(req_buff, res_buff, nbytes, &conf, &res_method);
        send_response(conn_fd, res_buff, res_method.resbytes);
        
        if(res_method.conn_alive_flag == FALSE){
          DEBUGS("Closing Connection: no-keep-alive");
          close(conn_fd);
          break;
        }
      }

    }
    
  }
  return 0; 

}
