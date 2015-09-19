/*
** database.c for  in /home/sabour_f/github/moulinette-server/src
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 18:50:28 2015 Florian SABOURIN
** Last update Sat Sep 19 18:50:29 2015 Florian SABOURIN
*/

#include <stdlib.h>
#include <sqlite3.h>

static sqlite3 *dbhandler = NULL;

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
