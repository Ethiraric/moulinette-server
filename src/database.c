/*
** database.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 18:50:28 2015 Florian SABOURIN
** Last update Sat Sep 19 18:50:29 2015 Florian SABOURIN
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"

static sqlite3 *dbhandler = NULL;

// Does nothing
// Used in insert statements as a callback to sqlite
static int	callback_nothing(void *param, int argc, char **argv,
				 char **column_names)
{
  (void)(param);
  (void)(argc);
  (void)(argv);
  (void)(column_names);
  return (0);
}

// Initializes the database connection
int	database_init(const char *filename)
{
  if (sqlite3_open(filename, &dbhandler) != SQLITE_OK)
    return (1);
  return (0);
}

// Closes the database connection
int	database_close()
{
  if (sqlite3_close(dbhandler) != SQLITE_OK)
    {
      dbhandler = NULL;
      return (1);
    }
  dbhandler = NULL;
  return (0);
}

// Returns a string describing why the last operation failed
const char *database_geterror()
{
  return (sqlite3_errmsg(dbhandler));
}

// Returns all informations about a user
int	database_getuser(const char *login, t_dbuser *dst)
{
  static char request[64]; // 46 chars max
  sqlite3_stmt *stmt;
  const char *name;
  const char *key;
  int left; // Number of bytes left in key
  int	i;
  int	cols;

  sprintf(request, "SELECT * FROM `auth` WHERE `login`='%s';", login);
  if (sqlite3_prepare(dbhandler, request, -1, &stmt, NULL) != SQLITE_OK)
    {
      fprintf(stderr, "sqlite3_prepare: %s\n", sqlite3_errmsg(dbhandler));
      return (1);
    }
  cols = sqlite3_column_count(stmt);
  if (sqlite3_step(stmt) != SQLITE_ROW) // Nothing found in db
    {
      sqlite3_finalize(stmt);
      return (1);
    }
  for (i = 0 ; i < cols ; ++i)
    {
      name = (const char *)sqlite3_column_name(stmt, i);
      if (!strcmp(name, "id"))
	dst->id = atoi((const char *)(sqlite3_column_text(stmt, i)));
      else if (!strcmp(name, "key"))
	{
	  key = (const char *)(sqlite3_column_text(stmt, i));
	  for (left = 0 ; left < 32 ; ++left)
	    {
	      char dig1, dig2; // The two digits
	      dig1 = key[left * 2];
	      dig2 = key[left * 2 + 1];
	      // Digit -> value
	      dig1 = dig1 >= '0' && dig1 <= '9' ? dig1 - '0' : dig1 - 'a' + 10;
	      dig2 = dig2 >= '0' && dig2 <= '9' ? dig2 - '0' : dig2 - 'a' + 10;
	      dst->key[left] = ((dig1 & 0x0F) << 4) | (dig2 & 0x0F);
	    }
	}
    }
  sqlite3_finalize(stmt);
  return (0);
}

// Adds a new user to the database
int	database_new_user(const char *login, const char *unam, const char *key)
{
  char	*err;
  char	*req;
  int	ret;

  ret = asprintf(&req, "INSERT INTO `auth` (`login`, `rg_username`, `rg_time`,"
		       " `key`) VALUES ('%s', '%s', CURRENT_TIMESTAMP, '%s');",
		 login, unam, key);
  if (ret == -1)
    {
      perror("asprintf");
      return (1);
    }
  ret = sqlite3_exec(dbhandler, req, &callback_nothing, NULL, &err);
  free(req);
  if (ret != SQLITE_OK)
    {
      fprintf(stderr, "sqlite3_exec: %s\n", sqlite3_errmsg(dbhandler));
      return (1);
    }
  return (0);
}
