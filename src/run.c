/*
** run.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 19:31:38 2015 Florian SABOURIN
** Last update Sat Sep 19 19:31:38 2015 Florian SABOURIN
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "mouli.h"

// Check for finished jobs
static void check_threads(t_mouli *mouli)
{
  unsigned int i;
  void	*ret;

  for (i = 0 ; i < mouli->nthreads ; ++i)
    {
      if (mouli->threads[i]->finished)
	{
	  // If it fails, the thread has terminated (ESRCH)
	  pthread_join(mouli->threads[i]->id, &ret);
	  if (mouli->threads[i]->socket)
	    close(mouli->threads[i]->socket);
	  free(mouli->threads[i]);
	  --mouli->nthreads;
	  memmove(&mouli->threads[i], &mouli->threads[i + 1],
	      sizeof(t_threadinfo) * (mouli->nthreads - i));
	  --i;
	}
    }
}

// Make sure we can hold at least newsize threads
static int realloc_threadinfos(t_mouli *mouli, size_t newsize)
{
  t_threadinfo **tmp;

  tmp = realloc(mouli->threads, newsize * sizeof(t_threadinfo *));
  if (!tmp)
    {
      perror("realloc");
      return (1);
    }
  mouli->threads = tmp;
  mouli->allocd = newsize;
  return (0);
}

// Get new thread info, before creating a new thread
static t_threadinfo *get_new_threadinfo(int socket)
{
  t_threadinfo *info;

  info = malloc(sizeof(t_threadinfo));
  if (!info)
    {
      perror("malloc");
      return (NULL);
    }
  info->id = 0;
  info->finished = 0;
  info->socket = socket;
  info->buffer = NULL;
  info->buflen = 0;
  return (info);
}

// Accept a new client
static int on_new_client(t_mouli *mouli)
{
  t_threadinfo *info;
  int	clsock;

  if (mouli->nthreads == mouli->allocd &&
      realloc_threadinfos(mouli, mouli->allocd + 16))
    return (1);
  clsock = accept(mouli->socket, NULL, NULL);
  if (clsock == -1)
    {
      perror("accept");
      return (1);
    }
  info = get_new_threadinfo(clsock);
  if (!info)
    {
      close(clsock);
      return (1);
    }
  info->mouli = mouli;
  mouli->threads[mouli->nthreads] = info;
  ++mouli->nthreads;
  if (pthread_create(&info->id, NULL, &handle_client, info))
    {
      perror("pthread_create");
      return (1);
    }
  return (0);
}

// Main loop of the mouli
int	mouli_run(t_mouli *mouli)
{
  static char *useless[128];
  struct timeval t;
  fd_set rfds;

  FD_ZERO(&rfds);
  FD_SET(mouli->socket, &rfds);
  FD_SET(0, &rfds);
  t.tv_sec = 0;
  t.tv_usec = 100000;
  while (select(mouli->socket + 1, &rfds, NULL, NULL, &t) >= 0)
    {
      if (FD_ISSET(0, &rfds) && !read(0, useless, 128))
	return (0);
      if (FD_ISSET(mouli->socket, &rfds) && on_new_client(mouli))
	return (1);
      FD_SET(0, &rfds);
      FD_SET(mouli->socket, &rfds);
      check_threads(mouli);
      t.tv_sec = 0;
      t.tv_usec = 100000;
    }
  perror("select");
  return (1);
}
