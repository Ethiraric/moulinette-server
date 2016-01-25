#include <stdlib.h>
#include <stdio.h>
#include <../ex1/my_strcpy.h>

int main()
{
  char	*dst;
  char	*ret;

  dst = malloc(512);
  if (!dst)
    {
      perror("malloc");
      return (1);
    }
  ret = my_strcpy(dst, "coucou");
  printf("ret:%s | dst:%s | src:%s\n", ret, dst, "coucou");
  ret = my_strcpy(dst, "hello");
  printf("ret:%s | dst:%s | src:%s\n", ret, dst, "hello");
  ret = my_strcpy(dst, "");
  printf("ret:%s | dst:%s | src:%s\n", ret, dst, "");
  ret = my_strcpy(dst, NULL);
  printf("ret:%s | dst:%s | src:(null)\n", ret, dst);
  return (0);
}
