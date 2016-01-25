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
#include <unistd.h>
#include "mouli.h"

// A buffer for the moulinette output
typedef struct s_buffer t_buffer;
struct s_buffer
{
  size_t buflen;
  size_t bufallocd;
  char	*buffer;
};

// Append data to buffer, reallocate if necessary
static int buffer_append(t_buffer *buffer, char *append, size_t appsiz)
{
  char	*tmp;

  if (buffer->buflen + appsiz > buffer->bufallocd)
    {
      if (!(tmp = (char *)realloc(buffer->buffer, buffer->bufallocd + appsiz)))
	{
	  perror("realloc");
	  return (1);
	}
      buffer->buffer = tmp;
      buffer->bufallocd += appsiz;
    }
  memcpy(&buffer->buffer[buffer->buflen], append, appsiz);
  buffer->buflen += appsiz;
  return (0);
}

// Call popen, with a printf-formated string (with mode "r")
// It pipe()s and fork()s so we can read the subprocess standard output
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
    perror("popen");
  return (ifs);
}

// Read the repository name and store it in me->buffer
// Checks it does not contain character which may not be safely passed to
// system()
static int read_repo(t_threadinfo *me)
{
  size_t size;
  void	*pos;

  // Read while there is no '\n'
  while (!(pos = memchr(me->buffer, '\n', me->buflen)) &&
	 me->buflen < 256)
    if (perform_read(me))
      return (1);

  // Change '\n' by a '\0' so we can consider the repository as a string
  size = (char *)(pos) - me->buffer;
  if (pos)
    *(char *)(pos) = '\0';
  if (!pos || size > 256 ||
      strspn(me->buffer, "0123456789abcdefghijklmnopqrstuvwxyz"
	     "ABCDEFGHIJKLMNOPQRSTUVWXYZ_-") != size)
    {
      dprintf(me->socket, "Invalid repo\n");
      return (1);
    }
  return (0);
}

// Write loop. Ensures len bytes are written to fd
static int write_loop(int fd, char *buffer, size_t len)
{
  int	ret;

  while (len)
    {
      ret = write(fd, buffer, len);
      if (ret <= 0)
	{
	  perror("write");
	  return (1);
	}
      buffer += ret;
      len -= ret;
    }
  return (len);
}

// Call git clone/pull for the user's repository
static int update_repo(t_threadinfo *me)
{
  char	outbuffer[THREAD_BUFLEN];
  FILE	*ifs;
  int	status;

  // Get repository name
  if (read_repo(me))
    return (1);

  // Create subprocess
  ifs = my_popen("./clone.sh \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
		 me->user.login, me->buffer, me->mouli->clone_subfolder,
		 me->mouli->tests_subfolder, me->mouli->clone_login);
  if (!ifs)
    {
      dprintf(me->socket, "Internal error\n");
      return (1);
    }

  // Get output in case the clone is not successful
  while ((status = fread(outbuffer, 1, THREAD_BUFLEN - 1, ifs)))
    if (write_loop(me->socket, outbuffer, status))
      return (1);

  // Close the subprocess pipe
  status = pclose(ifs);
  if (status == -1)
    {
      if (status == -1)
	perror("pclose");
      dprintf(me->socket, "Internal error\n");
      return (1);
    }

  // Get subprocess exit status, and send error to the student, if any
  status = WEXITSTATUS(status);
  if (status)
    return (1);
  return (0);
}

// Run the tests
// The file must be executable, and print to stdout the results
// If it runs another executable (e.g. gcc), it must display the error output
// on its own standard output if it wants it to be sent to the student
static int run_tests(t_threadinfo *me)
{
  t_buffer resbuffer;
  FILE	*ifs;
  char	buff[THREAD_BUFLEN];
  char	*markpos;
  int	status;
  int	hasread;

  // Open pipe with executable
  ifs = my_popen("(cd ./%s/%s/%s/.tests && ./%s)", me->mouli->clone_subfolder,
		 me->user.login, me->buffer, me->mouli->tests_filename);
  if (!ifs)
    {
      dprintf(me->socket, "Failed to exec tests (severe)\n");
      return (1);
    }

  // Read output until the end, and send it to the student
  resbuffer.bufallocd = 0;
  resbuffer.buflen = 0;
  resbuffer.buffer = NULL;
  hasread = 0;
  while ((status = fread(buff, 1, THREAD_BUFLEN, ifs)))
    {
      if (buffer_append(&resbuffer, buff, status))
	{
	  free(resbuffer.buffer);
	  return (1);
	}
      hasread = 1;
    }

  if (write_loop(me->socket, resbuffer.buffer, resbuffer.buflen))
    {
      free(resbuffer.buffer);
      return (1);
    }

  markpos = memmem(resbuffer.buffer, resbuffer.buflen, "Mark:", 5);
  if (markpos)
    hasread = atoi(markpos + 5);
  else
    {
      printf("Failed to find markpos\n");
      markpos = 0;
    }
  free(resbuffer.buffer);
  database_log(&me->user, me->buffer, hasread);

  // Close pipe
  status = pclose(ifs);
  if (status == -1 || !hasread)
    {
      if (status == -1)
	perror("pclose");
      dprintf(me->socket, "Failed to exec tests (severe)\n");
      return (1);
    }
  return (0);
}

// The first 8 bytes are the command the user wants to execute
// "register" or "mouli\x00\x00\x00"
static int readcmd(t_threadinfo *me)
{
  size_t left;
  int	status;

  // Loop until 8 bytes are read
  left = 8;
  while (left && (status = read(me->socket, &me->buffer[8 - left], left)) > 0)
    left -= status;

  // Display error if read fails, just return if the client disconnected
  if (status < 0)
    {
      perror("read");
      return (1);
    }
  if (!status)
    return (1);

  me->buffer[8] = '\0';
  return (0);
}

// Thread function handling a client
// Read informations about what command it wants to run (mouli / register)
// mouli -> authenticate, upadte repo, run test, send output
// register -> get login, add entry in db, send mail
// REMEMBER to set me->finished to something evaluating to true, so the main
// thread can free resources
void	*handle_client(void *arg)
{
  t_threadinfo *me; // It's informations about me, myself
  char	buffer[THREAD_BUFLEN];

  me = (t_threadinfo *)(arg);
  me->buffer = buffer;

  // Read command
  if (readcmd(me))
    {
      me->finished = 1;
      return (NULL);
    }

  // If the user wants to register
  if (!memcmp(me->buffer, "register", 8))
    {
      client_register(me);
      me->finished = 1;
      return (NULL);
    }

  // If the user wants a moulinette
  if (!memcmp(me->buffer, "mouli\x00\x00\x00", 8))
    if (authenticate(me) || update_repo(me) || run_tests(me))
      {
	me->finished = 1;
	return (NULL);
      }
  me->finished = 1;
  return (NULL);
}
