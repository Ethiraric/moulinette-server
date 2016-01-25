#include <string.h>

char *my_strcpy(char *dst, char *src)
{
  if (dst && src)
    strcpy(dst, src);
  return (dst);
}
