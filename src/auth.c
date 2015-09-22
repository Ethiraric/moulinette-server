/*
** auth.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 21:19:18 2015 Florian SABOURIN
** Last update Sat Sep 19 21:19:18 2015 Florian SABOURIN
*/

#include <curl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mouli.h"
#include "database.h"

#define CONNECT_URL  "https://intra.epitech.eu/"
#define RESPONSE_OK "Moved to <a href=\"/\">\"/\"</a>"

typedef struct s_curlnfo t_curlinfo;
struct s_curlnfo
{
  char	buff[THREAD_BUFLEN];
  size_t pos;
};

// CURL callback, where we store data into the buffer
static size_t curl_callback(char *ptr, size_t size, size_t nmemb, void *dat)
{
  t_curlinfo *info;

  info = (t_curlinfo *)dat;
  if (size * nmemb < THREAD_BUFLEN - info->pos)
    {
      strcpy(&info->buff[info->pos], ptr);
      info->pos += size * nmemb;
    }
  return (size * nmemb);
}

// Read from socket, store into buffer and update buflen
// Marks thread as finished upon error
int	perform_read(t_threadinfo *me)
{
  int ret;

  ret = read(me->socket, &me->buffer[me->buflen], THREAD_BUFLEN - me->buflen);
  if (!ret)
    return (1);
  if (ret < 0)
    {
      dprintf(me->socket, "read: %s\n", strerror(errno));
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
  if (!pos || size > 8 ||
      strspn(me->buffer, "0123456789abcdefghijklmnopqrstuvwxyz_-") != size)
    {
      dprintf(me->socket, "Invalid login\n");
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
  t_curlinfo cinfo;
  CURLcode res;
  CURL	*handler;
  char	*postargs;
  int	ret;

  ret = asprintf(&postargs, "format=json&login=%s&password=%s", login, pass);
  if (ret == -1)
    {
      perror("asprintf");
      return (1);
    }
  handler = curl_easy_init();
  if (!handler)
    {
      free(postargs);
      perror("curl_easy_init");
      return (1);
    }
  cinfo.pos = 0;
  curl_easy_setopt(handler, CURLOPT_URL, CONNECT_URL);
  curl_easy_setopt(handler, CURLOPT_POSTFIELDS, postargs);
  curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &curl_callback);
  curl_easy_setopt(handler, CURLOPT_WRITEDATA, &cinfo);
  res = curl_easy_perform(handler);
  curl_easy_cleanup(handler);
  free(postargs);
  if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform: %s\n", curl_easy_strerror(res));
      return (1);
    }
  if (strncmp(cinfo.buff, RESPONSE_OK, sizeof(RESPONSE_OK) - 1))
    return (1);
  return (0);
}

// Try to authenticate the user
int	authenticate(t_threadinfo *me)
{
  char	pass[16];
  int	passlen;

  if (read_login(me))
    return (1);
  strcpy(me->user.login, me->tmp);
  if (database_getuser(me->user.login, &me->user))
    {
      dprintf(me->socket, "You are not known from me.\n");
      return (1);
    }
  key_expansion(me->user.key, me->exp_key);
  if (read_ciphered_pass(me))
    return (1);
  inv_cipher((byte *)me->tmp, (byte *)pass, me->exp_key);
  passlen = pass[0];
  if (passlen > 15)
    {
      dprintf(me->socket, "Failed to match user and password: %d\n", passlen);
      return (1);
    }
  memmove(pass, &pass[1], passlen);
  pass[passlen] = '\0';
  if (match_login_pass(me->user.login, pass))
    {
      dprintf(me->socket, "Failed to match user and password\n");
      return (1);
    }
  return (0);
}
