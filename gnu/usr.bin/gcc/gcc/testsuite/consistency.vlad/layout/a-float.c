#include <stdio.h>

static float a [10];
static float e [0]; /* GCC only */

int main (void) {
  printf ("+++Array float:\n");
  printf ("size=%d,align=%d,5th-elem-offset=%d,5th-elem-align=%d\n",
          sizeof (a), __alignof__ (a),
          (char *) &a[5] - (char *) a, __alignof__ (a[5]));
  printf ("size=%d,align=%d,5th-elem-offset=%d,5th-elem-align=%d\n",
          sizeof (e), __alignof__ (e),
          (char *) &e[5] - (char *) a, __alignof__ (e[5]));
  return 0;
}
