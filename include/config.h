/*
** config.h for  in /home/sabour_f/aes
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 10:53:06 2015 Florian SABOURIN
** Last update Sat Sep 19 10:53:07 2015 Florian SABOURIN
*/

#ifndef CONFIG_H_
# define CONFIG_H_

# include <sys/types.h>

typedef struct s_entry t_entry;
struct s_entry
{
  char	*key; // Key (before the '=')
  char	*value; // Value (after the '=')
  char	*str; // For optimizing purpose, key and value may not point to
	      // allocated strings. So this is the argument to pass to free()
	      // to free both the key and the value
};

// Contains a list of key-value strings extracted from a configuration file
typedef struct s_config t_config;
struct s_config
{
  size_t	nb_entries; // Number of entries in entries
  size_t	alloc_entries; // Number of entries allocated in entries
  t_entry	*entries; // Dynamic array of entries
};

t_config	*loadconfig(const char *name);
void		deleteconfig(t_config *cfg);

#endif /* !CONFIG_H_ */
