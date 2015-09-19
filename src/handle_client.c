/*
** handle_client.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 19:55:52 2015 Florian SABOURIN
** Last update Sat Sep 19 19:55:52 2015 Florian SABOURIN
*/

#include <stdlib.h>
#include "mouli.h"

// Thread function handling a client
void	*handle_client(void *arg)
{
  t_threadinfo *me; // It's informations about me, myself
  char	buffer[512];

  me = (t_threadinfo *)(arg);
  me->buffer = buffer;
  if (authenticate(me))
    return (NULL);
  me->finished = 1;
  return (NULL);
}
