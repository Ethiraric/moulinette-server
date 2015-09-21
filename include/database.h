/*
** database.h for  in /home/sabour_f/github/moulinette-server/include
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 18:51:03 2015 Florian SABOURIN
** Last update Sat Sep 19 18:51:04 2015 Florian SABOURIN
*/

#ifndef DATABASE_H_
# define DATABASE_H_

// Defined for convenience
typedef unsigned char byte;

typedef struct s_dbuser t_dbuser;
struct s_dbuser
{
  unsigned int id;
  char	login[8];
  byte	key[32];
};

int	database_init(const char *filename);
const char *database_geterror();
int	database_close();
int	database_getuser(const char *login, t_dbuser *dst);

#endif /* !DATABASE_H_ */
