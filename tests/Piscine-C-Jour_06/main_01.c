/*
** main_01.c for  in /home/sabour_f/github/moulinette-server/tests/Piscine-C-Jour_06
**
** Made by Florian SABOURIN
** Login   <sabour_f@epitech.net>
**
** Started on  Sun Sep 20 19:29:07 2015 Florian SABOURIN
** Last update Mon Sep 21 10:29:33 2015 Florian SABOURIN
*/

#include <stdio.h>
#include <string.h>

char *my_strcpy(char *dst, char *src);

void run_test(const char *dst, const char *src)
{
  static char fdst[BUFSIZ];
  static char fsrc[BUFSIZ];
  char *ret;

  strcpy(fdst, dst);
  strcpy(fsrc, src);
  printf("my_strcpy(\"%s\", \"%s\"): ", fdst, fsrc);
  ret = my_strcpy(fdst, fsrc);
  printf("(\"%s\", \"%s\", \"%s\")\n", ret, fdst, fsrc);
}

int main()
{
  printf("Results are in the following order: (return, dst, src)\n");
  run_test("toto", "tata");
  return (0);
}
