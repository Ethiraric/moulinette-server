/*
** handle_client.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 19:55:52 2015 Florian SABOURIN
** Last update Sat Sep 19 19:55:52 2015 Florian SABOURIN
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mouli.h"

// Call system, with a printf-formated string
#ifdef __GNUC__
__attribute__ ((format (printf, 1, 2)))
#endif
static FILE *my_popen(const char *fmt, ...)
{
  va_list va;
  FILE	*ifs;
  char	*command;
  int	ret;

  va_start(va, fmt);
  ret = vasprintf(&command, fmt, va);
  va_end(va);
  if (ret == -1)
    {
      perror("vasprintf");
      return (NULL);
    }
  printf("popening: %s\n", command);
  ifs = popen(command, "r");
  free(command);
  if (!ifs)
    {
      perror("popen");
      return (NULL);
    }
  return (ifs);
}

// Read the repository
static int read_repo(t_threadinfo *me)
{
  size_t size;
  void	*pos;

  while (!(pos = memchr(me->buffer, '\n', me->buflen)) &&
	 me->buflen < 256)
    if (perform_read(me))
      return (1);
  size = (char *)(pos) - me->buffer;
  if (pos)
    *(char *)(pos) = '\0';
  if (!pos || size > 256 ||
      strspn(me->buffer, "0123456789abcdefghijklmnopqrstuvwxyz"
	     "ABCDEFGHIJKLMNOPQRSTUVWXYZ_-") != size)
    {
      dprintf(me->socket, "Invalid repo\n");
      me->finished = 1;
      return (1);
    }
  return (0);
}

// Call git clone/pull for the user's repository
static int update_repo(t_threadinfo *me)
{
  size_t pos;
  char	outbuffer[THREAD_BUFLEN];
  FILE	*ifs;
  int	status;

  if (read_repo(me))
    return (1);
  ifs = my_popen("./clone.sh \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" 2> /dev/null",
		 me->login, me->buffer, CLONE_SUBFOLDER, TESTS_SUBFOLDER,
		 CLONE_LOGIN);
  pos = 0;
  while (fread(&outbuffer[pos], THREAD_BUFLEN - pos - 1, 1, ifs))
    ;
  status = pclose(ifs);
  if (status == -1)
    {
      perror("pclose");
      return (1);
    }
  status = WEXITSTATUS(status);
  if (status)
    {
      dprintf(me->socket, "%s", outbuffer);
      return (1);
    }
  return (0);
}

// Thread function handling a client
void	*handle_client(void *arg)
{
  t_threadinfo *me; // It's informations about me, myself
  char	buffer[THREAD_BUFLEN];

  me = (t_threadinfo *)(arg);
  me->buffer = buffer;
  if (authenticate(me) || update_repo(me))
    {
      me->finished = 1;
      return (NULL);
    }
  me->finished = 1;
  return (NULL);
}
