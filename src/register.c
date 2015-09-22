/*
** register.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Mon Sep 21 23:49:36 2015 Florian SABOURIN
** Last update Mon Sep 21 23:49:36 2015 Florian SABOURIN
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mouli.h"

// Generate a new key for a new user
static void create_key(byte *dst)
{
  unsigned int pos;

  for (pos = 0 ; pos < 32 ; ++pos)
    dst[pos] = rand() & 0xFF;
}

// Use sendmail to send the mail
// Most of it can be configured in the config file
static int send_mail(t_threadinfo *me, const char *login, const char *key)
{
  char	*str;
  int	status;

  // Prepare system command
  status = asprintf(&str, "echo -e \"" MAIL_CONTENT "\" | sendmail -F \"%s\""
			  " -f \"%s\" \"%s@epitech.eu\"", key,
		    me->mouli->mail_sendername, me->mouli->mail_sendaddr,
		    login);
  if (status == -1)
    {
      dprintf(me->socket, "Failed to send mail\n");
      perror("asprintf");
      return (1);
    }

  // Run it
  status = system(str);
  free(str);

  // Check for errors (if system fails, or sendmail exited abnormally)
  if (status == -1)
    {
      dprintf(me->socket, "Failed to send mail\n");
      perror("system");
      return (1);
    }
  status = WEXITSTATUS(status);
  if (status)
    {
      dprintf(me->socket, "Failed to send mail\n");
      return (1);
    }
  return (0);
}

// Create key, register student in database, and send him the mail
static int handle_register(t_threadinfo *me, char *login, char *unam)
{
  char	keystr[64];
  byte	key[32];
  int	i;

  // Create a 32 bytes-long key
  create_key(key);

  // Convert it to hexadecimal for the database and the mail
  for (i = 0 ; i < 32 ; ++i)
    sprintf(&keystr[i * 2], "%02hhx" ,key[i]);

  // Add it to the database and send the mail
  if (database_new_user(login, unam, keystr))
    {
      dprintf(me->socket, "Failed to register (severe)\n");
      return (1);
    }
  return (send_mail(me, login, keystr));
}

// When a student wants to register
// Read login, username
// Generate a key, add to the database, and send him a mail
int	client_register(t_threadinfo *me)
{
  size_t buflen;
  char	*login;
  char	*username;
  char	*eol;
  int	status;

  // Read first line (login)
  buflen = 0;
  while ((status = read(me->socket, &me->buffer[buflen], 127 - buflen)) > 0)
    {
      buflen += status;
      me->buffer[buflen] = '\0';
      eol = strchr(me->buffer, '\n');
      if (eol) // Stop if we found newline
	break;
    }
  if (status <= 0)
    {
      perror("read");
      return (1);
    }

  // Change '\n' to '\0' and set username to begin right after
  *eol = '\0';
  login = me->buffer;
  username = ++eol;

  // Read username
  eol = strchr(eol, '\n');
  if (!eol)
    while ((status = read(me->socket, &me->buffer[buflen], 127 - buflen)) > 0)
      {
	buflen += status;
	me->buffer[buflen] = '\0';
	eol = strchr(me->buffer, '\n');
	if (eol) // Stop if we found newline
	  break;
      }
  if (status <= 0)
    {
      perror("read");
      return (1);
    }
  *eol = '\0';
  // Since login / uname go to sql and system, ensure they are safe
  if (strlen(login) > 8 || strlen(username) > 32 ||
      (strspn(login, "0123456789abcdefghijklmnopqrstuvwxyz_-") !=
       strlen(login)) ||
      (strspn(username, "0123456789abcdefghijklmnopqrstuvwxyz"
	      "ABCDEFGHIJKLMNOPQRSTUVWXYZ _-") != strlen(username)))
    {
      dprintf(me->socket, "Invalid datas\n");
      return (1);
    }

  // Check if the user has already registered
  t_dbuser unused;
  if (!database_getuser(login, &unused))
    {
      dprintf(me->socket, "Already registered\n");
      return (1);
    }
  return (handle_register(me, login, username));
}
