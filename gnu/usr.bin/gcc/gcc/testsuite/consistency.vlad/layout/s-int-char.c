#include <stdio.h>

static struct sss{
  int f;
  char snd;
} sss;

#define _offsetof(st,f) ((char *)&((st *) 16)->f - (char *) 16)

int main (void) {
  printf ("+++Struct int-char:\n");
  printf ("size=%d,align=%d,offset-int=%d,offset-char=%d,\nalign-int=%d,align-char=%d\n",
          sizeof (sss), __alignof__ (sss),
          _offsetof (struct sss, f), _offsetof (struct sss, snd),
          __alignof__ (sss.f), __alignof__ (sss.snd));
  return 0;
}
