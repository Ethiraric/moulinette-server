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

// Check that the configuration value is valid (i.e. it may be passed to
// system() with no risk)
#define IS_VALUE_VALID(a) (strspn(a, "abcdefghijklmnopqrstuvwxyz0123456789" \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ-_.@") == strlen(a))

// Initialize the mouli fields
// Also initialize rand()
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
  mouli->cfg = NULL;
}

// Deletes and deallocates all the resources
static void mouli_delete(t_mouli *mouli)
{
  unsigned int i;

  if (mouli->cfg)
    deleteconfig(mouli->cfg);
  for (i = 0 ; i < mouli->nthreads ; ++i)
    free(mouli->threads[i]);
  free(mouli->threads);
  if (mouli->socket > 0)
    close(mouli->socket);
}

// Setup basic network
// Create a socket, bind it and listen on it
// The listen queue is set to 20
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

  // Set the reuseaddr flag so we don't have to wait in case the network is
  // not shut down properly (http://stackoverflow.com/a/14388707/4173304)
  if (setsockopt(mouli->socket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) == -1)
    {
      close(mouli->socket);
      mouli->socket = 0;
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
      mouli->socket = 0;
      perror("bind");
      return (1);
    }
  if (listen(mouli->socket, 20) == -1)
    {
      close(mouli->socket);
      mouli->socket = 0;
      perror("listen");
      return (1);
    }
  return (0);
}

// Loads config from file (key=value pairs)
// Fields needed are:
// clone_subfolder: the folder in which to clone the repositories. They will
//    be cloned to <subfolder>/<login>/<repository>
// clone_login:     the login to git clone with
//    (git clone <login>@git.epitech.eu)
// tests_subfolder: the folder where all the tests are. Tests must have the
//    following name: <test folder>/<repository>
// tests_filename:  the file which will launch the tests. It will be piped so
//    the moulinette can get its output (stdout ONLY). It must be executable
//    (no interpreter will be used by default)
// mail_sendaddr:   the mail the moulinette will use. It does not need to be
//    a valid address (sendmail doesn't check)
// mail_sendername: the name the mail will be sent with
// ALL fields are MANDATORY
// A check is performed so there is no character in the values (double quotes,
// spaces ...) which may be a security risk when using system() or sqlite
static int load_config(t_mouli *cl, const char *filename)
{
  unsigned int i;

  cl->cfg = loadconfig(filename);
  if (!cl->cfg)
    return (1);

  // Assign value in the mouli structure
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

  // Check if all fields are present
  if (!cl->clone_subfolder || !cl->clone_login || !cl->tests_subfolder ||
      !cl->tests_filename || !cl->mail_sendaddr || !cl->mail_sendername)
    {
      fprintf(stderr, "Missing fields in config file\n");
      deleteconfig(cl->cfg);
      return (1);
    }

  // Ensure they are all valid
  if (!IS_VALUE_VALID(cl->clone_subfolder) ||
      !IS_VALUE_VALID(cl->clone_login) ||
      !IS_VALUE_VALID(cl->tests_subfolder) ||
      !IS_VALUE_VALID(cl->tests_filename) ||
      !IS_VALUE_VALID(cl->mail_sendaddr) ||
      !IS_VALUE_VALID(cl->mail_sendername))
    {
      fprintf(stderr, "Invalid value(s)\n");
      deleteconfig(cl->cfg);
      return (1);
    }
  return (0);
}

// Entry point
// Use the program as: ./mouliserver <port> <sqlitefile> <configfile>
int	main(int argc, char **argv)
{
  t_mouli mouli;

  if (argc != 4)
    {
      fprintf(stderr, "Usage: %s port sqlitefile configfile\n", *argv);
      return (1);
    }

  // Init config and database
  mouli_init(&mouli);
  if (load_config(&mouli, argv[3]))
    return (1);
  if (database_init(argv[2]))
    {
      mouli_delete(&mouli);
      fprintf(stderr, "%s\n", database_geterror());
      return (1);
    }

  // Init network and set main loop
  if (setup_network(&mouli, argv[1]) || mouli_run(&mouli))
    {
      curl_global_cleanup();
      if (database_close())
	fprintf(stderr, "Warning: failed to close database properly\n");
      mouli_delete(&mouli);
      return (1);
    }

  // Global cleanup
  mouli_delete(&mouli);
  curl_global_cleanup();
  if (database_close())
    fprintf(stderr, "Warning: failed to close database properly\n");
  return (0);
}
