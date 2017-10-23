#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include "utils.h"
#include "netutils.h"
#include "debug.h"

int get_socket (config_struct * conf)
{

  int sockfd;
  struct sockaddr_in sin;
  int yes = 1;
  socklen_t remotelength;

  memset (&sin, 0, sizeof (sin));

  sin.sin_family = AF_INET;
  sin.sin_port = htons (conf->port_number);
  sin.sin_addr.s_addr = INADDR_ANY;

  checkforerror (sockfd = socket (AF_INET, SOCK_STREAM, 0), "Unable to create socket");

  // Avoiding the "Address Already in use" error message
  if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) < 0)
  {
    perror ("Unable to set so_reuseaddr:");
    exit (1);
  }

  if (bind (sockfd, (struct sockaddr *) &sin, sizeof (sin)) < 0)
  {
    perror ("Unable to bind the socket:");
    exit (1);
  }

  if (listen (sockfd, MAXCONNECTION) < 0)
  {
    perror ("Unable to call listen on socket:");
    exit (1);
  }

  /*if( fcntl(sockfd, F_SETFL, O_NONBLOCK)  < 0){*/
  /*perror("Unable to set socket as non-blocking:");*/
  /*exit (1);*/
  /*}*/

  return sockfd;

}


ssize_t update_buff (char *buff, char *content, ssize_t content_length)
{

  memcpy (buff, content, content_length);
  return content_length;

}


ssize_t update_buff_as_delim (char *buff)
{

  return update_buff (buff, HTTP_RES_DELIM, HTTP_RES_DELIM_LEN);

}

ssize_t update_buff_with_delim (char *buff, char *content, ssize_t content_length)
{

  content_length = update_buff (buff, content, content_length);
  content_length += update_buff_as_delim (buff + content_length);
  return content_length;

}

int check_extension_type (char *extension, char **type, config_struct * conf)
{

  int i;
  int flag = FALSE;
  *type = NULL;
  for (i = 0; i < MAXCONTENTTYPE && extension; i++)
  {

    if (!conf->content_type[i])
      break;      // in case of NULL
    if (strcmp (extension, conf->content_type[i]->extension) == 0)
    {
      flag = TRUE;
      *type = strdup (conf->content_type[i]->type);
      break;
    }

  }

  return flag;
}

void fill_error_res_struct (req_struct * rq, res_struct * rs, config_struct * conf, char *flag)
{

  char data[MAXREQRESBUFFER];
  ssize_t data_length, template_size;
  char *temp;
  const char *template_array[MAX_TEMPLATES];

  template_array[0] = HTTP_GENERIC_TEMPLATE;
  if (strncmp (flag, HTTP_BAD_REQ_FLAG, HTTP_BAD_REQ_FLAG_LEN) == 0)
  {

    template_array[1] = HTTP_RES_BAD_REQ_TEMPLATE;
    rs->status_line = strcmp(rq->http_version, HTTP_1_1)  == 0 ? strdup (HTTP_RES_11_BAD_REQ) : strdup (HTTP_RES_10_BAD_REQ);
    rs->content_type = strdup (HTTP_RES_BAD_REQ_TYPE);

    if (strcmp (flag, HTTP_BAD_REQ_INVALID_METHOD_FLAG) == 0)
    {

      template_array[2] = HTTP_RES_BAD_REQ_INVALID_METHOD_TEMPLATE;
      template_array[3] = rq->method;
      template_size = 4;

    }
    else if (strcmp (flag, HTTP_BAD_REQ_INVALID_URI_FLAG) == 0)
    {

      template_array[2] = HTTP_RES_BAD_REQ_INVALID_URI_TEMPLATE;
      template_array[3] = rq->uri;
      template_size = 4;

    }
    else if (strcmp (flag, HTTP_BAD_REQ_INVALID_TYPE_FLAG) == 0)
    {

      template_array[2] = HTTP_RES_BAD_REQ_INVALID_FILE_TYPE_TEMPLATE;
      template_array[3] = rq->uri;
      template_size = 4;

    }
    else if (strcmp (flag, HTTP_BAD_REQ_INVALID_HTTP_FLAG) == 0)
    {
      template_array[2] = HTTP_RES_BAD_REQ_INVALID_HTTP_TEMPLATE;
      template_array[3] = rq->http_version;
      template_size = 4;
    }
    else
    {
      fprintf (stderr, "Unknown Flag : %s", flag);
      exit (1);
    }

  }
  else if (strcmp (flag, HTTP_RES_404_FLAG) == 0)
  {

    rs->status_line = strcmp(rq->http_version, HTTP_1_1) == 0 ? strdup (HTTP_RES_11_404) : strdup(HTTP_RES_10_404);
    rs->content_type = strdup (HTTP_RES_404_TYPE);

    template_array[1] = HTTP_RES_404_FILE_TEMPLATE;
    template_array[2] = rq->uri;
    template_size = 3;

  }
  else if (strncmp (flag, HTTP_NOT_IMPLEMENTED_FLAG, HTTP_NOT_IMPLEMENTED_FLAG_LEN) == 0)
  {

    rs->status_line =  strcmp (rq->http_version, HTTP_1_1) == 0 ? strdup (HTTP_RES_11_NOT_IMPLEMENTED) : strdup(HTTP_RES_10_NOT_IMPLEMENTED);
    rs->content_type = strdup (HTTP_RES_NOT_IMPLEMENTED_TYPE);
    template_array[1] = HTTP_RES_NOT_IMPLEMENTED_TEMPLATE;

    if (strcmp (flag, HTTP_NOT_IMPLEMENTED_METHOD_FLAG) == 0)
    {

      template_array[2] = HTTP_RES_NOT_IMPLEMENTED_METHOD_TEMPLATE;
      template_array[3] = rq->method;
      template_size = 4;

    }
    else if (strcmp (flag, HTTP_NOT_IMPLEMENTED_FILE_TYPE_FLAG) == 0)
    {
      template_array[2] = HTTP_RES_NOT_IMPLEMENTED_FILE_TYPE_TEMPLATE;
      template_array[3] = rq->uri;
      template_size = 4;
    }

  }
  else if (strcmp(flag, HTTP_RES_SERVER_ERROR_FLAG) == 0)
  {
    rs->status_line =  strcmp (rq->http_version, HTTP_1_1) == 0 ? strdup (HTTP_RES_11_SERVER_ERROR) : strdup(HTTP_RES_10_SERVER_ERROR);
    rs->content_type = strdup(HTTP_RES_SERVER_ERROR_TYPE);
    template_array[1] = HTTP_RES_SERVER_OOM;
    template_size = 2;
  }
  else
  {

    fprintf (stderr, "Unknown Flag : %s", flag);
    exit (1);

  }

  data_length = fill_buff_with_templates (data, template_array, template_size);
  rs->content_length = data_length;
  rs->body = (u_char *) malloc (data_length * sizeof (u_char));
  memcpy (rs->body, data, data_length);
}

bool  check_http_version(req_struct *rq)
{

  if (strcmp(rq->http_version, HTTP_1_1) == 0 || strcmp(rq->http_version, HTTP_1_0) == 0) return 0;
  else return 1;
}

void fill_get_res_struct (req_struct * rq, res_struct * rs, config_struct * conf)
{

  char *file_name, *extension, *type, *temp;
  char file_path[MAXFILENAMEPATH];
  int temp_len;

  FILE *fp;

  if (strchr (rq->uri, GET_URI_ROOT) == 0) file_name = strdup (conf->doc_index);
  else if (check_last_char(rq->uri, GET_URI_ROOT))
  {
    temp = strdup (conf->doc_index);
    temp_len = strlen(temp);
    file_name = (char *) malloc(temp_len + strlen(rq->uri) + 2);
    sprintf(file_name, "%s%s", rq->uri + 1, temp);
    free(temp);
  }
  else if (strchr (rq->uri, '.')) file_name = strdup(rq->uri + 1);
  else file_name = strdup (rq->uri + 1); // +1 to skip the initial / and making it consistent with doc_index

  get_extension (&extension, file_name);

  if (check_extension_type (extension, &type, conf))
  {

    rs->content_type = strdup (type);

    if (sprintf (file_path, "%s/%s", conf->doc_root, file_name) < 0)
    {
      fill_error_res_struct(rq, rs, conf, HTTP_RES_SERVER_ERROR_FLAG);
      return ;
      /*perror ("Error in making file path string:");*/
      /*exit (1);*/
    }

    if (!(fp = fopen (file_path, "rb")))
    {
      // Case when request file doesn't exist
      fill_error_res_struct (rq, rs, conf, HTTP_RES_404_FLAG);
    }

    // TODO: Test it works fine

    if (fp)
    {
      rs->content_length = fill_res_body (fp, &(rs->body));
      if (rs->content_length == -1)
      {
        fill_error_res_struct(rq, rs, conf, HTTP_RES_SERVER_ERROR_FLAG);
        return ;
      }
      rs->status_line = strcmp(rq->http_version, HTTP_1_1) == 0 ? strdup (HTTP_RES_11_OK) : strdup(HTTP_RES_10_OK);
      fclose (fp);
    }

    free (extension);
    free (file_name);

  }
}

void fill_post_res_struct (req_struct * rq, res_struct * rs, config_struct * conf)
{
  char *temp;
  int index, extra_memory;
  int temp_len;
  char *file_name, *extension, *type;
  char file_path[MAXFILENAMEPATH];

  FILE *fp;

  if (strchr (rq->uri, GET_URI_ROOT) == 0) file_name = strdup (conf->doc_index);
  else if (check_last_char(rq->uri, GET_URI_ROOT))
  {
    temp = strdup (conf->doc_index);
    temp_len = strlen(temp);
    file_name = (char *) malloc(temp_len + strlen(rq->uri) + 1);
    sprintf(file_name, "%s/%s", rq->uri + 1, temp);
    free(temp);
  }
  else if (strchr (rq->uri, '.')) file_name = strdup(rq->uri + 1);
  else file_name = strdup (rq->uri + 1); // +1 to skip the initial / and making it consistent with doc_index

  get_extension (&extension, file_name);

  if (check_extension_type (extension, &type, conf))
  {

    rs->content_type = strdup (type);

    if (sprintf (file_path, "%s/%s", conf->doc_root, file_name) < 0)
    {
      fill_error_res_struct(rq, rs, conf, HTTP_RES_SERVER_ERROR_FLAG);
      return ;
      /*perror ("Error in making file path string:");*/
      /*exit (1);*/
    }

    if (!(fp = fopen (file_path, "rb")))
    {
      // Case when request file doesn't exist
      fill_error_res_struct (rq, rs, conf, HTTP_RES_404_FLAG);
      free(extension);
      free(file_name);
      return;
    }

    // TODO: Test it works fine

    if (fp)
    {
      rs->content_length = fill_res_body (fp, &(rs->body));
      if (rs->content_length == -1)
      {
        fill_error_res_struct(rq, rs, conf, HTTP_RES_SERVER_ERROR_FLAG);
        return ;
      }
      rs->status_line = strcmp(rq->http_version, HTTP_1_1) == 0 ? strdup (HTTP_RES_11_OK) : strdup(HTTP_RES_10_OK);
      fclose (fp);
    }

    free (extension);
    free (file_name);

    // Writing the </body> makes this formatting agnostic, no matter the file formatting
    // this will work

    temp = strstr (rs->body, HTTP_BODY_END);
    memset (temp, 0, sizeof (u_char));
    rs->content_length -= rs->content_length - (temp - (char *) rs->body);

    extra_memory = rq->content_length + HTTP_POST_DATA_HEADING_LEN + HTTP_PRE_START_END_TAG_LEN + HTTP_END_LEN + 1;

    rs->body = (u_char *) realloc (rs->body, rs->content_length + extra_memory);
    rs->content_length += sprintf (rs->body + rs->content_length, "%s%s%s%s%s", HTTP_POST_DATA_HEADING, HTTP_PRE_START_TAG, rq->content, HTTP_PRE_END_TAG, HTTP_END);
  }

}

size_t fill_res_body (FILE * fp, u_char ** buff)
{

  size_t buff_size;
  fseek (fp, 0L, SEEK_END);
  buff_size = ftell (fp);
  rewind (fp);


  // Check if buff_size + 1 is correct strategy
  if (!(*buff = (u_char *) malloc ((buff_size + 1) * sizeof (u_char))))
  {
    /*perror ("Error in mallocing buff:");*/
    /*exit (1);*/
    return -1;
  }

  if (fread (*buff, buff_size, 1, fp) < 0)
  {
    /*perror ("Error in copying data to the buff:");*/
    /*exit (1);*/
    return -1;
  }

  return buff_size;
}

ssize_t res_struct_to_buff (res_struct * rs, u_char * buff)
{
  ssize_t print_size;
  print_size = sprintf (buff,
                        "%s%s"
                        "%s%s%s"
                        "%s%d%s"
                        "%s%s%s"
                        "%s",
                        rs->status_line, HTTP_RES_DELIM,
                        HTTP_RES_CONTENT_TYPE, rs->content_type,
                        HTTP_RES_DELIM, HTTP_RES_CONTENT_LENGTH,
                        rs->content_length, HTTP_RES_DELIM,
                        HTTP_RES_CONNECTION, rs->connection, HTTP_RES_DELIM,
                        HTTP_RES_DELIM);

  memcpy (buff + print_size, rs->body, rs->content_length);
  return print_size + rs->content_length;

}

// TODO
bool  check_uri (req_struct *rq)
{
  char *temp;
  char *illegal_strings[13] = { "<", ">", "#", "%", "<\">", "{", "}", "|", "\\", "^", "[", "]", "`"};
  int illegal_strings_size = 13, i;

  if (rq->uri && rq->uri[0] == '/')
  {
    temp = rq->uri;
    for (i = 0; i < illegal_strings_size; i++)
    {
      if (strstr(rq->uri, illegal_strings[i])) return true;
    }

    return false;
  }

  return true;
}

bool check_method(req_struct * rq)
{
  if ( rq->method != NULL && !(strcmp (rq->method, POST_HEADER) == 0 || strcmp(rq->method, GET_HEADER) == 0) ) return true;
  return false;
}

bool check_type(req_struct *rq, config_struct *conf)
{
  char *extension, *uri, *file_name, *type;
  bool return_value;
  if (rq->uri == NULL) return true;
  uri = rq->uri;

// If it is just the "/"
  if (check_last_char(uri, '/')) return false;

  file_name = strdup(uri + 1);
  get_extension(&extension, file_name);

  return_value = check_extension_type(extension, &type, conf);

  free(file_name);
  free(extension);
  free(type);

  return !return_value;

}
void process_request (req_struct * rq, u_char * dest_buff, ssize_t src_buff_len, config_struct * conf, process_req_res_struct * res_method)
{

  int i;
  ssize_t dest_buff_size = 0,  dest_content_length = 0;
  char file_name[MAXFILENAMEPATH];
  res_struct rs;
  bool http_flag, uri_flag, method_flag, type_flag;
  memset (&rs, 0, sizeof (rs));
  memset(dest_buff, 0, sizeof(dest_buff));
  // Case when no method is supplied by rest are send
  // TODO: Do the flag thing for type as well
  http_flag = check_http_version(rq);
  uri_flag = check_uri(rq);
  method_flag = check_method(rq);
  type_flag = check_type(rq, conf);

  if (method_flag) fill_error_res_struct(rq, &rs, conf, HTTP_NOT_IMPLEMENTED_METHOD_FLAG);
  else if (uri_flag) fill_error_res_struct(rq, &rs, conf, HTTP_BAD_REQ_INVALID_URI_FLAG);
  else if (type_flag) fill_error_res_struct(rq, &rs, conf, HTTP_NOT_IMPLEMENTED_FILE_TYPE_FLAG);
  else if (http_flag) fill_error_res_struct(rq, &rs, conf, HTTP_BAD_REQ_INVALID_HTTP_FLAG);
  else if (rq->method != NULL)
  {
    if (strcmp (rq->method, GET_HEADER) == 0) fill_get_res_struct (rq, &rs, conf);
    else if (strcmp (rq->method, POST_HEADER) == 0) fill_post_res_struct (rq, &rs, conf);
  }
  /*else*/
  /*{*/
  /*// Requested method is not implemented*/
  /*fill_error_res_struct (rq, &rs, conf, HTTP_NOT_IMPLEMENTED_METHOD_FLAG);*/
  /*}*/

  rs.connection = strdup(rq->connection);

  debug_res_struct (&rs);
  dest_buff_size = res_struct_to_buff (&rs, dest_buff);

  res_method->resbytes = dest_buff_size;
  // FALSE if rs->connection is connection close
  // TODO: handle the case where connection: value is something else
  res_method->conn_alive_flag = strcmp (rs.connection, HTTP_REQ_CONNECTION_CLOSE) == 0 ? FALSE : TRUE;
  free_res_struct (&rs);
}


void get_req_struct (req_struct * rq, char *buff, ssize_t buff_len, int first_flag)
{

  char *temp_char, *temp_str, *temp_buff;
  ssize_t temp_buff_len, temp_len;
  int flag = TRUE, conn_flag = FALSE;

  temp_buff = (char *) malloc ((buff_len + 1) * sizeof (char));
  memset (temp_buff, 0, sizeof (temp_buff));
  // copying the entire buffer
  memcpy (temp_buff, buff, buff_len);
  temp_buff_len = buff_len;

  temp_char = strstr (temp_buff, HTTP_REQ_DELIM);
  temp_len = temp_char - temp_buff;
  temp_str = strndup (temp_buff, temp_len);


  // Assuming that the first line is of type METHOD URI HTTP_VERSION

  //if (strncmp (temp_str, GET_HEADER, GET_HEADER_LEN) == 0 || strncmp (temp_str, POST_HEADER, POST_HEADER_LEN) == 0) fill_req_struct (temp_str, rq, REQ_HEADER);
  if (first_flag == 1 && rq->method == NULL) fill_req_struct(temp_str, rq, REQ_HEADER);
  else if (strncmp (temp_str, HTTP_REQ_CONNECTION_PARAM, HTTP_REQ_CONNECTION_PARAM_LEN) == 0) fill_req_struct (temp_str, rq, HTTP_REQ_CONNECTION_PARAM);
  else if (strncmp (temp_str, HTTP_REQ_CONTENT_LEN_PARAM, HTTP_REQ_CONTENT_LEN_PARAM_LEN) == 0) fill_req_struct (temp_str, rq, HTTP_REQ_CONTENT_LEN_PARAM);

  free (temp_str);


  free (temp_buff);
}

void fill_req_struct (char *str, req_struct * rq, char *flag)
{
  char *temp1, *temp2;
  ssize_t str_len;

  DEBUGSS ("Part of Request Filled", str);
  if (strcmp (flag, HTTP_REQ_CONNECTION_PARAM) == 0)
  {

    temp1 = strchr (str, ' ');
    temp2 = strdup (temp1 + 1);
    rq->connection = temp2;

  }
  else if (strcmp (flag, HTTP_REQ_CONTENT_LEN_PARAM) == 0)
  {

    temp1 = strchr (str, ' ');
    temp2 = strdup (temp1 + 1);
    rq->content_length = atoi (temp2);

  }
  else if (strcmp (flag, REQ_HEADER) == 0)
  {

    temp1 = strchr (str, ' ');  // pointer of first space
    str_len = temp1 - str;

    rq->method = strndup (str, str_len);

    temp2 = strrchr (str, ' '); // point to the last space
    str_len = temp2 - 1 - temp1;

    rq->uri = strndup (temp1 + 1, str_len);

    rq->http_version = strdup (temp2 + 1);

  }
  else if (strcmp (flag, HTTP_REQ_CONTENT) == 0)
  {
    rq->content = strdup (str);
  }
}

void send_response (int socket, char *buff, ssize_t buff_len)
{
  ssize_t resbytes;
  resbytes = send (socket, buff, buff_len, 0);

  checkforerror (resbytes, "Unable to reply data");

  // TODO: Case when partial response is sent
  checkforerror (resbytes - buff_len, "Unable to send complete data");

}

int checkifget (char *buff)
{
  if (strncmp (buff, GET_HEADER, GET_HEADER_LEN) == 0)
    return TRUE;
  return FALSE;
}



void free_res_struct (res_struct * rs)
{
  free (rs->status_line);
  free (rs->content_type);
  free (rs->body);
}

void free_req_struct (req_struct * rq)
{
  free (rq->method);
  free (rq->uri);
  free (rq->connection);
  free (rq->http_version);
}

void debug_req_struct (req_struct * rq)
{

  DEBUGSN ("Child PID:", getpid ());
  DEBUGS ("Printing request struct");
  DEBUGSS ("\tMethod", rq->method);
  DEBUGSS ("\tURI", rq->uri);
  DEBUGSS ("\tHTTP Version", rq->http_version);
  DEBUGSS ("\tKeep Connection Alive", rq->connection);

  // Will throw seg fault in case rq->method is NULL
  if (rq->method && strcmp (rq->method, POST_HEADER) == 0)
    DEBUGSN ("\tContent Length", rq->content_length);
  //DEBUGSS("\tContent", rq->content);

}

void debug_res_struct (res_struct * res)
{

  DEBUGSN ("Child PID:", getpid ());
  DEBUGS ("Printing Response Struct");
  DEBUGSS ("\tSTATUS LINE", res->status_line);
  DEBUGSN ("\tCONTENT-LENGTH", res->content_length);
  DEBUGSS ("\tCONTENT-TYPE", res->content_type);
  //DEBUGSS("\tBODY", res->body);

}
