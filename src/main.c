/*
** main.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 18:24:18 2015 Florian SABOURIN
** Last update Sat Sep 19 18:24:18 2015 Florian SABOURIN
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl.h>
#include "database.h"
#include "mouli.h"

// Initializes the mouli fields
static void mouli_init(t_mouli *mouli)
{
  srand(time(NULL) ^ (getpid() << 11));
  mouli->threads = NULL;
  mouli->nthreads = 0;
  mouli->allocd = 0;
  mouli->socket = 0;
  mouli->clone_login = NULL;
  mouli->clone_subfolder = NULL;
  mouli->tests_subfolder = NULL;
  mouli->tests_filename = NULL;
  mouli->mail_sendaddr = NULL;
  mouli->mail_sendername = NULL;
}

// Setup basic network
static int setup_network(t_mouli *mouli, char *portstr)
{
  struct sockaddr_in sin;
  unsigned short port;
  int	t = 1;

  port = atoi(portstr);
  if (!port)
    {
      fprintf(stderr, "Invalid port\n");
      return (1);
    }
  mouli->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (mouli->socket == -1)
    {
      perror("socket");
      return (1);
    }
  if (setsockopt(mouli->socket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) == -1)
    {
      close(mouli->socket);
      perror("setsockopt");
      return (1);
    }
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  if (bind(mouli->socket, (struct sockaddr *)(&sin),
	   sizeof(struct sockaddr_in)) == -1)
    {
      close(mouli->socket);
      perror("bind");
      return (1);
    }
  if (listen(mouli->socket, 20) == -1)
    {
      close(mouli->socket);
      perror("listen");
      return (1);
    }
  return (0);
}

// Loads config from file (key=value pairs)
#warning TODO: Check if value is legal
static int load_config(t_mouli *cl, const char *filename)
{
  unsigned int i;

  cl->cfg = loadconfig(filename);
  if (!cl->cfg)
    return (1);
  for (i = 0 ; i < cl->cfg->nb_entries ; ++i)
    {
      if (!strcmp(cl->cfg->entries[i].key, "clone_subfolder"))
	cl->clone_subfolder = cl->cfg->entries[i].value;
      else if (!strcmp(cl->cfg->entries[i].key, "clone_login"))
	cl->clone_login = cl->cfg->entries[i].value;
      else if (!strcmp(cl->cfg->entries[i].key, "tests_subfolder"))
	cl->tests_subfolder = cl->cfg->entries[i].value;
      else if (!strcmp(cl->cfg->entries[i].key, "tests_filename"))
	cl->tests_filename = cl->cfg->entries[i].value;
      else if (!strcmp(cl->cfg->entries[i].key, "mail_sendaddr"))
	cl->mail_sendaddr = cl->cfg->entries[i].value;
      else if (!strcmp(cl->cfg->entries[i].key, "mail_sendername"))
	cl->mail_sendername = cl->cfg->entries[i].value;
    }
  if (!cl->clone_subfolder || !cl->clone_login || !cl->tests_subfolder ||
      !cl->tests_filename || !cl->mail_sendaddr || !cl->mail_sendername)
    {
      fprintf(stderr, "Missing fields in config file\n");
      deleteconfig(cl->cfg);
      return (1);
    }
  return (0);
}

// Entry point
// Use the program as: ./mouli <port> <sqlitefile>
#warning TODO: patch leaks
int	main(int argc, char **argv)
{
  t_mouli mouli;

  if (argc != 4)
    {
      fprintf(stderr, "Usage: %s port sqlitefile config\n", *argv);
      return (1);
    }
  mouli_init(&mouli);
  if (load_config(&mouli, argv[3]))
    return (1);
  if (database_init(argv[2]))
    {
      fprintf(stderr, "%s\n", database_geterror());
      return (1);
    }
  if (setup_network(&mouli, argv[1]) || mouli_run(&mouli))
    {
      curl_global_cleanup();
      if (database_close())
	fprintf(stderr, "Warning: failed to close database properly\n");
      return (1);
    }
  curl_global_cleanup();
  if (database_close())
    fprintf(stderr, "Warning: failed to close database properly\n");
  return (0);
}
