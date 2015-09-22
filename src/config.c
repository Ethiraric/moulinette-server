/*
** config.c for  in /home/sabour_f/aes
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 10:53:08 2015 Florian SABOURIN
** Last update Sat Sep 19 10:53:08 2015 Florian SABOURIN
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

// The configuration file should contain at least the following entries:
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

// Reallocates cfg->entries so it can hold newsize entries
static int realloc_config_entries(t_config *cfg, size_t newsize)
{
  t_entry *tmp;

  tmp = realloc(cfg->entries, newsize * sizeof(t_entry));
  if (!tmp)
    {
      perror("realloc");
      return (1);
    }
  cfg->alloc_entries = newsize;
  cfg->entries = tmp;
  return (0);
}

// Loads the configuration from given file
// The file must contain a list of key=value, one by line
t_config *loadconfig(const char *name)
{
  t_config *ret;
  FILE	*ifs;

  // Initialize config structure and open input stream
  ret = malloc(sizeof(t_config));
  if (!ret)
    {
      perror("malloc");
      return (NULL);
    }
  ifs = fopen(name, "r");
  if (!ifs)
    {
      free(ret);
      perror("fopen");
      return (NULL);
    }
  ret->nb_entries = 0;
  ret->alloc_entries = 0;
  ret->entries = NULL;

  // Read line by line and extract strings
  char	*line = NULL;
  size_t linesize = 0;
  t_entry *currententry;
  while (getline(&line, &linesize, ifs) > 0)
    {
      // Check if we can add another entry
      if (ret->nb_entries == ret->alloc_entries)
	if (realloc_config_entries(ret, ret->alloc_entries + 16))
	  {
	    fclose(ifs);
	    deleteconfig(ret);
	    return (NULL);
	  }

      // Fill current entry
      currententry = &ret->entries[ret->nb_entries];
      ++ret->nb_entries;
      currententry->str = line;
      currententry->key = line;

      // Check that line is valid
      line = strchr(line, '=');
      if (!line)
	{
	  fprintf(stderr, "Ill-formated config file\n");
	  fclose(ifs);
	  deleteconfig(ret);
	  return (NULL);
	}
      *line = '\0';
      currententry->value = line + 1;
      line = (char *)strchr(currententry->value, '\n');
      if (line)
	*line = '\0';

      // Prepare for next entry
      line = NULL;
      linesize = 0;
    }
  free(line);
  fclose(ifs);
  return (ret);
}

// Deallocates all resources allocated for the configuration
void	deleteconfig(t_config *cfg)
{
  size_t i;

  for (i = 0 ; i < cfg->nb_entries ; ++i)
    free(cfg->entries[i].str);
  free(cfg->entries);
  free(cfg);
}
