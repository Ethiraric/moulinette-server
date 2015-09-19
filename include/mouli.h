/*
** mouli.h for  in /home/sabour_f/github/moulinette-server/include
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sat Sep 19 18:22:04 2015 Florian SABOURIN
** Last update Sat Sep 19 18:22:04 2015 Florian SABOURIN
*/

#ifndef MOULI_H_
# define MOULI_H_

# include <pthread.h>
# include <stdint.h>

// Defined for convenience
typedef unsigned char byte;

// Thread information
typedef struct s_threadinfo t_threadinfo;
struct s_threadinfo
{
  pthread_t id;
  int	finished;
  int	socket;
  char	*buffer; // Input buffer, what is read from socket
  size_t buflen; // Bytes of data in buffer
  char	tmp[128]; // Temporary buffer, used by some thread functions
  byte	exp_key[256]; // Expanded AES key (240 bytes)
  byte	key[32];
};

// Main moulinette structure
typedef struct s_mouli t_mouli;
struct	s_mouli
{
  t_threadinfo **threads;
  size_t nthreads;
  size_t allocd;
  int	socket;
};

// AES utilities
void	key_expansion(uint8_t *key, uint8_t *w);
void	cipher(uint8_t *in, uint8_t *out, uint8_t *w);
void	inv_cipher(uint8_t *in, uint8_t *out, uint8_t *w);

int	mouli_run(t_mouli *mouli);
void	*handle_client(void *arg);
int	authenticate(t_threadinfo *me);

// Helper functions
int	perform_read(t_threadinfo *me);

#endif /* !MOULI_H_ */
