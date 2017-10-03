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

  int listen_fd, conn_fd;
  pid_t pid;
  ssize_t recvbytes, resbytes, nbytes;
  config_struct conf;
  struct sockaddr_in remote_address;
  socklen_t addr_size;
  char req_buff[MAXREQRESBUFFER], *remote_ip;
  u_char res_buff[MAXREQRESBUFFER];

  addr_size = sizeof(struct sockaddr_in);

  memset(&conf, 0, sizeof(config_struct));
  read_conf(&conf, "ws.conf");
  //print_config_struct(&conf);

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
      while(TRUE){
        
        nbytes = 0;

        while(TRUE){

          DEBUGSN("Reading Child PID:", getpid());
          recvbytes = recv(conn_fd, req_buff + nbytes, MAXREQRESBUFFER, 0);
          checkforerror(recvbytes, "Error with recv"); 
          if(recvbytes == 0) break;
          nbytes += recvbytes;
          /*req_buff[nbytes] = '\0';*/ // No need buffer is already null char terminated
          if (strstr(req_buff, HTTP_REQ_END)) break;

        }

        if(recvbytes == 0) {
          DEBUGSN("Closing Child PID:", getpid());
          DEBUGSS("Connection Closed by", remote_ip);
          DEBUGSN("Port", ntohs(remote_address.sin_port));
          close(conn_fd);
          break;
        }
        
        /*DEBUGSS("Client Sent", req_buff);*/
        resbytes = process_request(req_buff, res_buff, nbytes, &conf);
        send_response(conn_fd, res_buff, resbytes);

      }

    }
    
  }
  return 0; 

}
