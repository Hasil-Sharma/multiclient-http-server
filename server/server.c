#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include "netutils.h"
#include "debug.h"

static volatile int keepRunning = TRUE;
static volatile int listen_fd;

void waitForChildren(int con){

  keepRunning = FALSE;
  printf("Shutting down receive operation\n");

  shutdown(listen_fd, SHUT_RD);
}
int main () {

  struct timeval tv;
  int conn_fd, post_flag, first_flag, expect_data, read_bytes;
  pid_t pid;
  ssize_t recvbytes, resbytes, nbytes;
  config_struct conf;
  struct sockaddr_in remote_address;
  socklen_t addr_size;
  char req_buff[1000], *remote_ip;
  u_char res_buff[MAXREQRESBUFFER];
  process_req_res_struct res_method;
  req_struct rq;

  addr_size = sizeof (struct sockaddr_in);
  memset (&conf, 0, sizeof (config_struct));
  read_conf (&conf, "ws.conf");

  tv.tv_sec = conf.keep_alive_time;
  listen_fd = get_socket (&conf);

  signal(SIGINT, waitForChildren);

  while (keepRunning)
  {
    DEBUGS("Waiting to Accept");
    conn_fd =
      accept (listen_fd, (struct sockaddr *) &remote_address, &addr_size);


    if(keepRunning == FALSE) continue;

    if(conn_fd == -1) {
      perror("Cannot Accept Connection");
    }

    DEBUGS("Accepted a connection");

    memset (req_buff, 0, sizeof (req_buff));
    memset (res_buff, 0, sizeof (res_buff));

    checkforerror (conn_fd, "Eror in accepting connections");

    remote_ip = inet_ntoa (remote_address.sin_addr);

    DEBUGSN ("Parent PID:", getpid ());
    DEBUGSS ("Accepted Connection from", remote_ip);
    DEBUGSN ("Port", ntohs (remote_address.sin_port));
    DEBUGSS ("Waiting for input from", remote_ip);
    DEBUGSN ("Port", ntohs (remote_address.sin_port));

    pid = fork ();

    if (pid != 0)
    {
      // Close connection on parent
      close (conn_fd);
      continue;

    }
    else
    {
      signal(SIGINT, SIG_IGN);
      setsockopt (conn_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof (struct timeval));

      while (keepRunning)
      {
        read_bytes = 1;
        nbytes = 0;
        expect_data = 0;
        DEBUGSN ("Reading Child PID:", getpid ());
        memset (&rq, 0, sizeof (req_struct));
        while (keepRunning)
        {

          if (expect_data == 1 && rq.method != NULL)
            if (strcmp (rq.method, POST_HEADER) == 0)
              read_bytes = rq.content_length;
            else if (strcmp (rq.method, GET_HEADER) == 0)
              break;

          recvbytes = recv (conn_fd, req_buff + nbytes, read_bytes, 0);
          if (recvbytes == 0 | recvbytes == -1)
            break;
          checkforerror (recvbytes, "Error with recv");
          nbytes += recvbytes;

          if (expect_data == 1)
          {
            rq.content = strdup (req_buff);
            break;
          }
          if (strstr (req_buff, HTTP_REQ_DELIM))
          {

            if (strcmp (req_buff, HTTP_REQ_DELIM) == 0 && rq.method != NULL)
            {
              expect_data += 1;
            }
            else
            {
              get_req_struct (&rq, req_buff, nbytes);
            }
            // rest the reading parameters
            nbytes = 0;
            memset (req_buff, 0, sizeof (req_buff));
          }
        }

        // Avoiding Segmentation Fault
        if(recvbytes > 0 && rq.connection == NULL) rq.connection = strdup(HTTP_REQ_CONNECTION_CLOSE);

        debug_req_struct (&rq);

        if (recvbytes == 0 || recvbytes == -1)
        {
          DEBUGSN ("Closing Child PID:", getpid ());
          if (recvbytes == -1)
            DEBUGS ("Socket timing out");
          DEBUGSS ("Connection Closed by", remote_ip);
          DEBUGSN ("Port", ntohs (remote_address.sin_port));
          close (conn_fd);
          exit(0);
        }

        /*DEBUGSS("Client Sent", req_buff); */
        memset (&res_method, 0, sizeof (struct process_req_res_struct));
        process_request (&rq, res_buff, nbytes, &conf, &res_method);
        send_response (conn_fd, res_buff, res_method.resbytes);

        free_req_struct (&rq);
        if (res_method.conn_alive_flag == FALSE)
        {
          DEBUGS ("Closing Connection: no-keep-alive");
          close (conn_fd);
          exit(0);
        }
      }

    }

  }

  printf("Waiting For Children to exit\n");
  waitpid(-1, NULL, 0);
  close(conn_fd);
  return 0;

}

