#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "debug.h"


void checkforerror (int val, char *error_msg)
{
  if (val < 0)
  {
    perror (error_msg);
    exit (1);
  }
}

void read_conf (config_struct * conf, char *file_name)
{

  FILE *fp;
  int file_line_len, content_index;
  char line[MAXFILEBUFF];
  char buff[MAXFILEBUFF];
  char *temp_char_ptr;

  if ((fp = fopen (file_name, "r")) <= 0)
  {
    perror ("Error In Opening Config File: ");
  }

  while (fgets (line, sizeof (line), fp))
  {
    // Ignore in case of comments or empty lines
    if ((strncmp (line, "#", 1) == 0) || (strncmp (line, "\n", 1) == 0))
      continue;
    file_line_len = strlen (line);
    if (line[file_line_len - 1] == '\n')
      line[file_line_len - 1] = '\0';
    memset (buff, 0, sizeof (buff));
    get_second_string (&temp_char_ptr, line, ' ');
    strcpy (buff, temp_char_ptr);
    if (strncmp (line, LISTEN_PORT_CONF, LISTEN_PORT_CONF_LEN) == 0) conf->port_number = (u_short) strtoul (buff, NULL, 10);
    else if (strncmp (line, DOCUMENT_ROOT_CONF, DOCUMENT_ROOT_CONF_LEN) == 0)
    {
      conf->doc_root = strdup (buff);
      remove_first_last_char (&conf->doc_root);
    }
    else if (strncmp (line, DIRECTORY_INDEX, DIRECTORY_INDEX_LEN) == 0) conf->doc_index = strdup (buff);
    else if (strncmp (line, KEEP_ALIVE_TIME_CONF, KEEP_ALIVE_TIME_CONF_LEN) == 0)
    {
      conf->keep_alive_time = atoi (buff);
    }
    else if (strncmp (line, CONTENT_TYPE_START, CONTENT_TYPE_START_LEN) == 0)
    {
      content_index = conf->content_type_index;
      conf->content_type[content_index] = (content_type_struct *) malloc (sizeof (content_type_struct));
      get_first_string (&conf->content_type[content_index]->extension, line, ' ');
      get_second_string (&conf->content_type[content_index]->type, line, ' ');
      conf->content_type_index++;
    }
  }

  fclose (fp);
}


void remove_first_last_char (char **str)
{
  int len = strlen (*str);

  /*DEBUGS("Remove First last Char"); */
  /*DEBUGS(*str); */
  // Remove the last character
  (*str)[len - 1] = '\0';

  /*DEBUGS(*str); */

  // Remove the fist character
  (*str) = (*str) + 1;

}

void get_second_string (char **dest, char *src, char delim)
{

  char *delim_char = strchr (src, delim);
  *dest = strdup (delim_char + 1);

}

void get_first_string (char **dest, char *src, char delim)
{

  char *delim_char = strchr (src, delim);
  int offset = delim_char - src;
  *dest = strndup (src, offset);

}

void get_extension (char **dest, char *src)
{

  char *delim_char = strrchr (src, '.');
  *dest = strdup (delim_char);
  // dest will have the "." in the starting
}

ssize_t fill_buff_with_templates (char *buff, const char **templates, int template_len)
{
  ssize_t buff_len;
  char *temp;
  int i;
  temp = strdup ("%s");
  for (i = 0; i < template_len; i++)
  {
    buff_len = sprintf (buff, temp, templates[i]);
    free (temp);
    temp = strdup (buff);
  }
  free (temp);
  return buff_len;
}

void print_config_struct (config_struct * conf)
{

  int i;
  DEBUGS ("Printing Conf Struct");
  DEBUGS ("Port Number");
  DEBUGN (conf->port_number);

  DEBUGS ("Doc Root");
  DEBUGS (conf->doc_root);

  DEBUGS ("Doc Index");
  DEBUGS (conf->doc_index);

  DEBUGS ("Keep Alive Time");
  DEBUGN (conf->keep_alive_time);

  DEBUGS ("Content-Type");

  for (i = 0; i < conf->content_type_index; i++)
  {
    DEBUGSS (conf->content_type[i]->extension, conf->content_type[i]->type);
  }

}
