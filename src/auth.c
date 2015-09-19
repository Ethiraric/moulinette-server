/*
** auth.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 21:19:18 2015 Florian SABOURIN
** Last update Sat Sep 19 21:19:18 2015 Florian SABOURIN
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mouli.h"
#include "database.h"

// Read from socket, store into buffer and update buflen
// Marks thread as finished upon error
int	perform_read(t_threadinfo *me)
{
  int ret;

  ret = read(me->socket, &me->buffer[me->buflen], 512 - me->buflen);
  if (ret <= 0)
    {
      dprintf(me->socket, "read: %s\n", strerror(errno));
      me->finished = 1;
      return (1);
    }
  me->buflen += ret;
  return (0);
}

// Keep on reading as long as we don't have the full login
// When found, copy to me->tmp
static int read_login(t_threadinfo *me)
{
  size_t size;
  void	*pos;

  while (!(pos = memchr(me->buffer, '\n', me->buflen)) && me->buflen < 16)
    if (perform_read(me))
      return (1);
  size = (char *)(pos) - me->buffer;
  if (size > 8)
    {
      dprintf(me->socket, "Invalid login\n");
      me->finished = 1;
      return (1);
    }
  memcpy(me->tmp, me->buffer, size);
  me->tmp[size] = '\0';
  me->buflen -= size + 1;
  memmove(me->buffer, &me->buffer[size + 1], me->buflen);
  return (0);
}

// Read the ciphered pass
// When found, copy to me->tmp
static int read_ciphered_pass(t_threadinfo *me)
{
  while (me->buflen < 16)
    if (perform_read(me))
      return (1);
  memcpy(me->tmp, me->buffer, 16);
  me->buflen -= 16;
  memmove(me->buffer, &me->buffer[16], me->buflen);
  return (0);
}

// Ensure that the login/password combination is correct
static int match_login_pass(char *login, char *pass)
{
  // Hehe, what should I do here
  (void)(login);
  (void)(pass);
  return (0);
}

// Try to authenticate the user
int	authenticate(t_threadinfo *me)
{
  char	pass[16];
  char	login[16];
  char	*key;

  if (read_login(me))
    return (1);
  strcpy(login, me->tmp);
  key = database_getkey(me->tmp);
  if (!key)
    {
      dprintf(me->socket, "You are not known from me.\n");
      me->finished = 1;
      return (1);
    }
  memcpy(me->key, key, 32);
  key_expansion(me->key, me->exp_key);
  if (read_ciphered_pass(me))
    return (1);
  inv_cipher((byte *)me->buffer, (byte *)pass, me->exp_key);
  printf("Trying to authenticate %s/%s\n", login, pass);
  if (!match_login_pass(login, pass))
    {
      dprintf(me->socket, "Invalid user or password\n");
      me->finished = 1;
      return (1);
    }
  return (0);
}
